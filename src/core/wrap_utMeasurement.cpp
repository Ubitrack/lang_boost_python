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
const typename T::value_type& get_measurement(const T& m) {
	return *(m.get());
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
						//,return_value_policy<reference_existing_object>()
        				,return_value_policy<copy_const_reference>()
						//,return_internal_reference<>()
						)
				.def(self_ns::str(self_ns::self))
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
//				.def(init<Measurement::Timestamp, npy_double >())
			);

	measurement_converter<Measurement::Button >::expose(
			class_<Measurement::Button>("Button")
//				.def(init<Measurement::Timestamp, npy_int >())
			);

	measurement_converter<Measurement::Position2D >::expose(
			class_<Measurement::Position2D>("Position2D")
//				.def(init<Measurement::Timestamp, const pyublas::numpy_vector<double>& >())
			);


	measurement_converter<Measurement::Position >::expose(
			class_<Measurement::Position>("Position")
//				.def(init<Measurement::Timestamp, const pyublas::numpy_vector<double>& >())
			);

	measurement_converter<Measurement::Vector4D >::expose(
			class_<Measurement::Vector4D>("Vector4D")
//				.def(init<Measurement::Timestamp, const pyublas::numpy_vector<double>& >())
			);

	measurement_converter<Measurement::Vector8D >::expose(
			class_<Measurement::Vector8D>("Vector8D")
//				.def(init<Measurement::Timestamp, const pyublas::numpy_vector<double>& >())
			);

	measurement_converter<Measurement::Rotation >::expose(
			class_<Measurement::Rotation>("Rotation")
//				.def(init<Measurement::Timestamp, const Math::Quaternion& >())
			);

	measurement_converter<Measurement::Matrix3x3 >::expose(
			class_<Measurement::Matrix3x3>("Matrix3x3")
//				.def(init<Measurement::Timestamp, const pyublas::numpy_matrix<double>& >())
			);

	measurement_converter<Measurement::Matrix3x4 >::expose(
			class_<Measurement::Matrix3x4>("Matrix3x4")
//				.def(init<Measurement::Timestamp, const pyublas::numpy_matrix<double>& >())
			);

	measurement_converter<Measurement::Matrix4x4 >::expose(
			class_<Measurement::Matrix4x4>("Matrix4x4")
//				.def(init<Measurement::Timestamp, const pyublas::numpy_matrix<double>& >())
			);

	measurement_converter<Measurement::Pose >::expose(
			class_<Measurement::Pose>("Pose")
//				.def(init<Measurement::Timestamp, const Math::Pose& >())
			);

/*
	measurement_converter<Measurement::ErrorPose >::expose(
			class_<Measurement::ErrorPose>("ErrorPose")
//				.def(init<Measurement::Timestamp, const Math::ErrorPose& >())
			);

	measurement_converter<Measurement::ErrorPosition >::expose(
			class_<Measurement::ErrorPosition>("ErrorPosition")
//				.def(init<Measurement::Timestamp, const Math::ErrorVector< 3 > & >())
			);

	measurement_converter<Measurement::RotationVelocity >::expose(
			class_<Measurement::RotationVelocity>("RotationVelocity")
//				.def(init<Measurement::Timestamp, const Math::RotationVelocity & >())
			);

*/

// XXX Multiple Measurement missing

	def("test_pos2d", test_pos2d);
	def("test_posemeasurement", test_posemeasurement);

}

