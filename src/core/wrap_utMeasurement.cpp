#define PYUBLAS_HAVE_BOOST_BINDINGS 1

#include "pyublas/numpy.hpp"

#include <complex>

#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/to_python_converter.hpp>

#include <utMeasurement/Measurement.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

using namespace boost::python;
using namespace Ubitrack;

namespace {

template< class T>
struct measurement_converter
{

	boost::shared_ptr<typename T:: value_type> get_measurement(const T& m) {
		typename T:: value_type* ptr = m.get();
		//boost::shared_ptr<typename T:: value_type> ptr(m.get());
		return ptr;
	}

    template <class C>
    static void expose(C const& c)
    {
        const_cast<C&>(c)
				.def(init<Measurement::Timestamp>())
				.def(init<Measurement::Timestamp, const typename T:: value_type& >())
				.def("time", (Measurement::Timestamp (T::*)() const)&T::time)
				.def("set_time", (void (T::*)(Measurement::Timestamp))&T::time)
				.def("invalid", (bool (T::*)())&T::invalid)
				.def("invalidate", (void (T::*)())&T::invalidate)
				.def("get", &get_measurement)
            ;
    }
};


}


/*
// here comes the magic
template <typename T> T* get_pointer(Measurement::Measurement<T> const& p) {
  //notice the const_cast<> at this point
  //for some unknown reason, bp likes to have it like that
  return const_cast<T*>(p.get());
}

// some boost.python plumbing is required as you already know
namespace boost { namespace python {

  template <typename T> struct pointee<Measurement::Measurement<T> > {
    typedef T type;
  };

} }
*/

// tests
Math::Vector< 4 > test_vec4() {
	return Math::Vector< 4 >(1,2,2,1.2);
}



BOOST_PYTHON_MODULE(_utmeasurement)
{
	def("now", Measurement::now);

	measurement_converter<Measurement::Distance>::expose(
			class_<Measurement::Distance, boost::noncopyable>("Distance")
				.def(init<Measurement::Timestamp, npy_double >())
//				.def("get", (typename Measurement::Distance:: value_type* (Measurement::Distance::*)() const)&Measurement::Distance::get
//						, return_internal_reference<>()
//						)
			);

	measurement_converter<Measurement::Position2D>::expose(
			class_<Measurement::Position2D, boost::noncopyable>("Position2D")
				.def(init<Measurement::Timestamp, const pyublas::numpy_vector<double>& >())
//				.def("get", (Math::Vector< 2 >* (Measurement::Position2D::*)() const)&Measurement::Position2D::get
//						//, return_internal_reference<>()
//						)
			);


	register_ptr_to_python< Measurement::Position2D >();

	def("test_vec4", test_vec4);

}

