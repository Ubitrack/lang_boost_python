#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <boost/numpy.hpp>
#include <numpy/arrayobject.h>
#include <complex>

#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/to_python_converter.hpp>

#include <utMeasurement/Measurement.h>

#include <iostream>

using namespace Ubitrack;

namespace bp = boost::python;
namespace bn = boost::numpy;

namespace {

template< class T>
const typename T::value_type& get_measurement(const T& m) {
	// potentially dangerous ..
	return *(m.get());
}



template< class T >
struct measurement_converter
{
    template <class C>
    static void expose(C const& c)
    {
        const_cast<C&>(c)
				.def(bp::init<Measurement::Timestamp>())
				.def(bp::init<Measurement::Timestamp, const typename T:: value_type& >())
				.def("time", (Measurement::Timestamp (T::*)() const)&T::time)
				.def("set_time", (void (T::*)(Measurement::Timestamp))&T::time)
				.def("invalid", (bool (T::*)())&T::invalid)
				.def("invalidate", (void (T::*)())&T::invalidate)
				.def("get", &get_measurement< T >
						//,return_value_policy<reference_existing_object>()
        				,bp::return_value_policy<bp::copy_const_reference>()
						//,return_internal_reference<>()
						)
				.def(bp::self_ns::str(bp::self_ns::self))
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
	bp::def("now", Measurement::now);

	measurement_converter<Measurement::Distance >::expose(
			bp::class_<Measurement::Distance>("Distance")
//				.def(bp::init<Measurement::Timestamp, npy_double >())
			);

	measurement_converter<Measurement::Button >::expose(
			bp::class_<Measurement::Button>("Button")
//				.def(bp::init<Measurement::Timestamp, npy_int >())
			);

	measurement_converter<Measurement::Position2D >::expose(
			bp::class_<Measurement::Position2D>("Position2D")
//				.def(bp::init<Measurement::Timestamp, const pyublas::numpy_vector<double>& >())
			);


	measurement_converter<Measurement::Position >::expose(
			bp::class_<Measurement::Position>("Position")
//				.def(bp::init<Measurement::Timestamp, const pyublas::numpy_vector<double>& >())
			);

	measurement_converter<Measurement::Vector4D >::expose(
			bp::class_<Measurement::Vector4D>("Vector4D")
//				.def(bp::init<Measurement::Timestamp, const pyublas::numpy_vector<double>& >())
			);

	measurement_converter<Measurement::Vector8D >::expose(
			bp::class_<Measurement::Vector8D>("Vector8D")
//				.def(bp::init<Measurement::Timestamp, const pyublas::numpy_vector<double>& >())
			);

	measurement_converter<Measurement::Rotation >::expose(
			bp::class_<Measurement::Rotation>("Rotation")
//				.def(bp::init<Measurement::Timestamp, const Math::Quaternion& >())
			);

	measurement_converter<Measurement::Matrix3x3 >::expose(
			bp::class_<Measurement::Matrix3x3>("Matrix3x3")
//				.def(bp::init<Measurement::Timestamp, const pyublas::numpy_matrix<double>& >())
			);

	measurement_converter<Measurement::Matrix3x4 >::expose(
			bp::class_<Measurement::Matrix3x4>("Matrix3x4")
//				.def(bp::init<Measurement::Timestamp, const pyublas::numpy_matrix<double>& >())
			);

	measurement_converter<Measurement::Matrix4x4 >::expose(
			bp::class_<Measurement::Matrix4x4>("Matrix4x4")
//				.def(bp::init<Measurement::Timestamp, const pyublas::numpy_matrix<double>& >())
			);

	measurement_converter<Measurement::Pose >::expose(
			bp::class_<Measurement::Pose>("Pose")
//				.def(bp::init<Measurement::Timestamp, const Math::Pose& >())
			);

/*
	measurement_converter<Measurement::ErrorPose >::expose(
			bp::class_<Measurement::ErrorPose>("ErrorPose")
//				.def(bp::init<Measurement::Timestamp, const Math::ErrorPose& >())
			);

	measurement_converter<Measurement::ErrorPosition >::expose(
			bp::class_<Measurement::ErrorPosition>("ErrorPosition")
//				.def(bp::init<Measurement::Timestamp, const Math::ErrorVector< 3 > & >())
			);

	measurement_converter<Measurement::RotationVelocity >::expose(
			bp::class_<Measurement::RotationVelocity>("RotationVelocity")
//				.def(bp::init<Measurement::Timestamp, const Math::RotationVelocity & >())
			);

*/

// XXX Multiple Measurement missing

	bp::def("test_pos2d", &test_pos2d);
	bp::def("test_posemeasurement", &test_posemeasurement);

}

