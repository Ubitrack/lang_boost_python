#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <boost/numpy.hpp>
#include <numpy/arrayobject.h>
#include <complex>

#include <boost/bind.hpp>

#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/to_python_converter.hpp>

#include <utFacade/AdvancedFacade.h>
#include <utComponents/ApplicationPushSink.h>
#include <utComponents/ApplicationPullSink.h>
#include <utComponents/ApplicationPushSource.h>


#include <iostream>

using namespace Ubitrack;

namespace bp = boost::python;
namespace bn = boost::numpy;

namespace {

template< class MT >
struct measurement_callback_wrapper_t
{
	measurement_callback_wrapper_t( bp::object callable ) : _callable( callable ) {}

    bool operator()(const MT& m)
    {
        PyGILState_STATE gstate = PyGILState_Ensure();
        bool ret = _callable(m);
        PyGILState_Release( gstate );
        return ret;
    }

    bp::object _callable;
};

template< class OT, class MT >
void setWrappedCallback( OT* self,  bp::object function )
{
	self->setCallback(boost::function<void (const MT&)>(
			measurement_callback_wrapper_t< MT >( function ) ));
}

template< class MT >
void setWrappedCallbackFacade( Facade::AdvancedFacade* self, const std::string& name, bp::object function )
{
	self->setCallback(name, boost::function<void (const MT&)>(
			measurement_callback_wrapper_t< MT >( function ) ));
}


template< class ComponentType, class EventType >
void expose_pushsink_for(const std::string& event_name)
{
	std::string pushsink_name("ApplicationPushSink");
	pushsink_name.append(event_name);
	bp::class_< ComponentType, boost::shared_ptr< ComponentType >, bp::bases< Dataflow::Component >, boost::noncopyable>(pushsink_name.c_str(), bp::no_init)
		.def("setCallback", &setWrappedCallback< ComponentType, EventType >)
		;
}

template< class EventType >
void expose_pullsink_for(const std::string& event_name)
{
	std::string pullsink_name("ApplicationPullSink");
	pullsink_name.append(event_name);
	bp::class_< Components::ApplicationPullSink< EventType >, boost::shared_ptr< Components::ApplicationPullSink< EventType > >, bp::bases< Dataflow::Component >, boost::noncopyable>(pullsink_name.c_str(), bp::no_init)
		.def("get", &Components::ApplicationPullSink< EventType >::get)
		;
}


}

