#define PYUBLAS_HAVE_BOOST_BINDINGS 1

#include "pyublas/numpy.hpp"

#include <complex>

#include <boost/python.hpp>
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

//  static PyObject* convert(Measurement::Measurement<T1> const& p)
//  {
//	  pyublas::numpy_vector<double> ph(*p);
//	  object obj(ph.to_python());
//	  return incref(make_tuple(long_(p.time()), obj).ptr());
//  }
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

//		to_python_converter<
//			Measurement::Measurement<VT>,
//			vector_to_array<VT>,
//			false //vector_to_array has no get_pytype
//			>();
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

//  static PyObject* convert(Measurement::Measurement<T1> const& p)
//  {
//	  pyublas::numpy_matrix<double> ph(*p);
//	  object obj(ph.to_python());
//	  return incref(make_tuple(long_(p.time()), obj).ptr());
//  }
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

//		to_python_converter<
//			Measurement::Measurement<MT>,
//			matrix_to_ndarray<MT>,
//			false //vector_to_array has no get_pytype
//			>();
		return *this;
	}
};

// helpers

template< class T >
pyublas::numpy_vector<double> quaternion_to_vector( const T& q) {
	Math::Vector< 4 > vec;
	q.toVector(vec);
	return vec;
}

template< class T >
T quaternion_from_vector( const pyublas::numpy_vector<double>& vec) {
	return T::fromVector(vec);
}

template< class T >
pyublas::numpy_matrix<double> quaternion_to_matrix( const T& q) {
	boost::numeric::ublas::matrix< double > mat;
	q.toMatrix(mat);
	return mat;
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


// tests
//Measurement::Position2D test_pos2d() {
//	boost::shared_ptr< Math::Vector<2> > pos(new Math::Vector<2>(1.0, 2.0));
//	Measurement::Position2D m(123, pos);
//	std::cout << "generated measurement: " << m;
//	return m;
//}

// tests
//Measurement::Pose test_posemeasurement() {
//	boost::shared_ptr< Math::Pose > pose(new Math::Pose(Math::Quaternion(0.0, 0.0, 0.0, 1.0), Math::Vector<3>(1.0, 2.0, 3.0)));
//
//	Measurement::Pose m(123, pose);
//	std::cout << "generated measurement: " << m;
//	return m;
//}


BOOST_PYTHON_MODULE(_utmath)
{
	vector_to_python_converter()
			.to_python< Math::Vector< 2 > >()
			.to_python< Math::Vector< 3 > >()
			.to_python< Math::Vector< 4 > >()
			.to_python< Math::Vector< 5 > >()
			.to_python< Math::Vector< 6 > >()
			.to_python< Math::Vector< 7 > >()
			.to_python< Math::Vector< 8 > >()
			;

	matrix_to_python_converter()
			.to_python< Math::Matrix < 2, 2 > >()
			.to_python< Math::Matrix < 3, 3 > >()
			.to_python< Math::Matrix < 4, 4 > >()
			.to_python< Math::Matrix < 3, 4 > >();


	class_<boost::math::quaternion< double >, std::auto_ptr< boost::math::quaternion< double > > >("QuaternionBase")
    	.def(init< std::complex<double>&, std::complex<double>& >())
		.def(init< double, double, double, double >())

        // doew not work .. needs casting ??
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
	scope in_Quaternion = class_<Math::Quaternion, std::auto_ptr< Math::Quaternion >, bases< boost::math::quaternion< double > > >("Quaternion")
    	.def(init< const pyublas::numpy_vector<double>&, double >())
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

		.def(self * pyublas::numpy_vector<double>())

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
        //.def("toAxisAngle", &quaternion_to_vector<Math::Quaternion>)
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


    class_<Math::Pose, std::auto_ptr< Math::Pose > >("Pose", init< const Math::Quaternion&, const pyublas::numpy_vector<double>& >())
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
        .def(self * pyublas::numpy_vector<double>())
        .def(self * self)

        .def("invert",  (Math::Pose (Math::Pose::*)())&Math::Pose::operator~)
        .def(self_ns::str(self_ns::self))
    ;

    // register ptrs
//    register_ptr_to_python< Measurement::Measurement< Math::Pose > >();
//    register_ptr_to_python< Measurement::Measurement< Math::Quaternion > >();


	def("test_vec4", test_vec4);
	def("test_mat33", test_mat33);
	def("test_quat", test_quat);
//	def("test_pos2d", test_pos2d);
//	def("test_posemeasurement", test_posemeasurement);
}

