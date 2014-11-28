#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <boost/numpy.hpp>
#include <numpy/arrayobject.h>
#include <complex>


#include <boost/python.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/to_python_converter.hpp>

#include <boost/python/object.hpp>
#include <boost/python/str.hpp>
#include <boost/python/extract.hpp>

#include <boost/optional.hpp>
#include <boost/utility/typed_in_place_factory.hpp>

#include <boost/filesystem.hpp>


#include <istream>
#include <streambuf>
#include <iostream>

#include <utUtil/Exception.h>
#include <utMath/ErrorPose.h>
#include <utAlgorithm/Projection.h>
#include <utAlgorithm/ToolTip/TipCalibration.h>
#include <utMath/VectorFunctions.h>
#include <utAlgorithm/PoseEstimation3D3D/AbsoluteOrientation.h>
#include <utAlgorithm/PoseEstimation3D3D/CovarianceEstimation.h>


using namespace Ubitrack;

namespace bp = boost::python;
namespace bn = boost::numpy;

namespace {


Math::Matrix< double, 4, 4 > projectionMatrix3x3ToOpenGL( double l, double r, double b, double t, double n, double f, Math::Matrix< double, 3, 3 > m ) {
	return Algorithm::projectionMatrixToOpenGL( l, r, b, t, n, f, m );
}

Math::Matrix< double, 4, 4 > projectionMatrix3x4ToOpenGL( double l, double r, double b, double t, double n, double f, Math::Matrix< double, 3, 4 > m ) {
	return Algorithm::projectionMatrixToOpenGL( l, r, b, t, n, f, m );
}

Math::Vector< double, 3 > tipCalibration( const std::vector< Ubitrack::Math::Pose > &poses) {
	Ubitrack::Math::Vector< double, 3 > pm;
	Ubitrack::Math::Vector< double, 3 > pw;
	Algorithm::ToolTip::tipCalibration(poses, pm, pw);
	return pm;
}

Math::Pose tipCalibrationPose( const std::vector< Ubitrack::Math::Pose > &poses) {
	Ubitrack::Math::Vector< double, 3 > pm;
	Ubitrack::Math::Vector< double, 3 > pw;
	Algorithm::ToolTip::tipCalibration(poses, pm, pw);

	double length = norm_2(pm);
	Math::Vector< double, 3 > refPos = -pm / length;
	Math::Vector< double, 3 > errPos(0,0,1);

	// 1. Calc normalized correction axis
	Math::Vector< double, 3 > axis = cross_product(errPos, refPos);
	// 2. Calc the correction angle
	double angle = acos (inner_prod(errPos, refPos) / ( norm_2(errPos) * norm_2(refPos)));
	// 3. Calc orientational correction. The axis will be normed by the constructor
	Math::Quaternion corrRot( axis, angle);

	Math::Pose p(corrRot, pm);
	return p;
}

Math::Pose estimatePose6D_3D3D(const std::vector< Ubitrack::Math::Vector< double, 3> > &pointsA, const std::vector< Ubitrack::Math::Vector< double, 3> > &pointsB) {
	Math::Pose resultPoseBA;
	bool success = Algorithm::PoseEstimation3D3D::estimatePose6D_3D3D(pointsB, resultPoseBA, pointsA);
	if (!success) {
		UBITRACK_THROW( "Cannot estimate a result pose, some problem occured while performing a 3d-3d pose estimation (absolute orientation)");
	}
	return resultPoseBA;
}

Math::ErrorPose estimatePose6D_3D3D_Error(const std::vector< Ubitrack::Math::Vector< double, 3> > &pointsA, const std::vector< Ubitrack::Math::Vector< double, 3> > &pointsB) {
	Math::Pose resultPoseBA;
	bool success = Algorithm::PoseEstimation3D3D::estimatePose6D_3D3D(pointsB, resultPoseBA, pointsA);
	if (!success) {
		UBITRACK_THROW( "Cannot estimate a result pose, some problem occured while performing a 3d-3d pose estimation (absolute orientation) ");
	}

	Ubitrack::Math::Matrix< double, 6, 6> cov = Ubitrack::Math::Matrix< double, 6, 6>::zeros();
	success = Algorithm::PoseEstimation3D3D::estimatePose6DCovariance(pointsA.begin(), pointsA.end(), resultPoseBA, pointsB.begin(), pointsB.end(), cov);
	if (!success) {
		UBITRACK_THROW( "Cannot estimate a result, some problem occured while calculating the covariance");
	}

	Ubitrack::Math::ErrorPose ep(resultPoseBA, cov);
	return ep;
}

} // end anon ns


BOOST_PYTHON_MODULE(_utcalibration)
{
	// initialize boost.numpy
	bn::initialize();

	bp::def("projectionMatrix3x3ToOpenGL", &projectionMatrix3x3ToOpenGL);
	bp::def("projectionMatrix3x4ToOpenGL", &projectionMatrix3x4ToOpenGL);
	bp::def("tipCalibration", &tipCalibration);
	bp::def("tipCalibrationPose", &tipCalibrationPose);
	bp::def("absoluteOrientation", &estimatePose6D_3D3D);
	bp::def("absoluteOrientationError", &estimatePose6D_3D3D_Error);
}
