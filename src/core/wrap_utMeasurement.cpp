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

#include <iostream>

using namespace boost::python;
using namespace Ubitrack;

namespace {

template< class T>
typename T::value_type* get_measurement(const T& m) {
	return m.get();
}

template< class T >
struct measurement_converter
{
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
				.def("get", &get_measurement< T>
						,return_value_policy<reference_existing_object>()
						)
            ;
    }
};


}


// tests
Measurement::Position2D test_pos2d() {
	boost::shared_ptr< Math::Vector<2> > pos(new Math::Vector<2>(1.0, 2.0));
	Measurement::Position2D m(123, pos);
	std::cout << "generated measurement: " << m;
	return m;
}

Measurement::Pose test_posemeasurement() {
	boost::shared_ptr< Math::Pose > pose(new Math::Pose(Math::Quaternion(0.0, 0.0, 0.0, 1.0), Math::Vector<3>(1.0, 2.0, 3.0)));

	Measurement::Pose m(123, pose);
	std::cout << "generated measurement: " << m;
	return m;
}



BOOST_PYTHON_MODULE(_utmeasurement)
{
	def("now", Measurement::now);

	measurement_converter<Measurement::Distance >::expose(
			class_<Measurement::Distance>("Distance")
				.def(init<Measurement::Timestamp, npy_double >())
			);

	measurement_converter<Measurement::Position2D >::expose(
			class_<Measurement::Position2D>("Position2D")
				.def(init<Measurement::Timestamp, const pyublas::numpy_vector<double>& >())
			);


	measurement_converter<Measurement::Rotation >::expose(
			class_<Measurement::Rotation>("Rotation")
				.def(init<Measurement::Timestamp, const Math::Quaternion& >())
			);

	measurement_converter<Measurement::Pose >::expose(
			class_<Measurement::Pose>("Pose")
				.def(init<Measurement::Timestamp, const Math::Pose& >())
			);

	def("test_pos2d", test_pos2d);
	def("test_posemeasurement", test_posemeasurement);

}

