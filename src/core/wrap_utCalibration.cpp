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

#include <utCalibration/Projection.h>

using namespace Ubitrack;

namespace bp = boost::python;
namespace bn = boost::numpy;

namespace {


Math::Matrix< 4, 4, double > projectionMatrix3x3ToOpenGL( double l, double r, double b, double t, double n, double f, Math::Matrix< 3, 3, double > m ) {
	return Calibration::projectionMatrixToOpenGL( l, r, b, t, n, f, m );
}

Math::Matrix< 4, 4, double > projectionMatrix3x4ToOpenGL( double l, double r, double b, double t, double n, double f, Math::Matrix< 3, 4, double > m ) {
	return Calibration::projectionMatrixToOpenGL( l, r, b, t, n, f, m );
}


} // end anon ns


BOOST_PYTHON_MODULE(_utcalibration)
{
	// initialize boost.numpy
	bn::initialize();

	bp::def("projectionMatrix3x3ToOpenGL", &projectionMatrix3x3ToOpenGL);
	bp::def("projectionMatrix3x4ToOpenGL", &projectionMatrix3x4ToOpenGL);

}
