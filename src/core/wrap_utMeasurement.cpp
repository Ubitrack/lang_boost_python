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
        				,bp::return_value_policy<bp::copy_const_reference>()
						//,return_value_policy<reference_existing_object>()
						//,return_internal_reference<>()
						)
				.def(bp::self_ns::str(bp::self_ns::self))
            ;
    }
};


// workaround, since by-value conversion is not properly working for Vector/Matrix classes
// since they are handles specially (implicit conversion) .. which is not yet working 100%
template< class T>
bp::list get_measurementlist(const T& m) {
	bp::list ret;
	for (typename T::value_type::iterator it = m->begin() ; it != m->end(); ++it) {
		ret.append((*it));
	}
	return ret;
}


template< class T >
struct measurementlist_converter
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
				.def("get", &get_measurementlist< T >)
            ;
    }
};


}


// tests
Measurement::Position2D test_pos2d() {
	boost::shared_ptr< Math::Vector<2> > pos(new Math::Vector<2>(1.0, 2.0));
	Measurement::Position2D m(123, pos);
	return m;
}

Measurement::Pose test_posemeasurement() {
	boost::shared_ptr< Math::Pose > pose(new Math::Pose(Math::Quaternion(0.0, 0.0, 0.0, 1.0), Math::Vector<3>(1.0, 2.0, 3.0)));
	Measurement::Pose m(123, pose);
	return m;
}

Measurement::PoseList test_poselistmeasurement() {
	boost::shared_ptr< std::vector<Math::Pose> > vec( new std::vector<Math::Pose>());
	for ( int i = 0; i < 3; i++ ) {
		Math::Pose pose(Math::Quaternion(0.0, 0.0, 0.0, 1.0), Math::Vector<3>(1.0, 2.0, 3.0));
		vec->push_back(pose);
	}
	Measurement::PoseList m(123, vec);
	return m;
}



BOOST_PYTHON_MODULE(_utmeasurement)
{
	bp::def("now", Measurement::now);

	measurement_converter<Measurement::Distance >::expose(
			bp::class_<Measurement::Distance>("Distance")
			);

	measurement_converter<Measurement::Button >::expose(
			bp::class_<Measurement::Button>("Button")
			);

	measurement_converter<Measurement::Position2D >::expose(
			bp::class_<Measurement::Position2D>("Position2D")
			);


	measurement_converter<Measurement::Position >::expose(
			bp::class_<Measurement::Position>("Position")
			);

	measurement_converter<Measurement::Vector4D >::expose(
			bp::class_<Measurement::Vector4D>("Vector4D")
			);

	measurement_converter<Measurement::Vector8D >::expose(
			bp::class_<Measurement::Vector8D>("Vector8D")
			);

	measurement_converter<Measurement::Rotation >::expose(
			bp::class_<Measurement::Rotation>("Rotation")
			);

	measurement_converter<Measurement::Matrix3x3 >::expose(
			bp::class_<Measurement::Matrix3x3>("Matrix3x3")
			);

	measurement_converter<Measurement::Matrix3x4 >::expose(
			bp::class_<Measurement::Matrix3x4>("Matrix3x4")
			);

	measurement_converter<Measurement::Matrix4x4 >::expose(
			bp::class_<Measurement::Matrix4x4>("Matrix4x4")
			);

	measurement_converter<Measurement::Pose >::expose(
			bp::class_<Measurement::Pose>("Pose")
			);

/*
	measurement_converter<Measurement::ErrorPose >::expose(
			bp::class_<Measurement::ErrorPose>("ErrorPose")
			);

	measurement_converter<Measurement::ErrorPosition >::expose(
			bp::class_<Measurement::ErrorPosition>("ErrorPosition")
			);

	measurement_converter<Measurement::RotationVelocity >::expose(
			bp::class_<Measurement::RotationVelocity>("RotationVelocity")
			);

*/


	measurementlist_converter<Measurement::PoseList >::expose(
			bp::class_<Measurement::PoseList>("PoseList")
			);

	measurementlist_converter<Measurement::PositionList2 >::expose(
			bp::class_<Measurement::PositionList2>("PositionList2")
			);

	measurementlist_converter<Measurement::PositionList >::expose(
			bp::class_<Measurement::PositionList>("PositionList")
			);

	measurementlist_converter<Measurement::DistanceList >::expose(
			bp::class_<Measurement::DistanceList>("DistanceList")
			);

	measurementlist_converter<Measurement::IDList >::expose(
			bp::class_<Measurement::IDList>("IDList")
			);

/*
	measurementlist_converter<Measurement::ErrorPoseList >::expose(
			bp::class_<Measurement::ErrorPoseList>("ErrorPoseList")
			);

	measurementlist_converter<Measurement::ErrorPositionList2 >::expose(
			bp::class_<Measurement::ErrorPositionList2>("ErrorPositionList2")
			);

	measurementlist_converter<Measurement::ErrorPositionList >::expose(
			bp::class_<Measurement::ErrorPositionList>("ErrorPositionList")
			);
*/


	bp::def("test_pos2d", &test_pos2d);
	bp::def("test_posemeasurement", &test_posemeasurement);
	bp::def("test_poselistmeasurement", &test_poselistmeasurement
//			,bp::return_value_policy<bp::manage_new_object>()
			);

}

