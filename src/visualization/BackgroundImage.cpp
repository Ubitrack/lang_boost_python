/*
 * BackgroundImage.cpp
 *
 *  Created on: 25/11/2013
 *      Author: jack
 */
/*
 * Ubitrack - Library for Ubiquitous Tracking
 * Copyright 2006, Technische Universitaet Muenchen, and individual
 * contributors as indicated by the @authors tag. See the
 * copyright.txt in the distribution for a full listing of individual
 * contributors.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */

#include "ubitrack_python/BackgroundImage.h"

namespace Ubitrack { namespace Python {

BackgroundImage::BackgroundImage( )
	: m_bTextureInitialized( false )
	, m_lastImageTimestamp(0)
{
}


BackgroundImage::~BackgroundImage()
{
}


/** deletes OpenGL state */
void BackgroundImage::glCleanup()
{
	//LOG4CPP_DEBUG( logger, "glCleanup() called" );

	if ( m_bTextureInitialized ) {
 		glBindTexture( GL_TEXTURE_2D, 0 );
 		glDisable( GL_TEXTURE_2D );
 		glDeleteTextures( 1, &m_texture );
 	}
}


/** render the object */
void BackgroundImage::draw( int m_width, int m_height )
{
	// Disable transparency for background image. The Transparency
	// module might have enabled global transparency for the virtual
	// scene.  We have to restore this state below.
	glDisable( GL_BLEND );

	// check if we have an image to display as background
	if ( m_image.get() == 0 ) return;

	// store the projection matrix
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();

	// create a 2D-orthogonal projection matrix
	glLoadIdentity();
	gluOrtho2D( 0.0, m_width, 0.0, m_height );


	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	// prepare fullscreen bitmap without fancy extras
	GLboolean bLightingEnabled = glIsEnabled( GL_LIGHTING );
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );

	// lock it to avoid random crashes
	boost::mutex::scoped_lock l( m_imageLock );

	// find out texture format
	GLenum imgFormat = GL_LUMINANCE;
	switch ( m_image->nChannels ) {
		case 1: imgFormat = GL_LUMINANCE; break;
#ifndef GL_BGR_EXT
		case 3: imgFormat = GL_RGB; break;
#else
		case 3:
			if ( m_image->channelSeq[ 0 ] == 'B' && m_image->channelSeq[ 1 ] == 'G' && m_image->channelSeq[ 2 ] == 'R' )
				imgFormat = GL_BGR_EXT;
			else
				imgFormat = GL_RGB;
			break;
#endif
	}

	// texture version
	glEnable( GL_TEXTURE_2D );

	if ( !m_bTextureInitialized )
	{
		m_bTextureInitialized = true;

		// generate power-of-two sizes
		m_pow2Width = 1;
		while ( m_pow2Width < (unsigned)m_image->width )
			m_pow2Width <<= 1;

		m_pow2Height = 1;
		while ( m_pow2Height < (unsigned)m_image->height )
			m_pow2Height <<= 1;

		// create new empty texture
		glGenTextures( 1, &m_texture );
		glBindTexture( GL_TEXTURE_2D, m_texture );

		// define texture parameters
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

		// load empty texture image (defines texture size)
		glTexImage2D( GL_TEXTURE_2D, 0, 3, m_pow2Width, m_pow2Height, 0, imgFormat, GL_UNSIGNED_BYTE, 0 );
		//LOG4CPP_DEBUG( logger, "glTexImage2D( width=" << m_pow2Width << ", height=" << m_pow2Height << " ): " << glGetError() );
	}

	// load image into texture
	glBindTexture( GL_TEXTURE_2D, m_texture );

	// only update texture if lastTS < image.time()
	if (m_lastImageTimestamp < m_image.time()) {
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, m_image->width, m_image->height,
			imgFormat, GL_UNSIGNED_BYTE, m_image->imageData );
	}

	// display textured rectangle
	double y0 = m_image->origin ? 0 : m_height;
	double y1 = m_height - y0;
	double tx = double( m_image->width ) / m_pow2Width;
	double ty = double( m_image->height ) / m_pow2Height;

	// draw two triangles
	glBegin( GL_TRIANGLE_STRIP );
	glTexCoord2d(  0, ty ); glVertex2d(       0, y1 );
	glTexCoord2d(  0,  0 ); glVertex2d(       0, y0 );
	glTexCoord2d( tx, ty ); glVertex2d( m_width, y1 );
	glTexCoord2d( tx,  0 ); glVertex2d( m_width, y0 );
	glEnd();

	glDisable( GL_TEXTURE_2D );

	// change timestamp to image time
	m_lastImageTimestamp = m_image.time();

	// restore opengl state
	glEnable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );
	if ( bLightingEnabled )
		glEnable( GL_LIGHTING );


	glPopMatrix();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );

}

/**
 * callback from Image port
 * passes image to the parent module
 * @param img an image Measurement
 */
void BackgroundImage::imageIn( const Ubitrack::Measurement::ImageMeasurement& img )
{
	//LOG4CPP_DEBUG( logger, "received background image with timestamp " << img.time() );
	boost::mutex::scoped_lock l( m_imageLock );
	if(img->depth == IPL_DEPTH_32F){
		boost::shared_ptr<Ubitrack::Vision::Image> p(new Ubitrack::Vision::Image(img->width , img->height , 1, IPL_DEPTH_8U ));
		float* depthData = (float*) img->imageData;
		unsigned char* up =(unsigned char*) p->imageData;
		for(unsigned int i=0;i<img->width*img->height;i++)
			if(depthData[i] != depthData[i])
				up[i] = 0;
			else
				up[i] = depthData[i]*255;

		m_image = Ubitrack::Measurement::ImageMeasurement(img.time(), p);
	} else
		m_image = img;

	//m_pModule->invalidate( this );
}

} } // namespace Ubitrack::Python





