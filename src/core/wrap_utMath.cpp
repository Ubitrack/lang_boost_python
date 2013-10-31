#define PYUBLAS_HAVE_BOOST_BINDINGS 1

#include "pyublas/numpy.hpp"

#include <complex>

#include <boost/python.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/to_python_converter.hpp>

#include <utMath/Vector.h>
#include <utMath/Matrix.h>
#include <utMath/Quaternion.h>
#include <utMath/Pose.h>

#include <utMeasurement/Measurement.h>

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
	  object obj(ph.to_python());
	  return incref(obj.ptr());
  }

};

struct vector_to_python_converter
{
	template< typename VT >
	vector_to_python_converter& to_python()
	{
		to_python_converter<
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
	  object obj(ph.to_python());
	  return incref(obj.ptr());
  }

};

struct matrix_to_python_converter
{
	template< typename MT >
	matrix_to_python_converter& to_python()
	{
		to_python_converter<
			MT,
			matrix_to_ndarray<MT>,
			false //vector_to_array has no get_pytype
			>();
		return *this;
	}
};



// helpers

template< class T >
Math::Vector< 4 > quaternion_to_vector( const T& q) {
	Math::Vector< 4 > vec;
	q.toVector(vec);
	return vec;
}

template< class T >
T quaternion_from_vector( const pyublas::numpy_vector<double>& vec) {
	return T::fromVector(vec);
}

template< class T >
Math::Matrix< 3, 3 > quaternion_to_matrix( const T& q) {
	Math::Matrix< 3, 3 > mat;
	q.toMatrix(mat);
	return mat;
}

template< class T >
tuple quaternion_to_axisangle( T& q) {
	Math::Vector< 3 > axis;
	double angle;
	q.toAxisAngle(axis, angle);
	return make_tuple(axis, angle);
}

template< typename T >
T get_value_from_scalar( Math::Scalar<T>& v) {
	return v.m_value;
}

}


// tests
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


