#include <boost/numpy.hpp>
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

using namespace Ubitrack;

namespace bp = boost::python;
namespace bn = boost::numpy;

namespace {


// by value converters for vector/matrix

template<typename T, int N>
struct vector_to_array {
	static PyObject* convert(Math::Vector< N, T > const& p) {
	    bp::tuple shape = bp::make_tuple(N);
	    bn::ndarray ret = bn::zeros(shape, bn::dtype::get_builtin<T>());
	    for (int i = 0; i < N; ++i) {
			ret[i] = (T)p(i);
		}
		return bp::incref(ret.ptr());
	}

};

struct vector_to_python_converter {
	template<typename T, int N>
	vector_to_python_converter& to_python() {
		bp::to_python_converter< Math::Vector< N, T >, vector_to_array< T, N >, false //vector_to_array has no get_pytype
		>();
		return *this;
	}
};


// Converts a vector < n > to numpy array.
template<typename T, int N, int M>
struct matrix_to_ndarray {
	static PyObject* convert(Math::Matrix< N, M, T > const& p) {
	    bp::tuple shape = bp::make_tuple(N, M);
	    bn::ndarray ret = bn::zeros(shape, bn::dtype::get_builtin<T>());
	    Py_intptr_t const * strides = ret.get_strides();
	    for (int i = 0; i < N; ++i) {
		    for (int j = 0; j < M; ++j) {
		    	*reinterpret_cast< T *>(ret.get_data() + i * strides[0] + j * strides[1]) = p(i, j);
		    }
		}
		return bp::incref(ret.ptr());
	}

};

struct matrix_to_python_converter {
	template<typename T, int N, int M>
	matrix_to_python_converter& to_python() {
		bp::to_python_converter< Math::Matrix< N, M, T >, matrix_to_ndarray< T, N, M >, false //vector_to_array has no get_pytype
		>();
		return *this;
	}
};




// convert ndarray to vector

template< typename T, int N>
static void copy_ndarray_to_vector(bn::ndarray const & array, Math::Vector< N, T >& vec) {
	for (int i = 0; i < N; ++i) {
		vec(i) = bp::extract<T>(array[i]);
	}
}


template <typename T, int N>
struct vector_from_python {

    /**
     *  Register the converter.
     */
    vector_from_python() {
        bp::converter::registry::push_back(
            &convertible,
            &construct,
            bp::type_id< Math::Vector< N, T > >()
        );
    }

    /**
     *  Test to see if we can convert this to the desired type; if not return zero.
     *  If we can convert, returned pointer can be used by construct().
     */
    static void * convertible(PyObject * p) {
        try {
            bp::object obj(bp::handle<>(bp::borrowed(p)));
            std::auto_ptr<bn::ndarray> array(
                new bn::ndarray(
                    bn::from_object(obj, bn::dtype::get_builtin< T >(), 1, 1, bn::ndarray::V_CONTIGUOUS)
                )
            );
            if (array->shape(0) != N) return 0;
            return array.release();
        } catch (bp::error_already_set & err) {
            bp::handle_exception();
            return 0;
        }
    }

    /**
     *  Finish the conversion by initializing the C++ object into memory prepared by Boost.Python.
     */
    static void construct(PyObject * obj, bp::converter::rvalue_from_python_stage1_data * data) {
        // Extract the array we passed out of the convertible() member function.
        std::auto_ptr<bn::ndarray> array(reinterpret_cast<bn::ndarray*>(data->convertible));
        // Find the memory block Boost.Python has prepared for the result.
        typedef bp::converter::rvalue_from_python_storage< Math::Vector< N, T > > storage_t;
        storage_t * storage = reinterpret_cast<storage_t*>(data);
        // Use placement new to initialize the result.
        Math::Vector< N, T >* v = new (storage->storage.bytes) Math::Vector< N, T >();
        // Fill the result with the values from the NumPy array.
        copy_ndarray_to_vector< T , N >(*array, *v);
        // Finish up.
        data->convertible = storage->storage.bytes;
    }

};



// convert ndarray to matrix

template< typename T, int N, int M>
static void copy_ndarray_to_matrix(bn::ndarray const & array, Math::Matrix< N, M, T >& mat) {
    Py_intptr_t const * strides = array.get_strides();
    if ((array.shape(0) == N) && (array.shape(1) == M)) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                mat(i, j) = *reinterpret_cast< T const *>(array.get_data() + i * strides[0] + j * strides[1]);
            }
        }
    } else {
    	std::cout << "Matrix size mismatch - should raise an error here .. " << std::endl;
    }
}


