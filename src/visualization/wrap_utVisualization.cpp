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
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>


#include <istream>
#include <streambuf>
#include <iostream>

#include <utMeasurement/Measurement.h>
#include <utVision/Image.h>

#include "opencv2/core/core.hpp"


#include <ubitrack_python/wrap_streambuf.h>

#include "ubitrack_python/BackgroundImage.h"

using namespace Ubitrack;
using namespace Ubitrack::Python;

namespace bp = boost::python;
namespace bn = boost::numpy;

namespace {




} // end anon ns


BOOST_PYTHON_MODULE(_utvisualization)
{
	// initialize boost.numpy
	bn::initialize();

	bp::class_< BackgroundImage, boost::shared_ptr<BackgroundImage>, boost::noncopyable >("BackgroundImage", bp::init<>())
			.def("draw", &BackgroundImage::draw)
			.def("imageIn", &BackgroundImage::imageIn)
			.def("glCleanup", &BackgroundImage::glCleanup)
			;

}