BOOST_PYTHON_MODULE(_utfacade)
{
	expose_pushsink_for< Components::ApplicationPushSinkButton, Measurement::Button >("Button");
	expose_pushsink_for< Components::ApplicationPushSinkPose, Measurement::Pose >("Pose");
//	expose_pushsink_for< Components::ApplicationPushSinkErrorPose, Measurement::ErrorPose >("ErrorPose");
	expose_pushsink_for< Components::ApplicationPushSinkPosition, Measurement::Position >("Position");
	expose_pushsink_for< Components::ApplicationPushSinkPosition2D, Measurement::Position2D >("Position2D");
	expose_pushsink_for< Components::ApplicationPushSinkRotation, Measurement::Rotation >("Rotation");
	expose_pushsink_for< Components::ApplicationPushSinkMatrix4x4, Measurement::Matrix4x4 >("Matrix4x4");
	expose_pushsink_for< Components::ApplicationPushSinkMatrix3x3, Measurement::Matrix3x3 >("Matrix3x3");

	expose_pushsink_for< Components::ApplicationPushSinkPositionList, Measurement::PositionList >("PositionList");
	expose_pushsink_for< Components::ApplicationPushSinkPositionList2, Measurement::PositionList2 >("PositionList2");
//	expose_pushsink_for< Components::ApplicationPushSinkErrorPositionList, Measurement::ErrorPositionList >("ErrorPositionList");
//	expose_pushsink_for< Components::ApplicationPushSinkErrorPositionList2, Measurement::ErrorPositionList2 >("ErrorPositionList2");

	expose_pullsink_for< Measurement::Button >("Button");
	expose_pullsink_for< Measurement::Pose >("Pose");
//	expose_pullsink_for< Measurement::ErrorPose >("ErrorPose");
	expose_pullsink_for< Measurement::Position2D >("Position2D");
	expose_pullsink_for< Measurement::Position >("Position");
//	expose_pullsink_for< Measurement::ErrorPosition >("ErrorPosition");
	expose_pullsink_for< Measurement::Rotation >("Rotation");
	expose_pullsink_for< Measurement::Matrix4x4 >("Matrix4x4");
	expose_pullsink_for< Measurement::Matrix3x3 >("Matrix3x3");
	expose_pullsink_for< Measurement::Vector4D >("Vector4D");

	expose_pullsink_for< Measurement::PositionList >("PositionList");
	expose_pullsink_for< Measurement::PositionList2 >("PositionList2");
//	expose_pullsink_for< Measurement::ErrorPositionList >("ErrorPositionList");
//	expose_pullsink_for< Measurement::ErrorPositionList2 >("ErrorPositionList2");



	bp::class_< Facade::AdvancedFacade, boost::shared_ptr< Facade::AdvancedFacade >, boost::noncopyable>("AdvancedFacade", bp::init< bp::optional< const std::string& > >())
		.def("loadDataflow", (void (Facade::AdvancedFacade::*)(const std::string&, bool))&Facade::AdvancedFacade::loadDataflow)
		.def("loadDataflow", (void (Facade::AdvancedFacade::*)(std::istream&, bool))&Facade::AdvancedFacade::loadDataflow)
		.def("clearDataflow", &Facade::AdvancedFacade::clearDataflow)
		.def("startDataflow", &Facade::AdvancedFacade::startDataflow)
		.def("stopDataflow", &Facade::AdvancedFacade::stopDataflow)
		.def("killEverything", &Facade::AdvancedFacade::killEverything)

		// all components .. need a better way do do this .. meta-meta programming needed - havent looking into boost::mpl yet ;)

		.def("getApplicationPushSinkButton", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSinkButton >)
		.def("getApplicationPushSinkPose", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSinkPose >)
//		.def("getApplicationPushSinkErrorPose", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSinkErrorPose >)
		.def("getApplicationPushSinkPosition", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSinkPosition >)
		.def("getApplicationPushSinkPosition2D", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSinkPosition2D >)
		.def("getApplicationPushSinkRotation", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSinkRotation >)
		.def("getApplicationPushSinkMatrix4x4", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSinkMatrix4x4 >)
		.def("getApplicationPushSinkMatrix3x3", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSinkMatrix3x3 >)

		.def("getApplicationPushSinkPositionList", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSinkPositionList >)
		.def("getApplicationPushSinkPositionList2", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSinkPositionList2 >)
//		.def("getApplicationPushSinkErrorPositionList", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSinkErrorPositionList >)
//		.def("getApplicationPushSinkErrorPositionList2", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSinkErrorPositionList2 >)


		.def("getApplicationPullSinkButton", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Button > >)
		.def("getApplicationPullSinkPose", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Pose > >)
//		.def("getApplicationPullSinkErrorPose", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::ErrorPose > >)
		.def("getApplicationPullSinkPosition", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Position > >)
		.def("getApplicationPullSinkPosition2D", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Position2D > >)
		.def("getApplicationPullSinkRotation", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Rotation > >)
		.def("getApplicationPullSinkMatrix4x4", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Matrix4x4 > >)
		.def("getApplicationPullSinkMatrix3x3", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Matrix3x3 > >)
		.def("getApplicationPullSinkVector4D", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Vector4D > >)

		.def("getApplicationPullSinkPositionList", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::PositionList > >)
		.def("getApplicationPullSinkPositionList2", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::PositionList2 > >)
//		.def("getApplicationPullSinkErrorPositionList", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::ErrorPositionList > >)
//		.def("getApplicationPullSinkErrorPositionList2", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::ErrorPositionList2 > >)

		// push sinks
		.def("setCallbackButton", &setWrappedCallbackFacade< Measurement::Button >)
		.def("setCallbackPose", &setWrappedCallbackFacade< Measurement::Pose >)
//		.def("setCallbackErrorPose", &setWrappedCallbackFacade< Measurement::ErrorPose >)
		.def("setCallbackPosition", &setWrappedCallbackFacade< Measurement::Position >)
		.def("setCallbackPosition2D", &setWrappedCallbackFacade< Measurement::Position2D >)
		.def("setCallbackRotation", &setWrappedCallbackFacade< Measurement::Rotation >)
		.def("setCallbackMatrix3x3", &setWrappedCallbackFacade< Measurement::Matrix4x4 >)
		.def("setCallbackMatrix4x4", &setWrappedCallbackFacade< Measurement::Matrix3x3 >)

		.def("setCallbackPositionList", &setWrappedCallbackFacade< Measurement::PositionList >)
		.def("setCallbackPositionList2", &setWrappedCallbackFacade< Measurement::PositionList2 >)
//		.def("setCallbackErrorPositionList", &setWrappedCallbackFacade< Measurement::ErrorPositionList >)
//		.def("setCallbackErrorPositionList2", &setWrappedCallbackFacade< Measurement::ErrorPositionList2 >)








		//.def("addDataflowObserver", &Facade::AdvancedFacade::addDataflowObserver)
		//.def("removeDataflowObserver", &Facade::AdvancedFacade::removeDataflowObserver)
		;


}