template <typename T, int N, int M >
struct matrix_from_python {

    /**
     *  Register the converter.
     */
    matrix_from_python() {
        bp::converter::registry::push_back(
            &convertible,
            &construct,
            bp::type_id< Math::Matrix< N, M, T > >()
        );
    }

    /**
     *  Test to see if we can convert this to the desired type; if not return zero.
     *  If we can convert, returned pointer can be used by construct().
     */
    static void * convertible(PyObject * p) {
        try {
            bp::object obj(bp::handle<>(bp::borrowed(p)));
            std::auto_ptr<bn::ndarray> array(
                new bn::ndarray(
                    bn::from_object(obj, bn::dtype::get_builtin< T >(), 2, 2, bn::ndarray::V_CONTIGUOUS)
                )
            );
            if (array->shape(0) != N) return 0;
            if (array->shape(1) != M) return 0;
            return array.release();
        } catch (bp::error_already_set & err) {
            bp::handle_exception();
            return 0;
        }
    }

    /**
     *  Finish the conversion by initializing the C++ object into memory prepared by Boost.Python.
     */
    static void construct(PyObject * obj, bp::converter::rvalue_from_python_stage1_data * data) {
        // Extract the array we passed out of the convertible() member function.
        std::auto_ptr<bn::ndarray> array(reinterpret_cast<bn::ndarray*>(data->convertible));
        // Find the memory block Boost.Python has prepared for the result.
        typedef bp::converter::rvalue_from_python_storage< Math::Matrix< N, M, T > > storage_t;
        storage_t * storage = reinterpret_cast<storage_t*>(data);
        // Use placement new to initialize the result.
        Math::Matrix< N, M, T >* v = new (storage->storage.bytes) Math::Matrix< N, M, T >();
        // Fill the result with the values from the NumPy array.
        copy_ndarray_to_matrix< T, N, M >(*array, *v);
        // Finish up.
        data->convertible = storage->storage.bytes;
    }

};



// accessor methods for retrieving vectors and matrices

template< class C, typename T>
static bn::ndarray py_get_translation(bp::object const & self) {
    Math::Vector< 3 , T > const & v = bp::extract<C const &>(self)().translation();
    return bn::from_data(
        v.content(),
        bn::dtype::get_builtin<T>(),
        bp::make_tuple(3),
        bp::make_tuple(sizeof(T)),
        self
    );
}

template< class C, int N, typename T>
static bn::ndarray py_to_vector(bp::object const & self) {
	Math::Vector< N , T > v;
    bp::extract<C const &>(self)().toVector(v);
    bp::tuple shape = bp::make_tuple(N);
    bn::ndarray ret = bn::zeros(shape, bn::dtype::get_builtin<T>());
    for (int i = 0; i < N; ++i) {
    		ret[i] = (T)v(i);
    	}
    return ret;
}



// helpers

template< class T >
T quaternion_from_vector( const Math::Vector< 4 >& vec) {
	return T::fromVector(vec);
}

template< class T >
Math::Vector< 7 > pose_to_vector( const T& q) {
	Math::Vector< 7 > vec;
	q.toVector(vec);
	return vec;
}

template< class T >
T pose_from_vector( const Math::Vector< 4 >& vec) {
	return T::fromVector(vec);
}


template< class T >
Math::Matrix< 3, 3 > quaternion_to_matrix( const T& q) {
	Math::Matrix< 3, 3 > mat;
	q.toMatrix(mat);
	return mat;
}