BOOST_PYTHON_MODULE(_utmath)
{

	/*
	 * Scalar Classes
	 */

    class_< Math::Scalar< int >, boost::shared_ptr< Math::Scalar< int > > >("ScalarInt", init< npy_int >())
    		.add_property("value", &get_value_from_scalar<int>)
            .def(self_ns::str(self_ns::self))
    		;

    class_< Math::Scalar< double >, boost::shared_ptr< Math::Scalar< double > > >("ScalarDouble", init< npy_double >())
    		.add_property("value", &get_value_from_scalar<double>)
            .def(self_ns::str(self_ns::self))
    		;


	/*
	 * Vector Classes
	 */
	implicitly_convertible< Math::Vector< 2 >, pyublas::numpy_vector< double > >();
	implicitly_convertible< Math::Vector< 3 >, pyublas::numpy_vector< double > >();
	implicitly_convertible< Math::Vector< 4 >, pyublas::numpy_vector< double > >();
	implicitly_convertible< Math::Vector< 5 >, pyublas::numpy_vector< double > >();
	implicitly_convertible< Math::Vector< 6 >, pyublas::numpy_vector< double > >();
	implicitly_convertible< Math::Vector< 7 >, pyublas::numpy_vector< double > >();
	implicitly_convertible< Math::Vector< 8 >, pyublas::numpy_vector< double > >();

	implicitly_convertible<pyublas::numpy_vector< double >,  Math::Vector< 2 > >();
	implicitly_convertible<pyublas::numpy_vector< double >,  Math::Vector< 3 > >();
	implicitly_convertible<pyublas::numpy_vector< double >,  Math::Vector< 4 > >();
	implicitly_convertible<pyublas::numpy_vector< double >,  Math::Vector< 5 > >();
	implicitly_convertible<pyublas::numpy_vector< double >,  Math::Vector< 6 > >();
	implicitly_convertible<pyublas::numpy_vector< double >,  Math::Vector< 7 > >();
	implicitly_convertible<pyublas::numpy_vector< double >,  Math::Vector< 8 > >();

	vector_to_python_converter()
			.to_python< Math::Vector< 2 > >()
			.to_python< Math::Vector< 3 > >()
			.to_python< Math::Vector< 4 > >()
			.to_python< Math::Vector< 5 > >()
			.to_python< Math::Vector< 6 > >()
			.to_python< Math::Vector< 7 > >()
			.to_python< Math::Vector< 8 > >()
			;

	/*
	 * Matrix Classes
	 */

	implicitly_convertible< Math::Matrix < 2, 2 >, pyublas::numpy_matrix< double > >();
	implicitly_convertible< Math::Matrix < 3, 3 >, pyublas::numpy_matrix< double > >();
	implicitly_convertible< Math::Matrix < 4, 4 >, pyublas::numpy_matrix< double > >();
	implicitly_convertible< Math::Matrix < 3, 4 >, pyublas::numpy_matrix< double > >();

	implicitly_convertible<pyublas::numpy_matrix< double >,  Math::Matrix < 2, 2 > >();
	implicitly_convertible<pyublas::numpy_matrix< double >,  Math::Matrix < 3, 3 > >();
	implicitly_convertible<pyublas::numpy_matrix< double >,  Math::Matrix < 4, 4 > >();
	implicitly_convertible<pyublas::numpy_matrix< double >,  Math::Matrix < 3, 4 > >();

	matrix_to_python_converter()
			.to_python< Math::Matrix < 2, 2 > >()
			.to_python< Math::Matrix < 3, 3 > >()
			.to_python< Math::Matrix < 4, 4 > >()
			.to_python< Math::Matrix < 3, 4 > >();



	/*
	 * Quaternion Class
	 */

	class_<boost::math::quaternion< double >, boost::shared_ptr< boost::math::quaternion< double > > >("QuaternionBase")
    	.def(init< std::complex<double>&, std::complex<double>& >())
		.def(init< double, double, double, double >())

        // does not work .. needs casting ??
		.def(self += double())
		.def(self += std::complex<double>())
		.def(self += self)

		.def(self -= double())
		.def(self -= std::complex<double>())
		.def(self -= self)

		.def(self *= double())
		.def(self *= std::complex<double>())
		.def(self *= self)

		.def(self /= double())
		.def(self /= std::complex<double>())
		.def(self /= self)

		.def(self + self)
		.def(self - self)

		.def(self == double())
		.def(self == std::complex<double>())
		.def(self == self)

		.def(self != double())
		.def(self != std::complex<double>())
		.def(self != self)

		.def("real", (double (boost::math::quaternion< double >::*)())&boost::math::quaternion< double >::real)
        .def("unreal",(boost::math::quaternion< double > (boost::math::quaternion< double >::*)())&boost::math::quaternion< double >::unreal)

        .def(self_ns::str(self_ns::self))
        // example for static method ..
        //.def("zeros", (Vector2 (*)())&Vector2::zeros)
    ;

    def("sup", &boost::math::sup<double>);
    def("l1", &boost::math::l1<double>);
    def("abs", &boost::math::abs<double>);
    def("conj", &boost::math::conj<double>);
    def("norm", &boost::math::norm<double>);
    def("spherical", &boost::math::spherical<double>);
    def("semipolar", &boost::math::semipolar<double>);
    def("multipolar", &boost::math::multipolar<double>);
    def("cylindrospherical", &boost::math::cylindrospherical<double>);
    def("cylindrical", &boost::math::cylindrical<double>);
    def("exp", &boost::math::exp<double>);
    def("cos", &boost::math::cos<double>);
    def("sin", &boost::math::sin<double>);
    def("tan", &boost::math::tan<double>);
    def("cosh", &boost::math::cosh<double>);
    def("sinh", &boost::math::sinh<double>);
    def("tanh", &boost::math::tanh<double>);
    def("pow", &boost::math::pow<double>);

	{
	scope in_Quaternion = class_<Math::Quaternion, boost::shared_ptr< Math::Quaternion >, bases< boost::math::quaternion< double > > >("Quaternion")
    	.def(init< const Math::Vector< 3 >&, const double >())
		.def(init< const boost::math::quaternion< double > >())
		.def(init< const boost::numeric::ublas::matrix< double > >())
		.def(init< double, double, double >())
		.def(init< double, double, double, double >())
        .def("x", &Math::Quaternion::x)
        .def("y", &Math::Quaternion::y)
        .def("z", &Math::Quaternion::z)
        .def("w", &Math::Quaternion::w)
        .def("normalize", (Math::Quaternion& (Math::Quaternion::*)())&Math::Quaternion::normalize,
        		return_internal_reference<>())
        .def("invert", (Math::Quaternion& (Math::Quaternion::*)())&Math::Quaternion::invert,
        		return_internal_reference<>())
		.def("inverted", (Math::Quaternion (Math::Quaternion::*)())&Math::Quaternion::operator~)

        // doew not work .. needs casting ??
		.def(self += double())
		.def(self += std::complex<double>())
		.def(self += self)
		.def(self += boost::math::quaternion< double >())

		.def(self -= double())
		.def(self -= std::complex<double>())
		.def(self -= self)
		.def(self -= boost::math::quaternion< double >())

		.def(self *= double())
		.def(self *= std::complex<double>())
		.def(self *= self)
		.def(self *= boost::math::quaternion< double >())

		.def(self /= double())
		.def(self /= std::complex<double>())
		.def(self /= self)
		.def(self /= boost::math::quaternion< double >())

		.def(self + self)
		.def(self - self)
		.def(self * self)
		.def(self / self)

		.def(self + boost::math::quaternion< double >())
		.def(self - boost::math::quaternion< double >())
		.def(self * boost::math::quaternion< double >())
		.def(self / boost::math::quaternion< double >())

		.def(self * Math::Vector< 3 >())

		.def(self == double())
		.def(self == std::complex<double>())
		.def(self == self)
		.def(self == boost::math::quaternion< double >())

		.def(self != double())
		.def(self != std::complex<double>())
		.def(self != self)
		.def(self != boost::math::quaternion< double >())

		.def("angle", &Math::Quaternion::angle)

		.def("negateIfCloser", &Math::Quaternion::negateIfCloser)

		.def("toLogarithm", (Math::Vector< 3 > (Math::Quaternion::*)())&Math::Quaternion::toLogarithm)
		//.def("fromLogaritm", (Math::Quaternion (Math::Quaternion::*)(??))&Math::Quaternion::fromLogarithm
		//		,return_value_policy<copy_const_reference>()
		//		)
		.def("getEulerAngles", (Math::Vector< 3 > (Math::Quaternion::*)(Math::Quaternion::t_EulerSequence) const)&Math::Quaternion::getEulerAngles)


        .def("toMatrix", &quaternion_to_matrix<Math::Quaternion>)
        .def("toAxisAngle", &quaternion_to_axisangle<Math::Quaternion>)
        .def("toVector", &quaternion_to_vector<Math::Quaternion>)
        .def("fromVector", &quaternion_from_vector<Math::Quaternion>)
        .staticmethod("fromVector")
		;


    enum_<Math::Quaternion::t_EulerSequence>("EULER_SEQUENCE")
    		.value("XYZ", Math::Quaternion::EULER_SEQUENCE_XYZ)
    		.value("YZX", Math::Quaternion::EULER_SEQUENCE_YZX)
    		.value("ZXY", Math::Quaternion::EULER_SEQUENCE_ZXY)
    		.value("ZYX", Math::Quaternion::EULER_SEQUENCE_ZYX)
    		.value("XZY", Math::Quaternion::EULER_SEQUENCE_XZY)
    		.value("YXZ", Math::Quaternion::EULER_SEQUENCE_YXZ)
    		;


	}

    def("slerp", &Math::slerp);

    /*
     * Pose Class
     */


    class_<Math::Pose, boost::shared_ptr< Math::Pose > >("Pose", init< const Math::Quaternion&, const pyublas::numpy_vector<double>& >())
		.def(init< boost::numeric::ublas::matrix< double > >())
		.def("rotation", (const Math::Quaternion& (Math::Pose::*)())&Math::Pose::rotation,
				return_internal_reference<>())
		.def("translation", (const Math::Vector< 3 >& (Math::Pose::*)())&Math::Pose::translation
				,return_value_policy<copy_const_reference>()
				//,return_internal_reference<>()
				)
        .def("scalePose", &Math::Pose::scalePose)
        //.def("toVector", (void (Math::Pose::*)(const Math::Vector< 7 >&))&Math::Pose::toVector< Math::Vector< 7 > >)
        //.def("fromVector", (Math::Pose (Math::Pose::*)(pyublas::numpy_vector<double>&))&Math::Pose::fromVector< Math::Vector< 7 > >)
        //.staticmethod("fromVector")
        .def(self * Math::Vector< 3 >())
        .def(self * self)

        .def("invert",  (Math::Pose (Math::Pose::*)())&Math::Pose::operator~)
        .def(self_ns::str(self_ns::self))
    ;


    /*
     * Testing functions
     */

	def("test_vec4", test_vec4);
	def("test_mat33", test_mat33);
	def("test_quat", test_quat);
}

