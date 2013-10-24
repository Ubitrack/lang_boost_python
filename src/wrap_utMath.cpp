#include "pyublas/numpy.hpp"

#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/to_python_converter.hpp>

#include <utMath/Vector.h>
#include <utMath/Matrix.h>
#include <utMath/Quaternion.h>
#include <utMath/Pose.h>

using namespace boost::python;
using namespace Ubitrack;

namespace {


// Converts a vector < n > to numpy array.
template <typename T1>
struct vector_to_array
{
  static PyObject* convert(T1 const& p)
  {
	  pyublas::numpy_vector<double> ph(p);
	  boost::python::object obj(ph.to_python());
	  return boost::python::incref(obj.ptr());
  }
};

struct vector_to_python_converter
{
	template< typename VT >
	vector_to_python_converter& to_python()
	{
		boost::python::to_python_converter<
			VT,
			vector_to_array<VT>,
			false //vector_to_array has no get_pytype
			>();
		return *this;
	}
};



// Converts a vector < n > to numpy array.
template <typename T1>
struct matrix_to_ndarray
{
  static PyObject* convert(T1 const& p)
  {
	  pyublas::numpy_matrix<double> ph(p);
	  boost::python::object obj(ph.to_python());
	  return boost::python::incref(obj.ptr());
  }
};

struct matrix_to_python_converter
{
	template< typename MT >
	matrix_to_python_converter& to_python()
	{
		boost::python::to_python_converter<
			MT,
			matrix_to_ndarray<MT>,
			false //vector_to_array has no get_pytype
			>();
		return *this;
	}
};


}

Math::Vector< 4 > test_vec4() {
	return Math::Vector< 4 >(1,2,2,1.2);
}


Math::Matrix< 3, 3 > test_mat33() {
	Math::Quaternion q;
	Math::Matrix< 3, 3 > m;
	q.toMatrix(m);
	return m;
}

Math::Quaternion test_quat() {
	Math::Quaternion q;
	return q;
}


BOOST_PYTHON_MODULE(ubitrack_python)
{
	vector_to_python_converter()
			.to_python< Math::Vector< 2 > >()
			.to_python< Math::Vector< 3 > >()
			.to_python< Math::Vector< 4 > >()
			.to_python< Math::Vector< 5 > >()
			.to_python< Math::Vector< 6 > >()
			.to_python< Math::Vector< 7 > >()
			.to_python< Math::Vector< 8 > >();

	matrix_to_python_converter()
			.to_python< Math::Matrix < 2, 2 > >()
			.to_python< Math::Matrix < 3, 3 > >()
			.to_python< Math::Matrix < 4, 4 > >()
			.to_python< Math::Matrix < 3, 4 > >();


    class_<Math::Quaternion>("Quaternion", init< Math::Vector< 3,double >, double >())
		.def(init< double, double, double >())
		.def(init< double, double, double, double >())
        .def("x", &Math::Quaternion::x)
        .def("y", &Math::Quaternion::y)
        .def("z", &Math::Quaternion::z)
        .def("w", &Math::Quaternion::w)
        //.def("normalize", (Math::Quaternion& (Math::Quaternion::*)())&Math::Quaternion::normalize)
        //.def("invert", &Math::Quaternion::invert)
        .def("angle", &Math::Quaternion::angle)

        // example for static method ..
        //.def("zeros", (Vector2 (*)())&Vector2::zeros)
    ;

    class_<Math::Pose>("Pose", init<  Math::Quaternion, Math::Vector< 3 > >())
		//.def(init< boost::numeric::ublas::matrix< double > >())
		//.add_property("rotation", &Math::Pose::rotation)
		//.add_property("translation", &Math::Pose::translation)
        .def("scalePose", &Math::Pose::scalePose)
        //.def("toVector", &Math::Pose::toVector< Math::Vector< 7 > >)
        //.def("fromVector", &Math::Pose::fromVector< Math::Vector< 7 > >)
    ;

	def("test_vec4", test_vec4);
	def("test_mat33", test_mat33);
	def("test_quat", test_quat);
}