template< class T >
bp::tuple quaternion_to_axisangle( T& q) {
	Math::Vector< 3 > axis;
	double angle;
	q.toAxisAngle(axis, angle);
	return bp::make_tuple(axis, angle);
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
	// initialize boost.numpy
	bn::initialize();
	/*
	 * Scalar Classes
	 */

    bp::class_< Math::Scalar< int >, boost::shared_ptr< Math::Scalar< int > > >("ScalarInt", bp::init< int >())
    		.add_property("value", &get_value_from_scalar<int>)
            .def(bp::self_ns::str(bp::self_ns::self))
    		;

    bp::class_< Math::Scalar< double >, boost::shared_ptr< Math::Scalar< double > > >("ScalarDouble", bp::init< double >())
    		.add_property("value", &get_value_from_scalar<double>)
            .def(bp::self_ns::str(bp::self_ns::self))
    		;


	/*
	 * Vector Classes
	 */

    vector_from_python< double, 2 >();
    vector_from_python< double, 3 >();
    vector_from_python< double, 4 >();
    vector_from_python< double, 5 >();
    vector_from_python< double, 6 >();
    vector_from_python< double, 7 >();
    vector_from_python< double, 8 >();

    vector_to_python_converter()
    	.to_python< double, 2 >()
    	.to_python< double, 3 >()
    	.to_python< double, 4 >()
    	.to_python< double, 5 >()
    	.to_python< double, 6 >()
    	.to_python< double, 7 >()
    	.to_python< double, 8 >()
    	;


    /*
	 * Matrix Classes
	 */

    matrix_from_python< double, 3, 3 >();
    matrix_from_python< double, 3, 4 >();
    matrix_from_python< double, 4, 4 >();

    matrix_to_python_converter()
    	.to_python< double, 3, 3 >()
    	.to_python< double, 3, 4 >()
    	.to_python< double, 4, 4 >()
    	;

    /*
	 * Quaternion Class
	 */

    bp::class_<boost::math::quaternion< double >, boost::shared_ptr< boost::math::quaternion< double > > >("QuaternionBase")
    	.def(bp::init< std::complex<double>&, std::complex<double>& >())
		.def(bp::init< double, double, double, double >())

        // does not work .. needs casting ??
		.def(bp::self += double())
		.def(bp::self += std::complex<double>())
		.def(bp::self += bp::self)

		.def(bp::self -= double())
		.def(bp::self -= std::complex<double>())
		.def(bp::self -= bp::self)

		.def(bp::self *= double())
		.def(bp::self *= std::complex<double>())
		.def(bp::self *= bp::self)

		.def(bp::self /= double())
		.def(bp::self /= std::complex<double>())
		.def(bp::self /= bp::self)

		.def(bp::self + bp::self)
		.def(bp::self - bp::self)

		.def(bp::self == double())
		.def(bp::self == std::complex<double>())
		.def(bp::self == bp::self)

		.def(bp::self != double())
		.def(bp::self != std::complex<double>())
		.def(bp::self != bp::self)

		.def("real", (double (boost::math::quaternion< double >::*)())&boost::math::quaternion< double >::real)
        .def("unreal",(boost::math::quaternion< double > (boost::math::quaternion< double >::*)())&boost::math::quaternion< double >::unreal)

        .def(bp::self_ns::str(bp::self_ns::self))
        // example for static method ..
        //.def("zeros", (Vector2 (*)())&Vector2::zeros)
    ;

    bp::def("sup", &boost::math::sup<double>);
    bp::def("l1", &boost::math::l1<double>);
    bp::def("abs", &boost::math::abs<double>);
    bp::def("conj", &boost::math::conj<double>);
    bp::def("norm", &boost::math::norm<double>);
    bp::def("spherical", &boost::math::spherical<double>);
    bp::def("semipolar", &boost::math::semipolar<double>);
    bp::def("multipolar", &boost::math::multipolar<double>);
    bp::def("cylindrospherical", &boost::math::cylindrospherical<double>);
    bp::def("cylindrical", &boost::math::cylindrical<double>);
    bp::def("exp", &boost::math::exp<double>);
    bp::def("cos", &boost::math::cos<double>);
    bp::def("sin", &boost::math::sin<double>);
    bp::def("tan", &boost::math::tan<double>);
    bp::def("cosh", &boost::math::cosh<double>);
    bp::def("sinh", &boost::math::sinh<double>);
    bp::def("tanh", &boost::math::tanh<double>);
    bp::def("pow", &boost::math::pow<double>);

	{
    	bp::scope in_Quaternion = bp::class_<Math::Quaternion, boost::shared_ptr< Math::Quaternion >, bp::bases< boost::math::quaternion< double > > >("Quaternion")
    	.def(bp::init< const Math::Vector< 3 >&, const double >())
    	.def(bp::init< const boost::math::quaternion< double > >())
		.def(bp::init< const Math::Matrix< 4, 4 > >())
		.def(bp::init< double, double, double >())
		.def(bp::init< double, double, double, double >())
        .def("x", &Math::Quaternion::x)
        .def("y", &Math::Quaternion::y)
        .def("z", &Math::Quaternion::z)
        .def("w", &Math::Quaternion::w)
        .def("normalize", (Math::Quaternion& (Math::Quaternion::*)())&Math::Quaternion::normalize,
        		bp::return_internal_reference<>())
        .def("invert", (Math::Quaternion& (Math::Quaternion::*)())&Math::Quaternion::invert,
        		bp::return_internal_reference<>())
		.def("inverted", (Math::Quaternion (Math::Quaternion::*)())&Math::Quaternion::operator~)

        // doew not work .. needs casting ??
		.def(bp::self += double())
		.def(bp::self += std::complex<double>())
		.def(bp::self += bp::self)
		.def(bp::self += boost::math::quaternion< double >())

		.def(bp::self -= double())
		.def(bp::self -= std::complex<double>())
		.def(bp::self -= bp::self)
		.def(bp::self -= boost::math::quaternion< double >())

		.def(bp::self *= double())
		.def(bp::self *= std::complex<double>())
		.def(bp::self *= bp::self)
		.def(bp::self *= boost::math::quaternion< double >())

		.def(bp::self /= double())
		.def(bp::self /= std::complex<double>())
		.def(bp::self /= bp::self)
		.def(bp::self /= boost::math::quaternion< double >())

		.def(bp::self + bp::self)
		.def(bp::self - bp::self)
		.def(bp::self * bp::self)
		.def(bp::self / bp::self)

		.def(bp::self + boost::math::quaternion< double >())
		.def(bp::self - boost::math::quaternion< double >())
		.def(bp::self * boost::math::quaternion< double >())
		.def(bp::self / boost::math::quaternion< double >())

		.def(bp::self * Math::Vector< 3 >())

		.def(bp::self == double())
		.def(bp::self == std::complex<double>())
		.def(bp::self == bp::self)
		.def(bp::self == boost::math::quaternion< double >())

		.def(bp::self != double())
		.def(bp::self != std::complex<double>())
		.def(bp::self != bp::self)
		.def(bp::self != boost::math::quaternion< double >())

		.def("angle", &Math::Quaternion::angle)

		.def("negateIfCloser", &Math::Quaternion::negateIfCloser)

		.def("toLogarithm", (Math::Vector< 3 > (Math::Quaternion::*)())&Math::Quaternion::toLogarithm)
		//.def("fromLogaritm", (Math::Quaternion (Math::Quaternion::*)(??))&Math::Quaternion::fromLogarithm
		//		,return_value_policy<copy_const_reference>()
		//		)
		.def("getEulerAngles", (Math::Vector< 3 > (Math::Quaternion::*)(Math::Quaternion::t_EulerSequence) const)&Math::Quaternion::getEulerAngles)


        .def("toMatrix", &quaternion_to_matrix<Math::Quaternion>)
        .def("toAxisAngle", &quaternion_to_axisangle<Math::Quaternion>)
        .def("toVector", &py_to_vector<Math::Quaternion, 4, double>)
        .def("fromVector", &quaternion_from_vector<Math::Quaternion>)
        .staticmethod("fromVector")
		;


    	bp::enum_<Math::Quaternion::t_EulerSequence>("EULER_SEQUENCE")
    		.value("XYZ", Math::Quaternion::EULER_SEQUENCE_XYZ)
    		.value("YZX", Math::Quaternion::EULER_SEQUENCE_YZX)
    		.value("ZXY", Math::Quaternion::EULER_SEQUENCE_ZXY)
    		.value("ZYX", Math::Quaternion::EULER_SEQUENCE_ZYX)
    		.value("XZY", Math::Quaternion::EULER_SEQUENCE_XZY)
    		.value("YXZ", Math::Quaternion::EULER_SEQUENCE_YXZ)
    		;


	}

	bp::def("slerp", &Math::slerp);

    /*
     * Pose Class
     */


	bp::class_<Math::Pose, boost::shared_ptr< Math::Pose > >("Pose", bp::init< const Math::Quaternion&, const Math::Vector< 3 >& >())
		.def(bp::init< const Math::Matrix< 4, 4 >& >())
		.def("rotation", (const Math::Quaternion& (Math::Pose::*)())&Math::Pose::rotation
				,bp::return_internal_reference<>()
				)
		.def("translation", &py_get_translation<Math::Pose, double>
				//(const Math::Vector< 3 >& (Math::Pose::*)())&Math::Pose::translation
				//,return_value_policy<copy_const_reference>()
				//,return_internal_reference<>()
				)

        .def("scalePose", &Math::Pose::scalePose)

        .def("toVector", &py_to_vector<Math::Pose, 7, double>)
        .def("fromVector", &pose_from_vector<Math::Pose>)
        .staticmethod("fromVector")

        .def(bp::self * Math::Vector< 3 >())
        .def(bp::self * bp::self)

        .def("invert",  (Math::Pose (Math::Pose::*)())&Math::Pose::operator~)
        .def(bp::self_ns::str(bp::self_ns::self))
    ;


    /*
     * Testing functions
     */

	bp::def("test_vec4", test_vec4);
	bp::def("test_mat33", test_mat33);
	bp::def("test_quat", test_quat);
}

