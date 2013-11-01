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


template< class EventType >
static void expose_pushsink_for(const std::string& event_name)
{
	std::string pushsink_name("ApplicationPushSink");
	pushsink_name.append(event_name);
	bp::class_< Components::ApplicationPushSink< EventType >, boost::shared_ptr< Components::ApplicationPushSink< EventType > >, bp::bases< Dataflow::Component >, boost::noncopyable>(pushsink_name.c_str(), bp::no_init)
		.def("setCallback", &setWrappedCallback< Components::ApplicationPushSink< EventType >, EventType >)
		;
}

template< class EventType >
static void expose_pullsink_for(const std::string& event_name)
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
	expose_pushsink_for< Measurement::Button >("Button");
	expose_pushsink_for< Measurement::Pose >("Pose");
	expose_pushsink_for< Measurement::ErrorPose >("ErrorPose");
	expose_pushsink_for< Measurement::Position >("Position");
	expose_pushsink_for< Measurement::Position2D >("Position2D");
	expose_pushsink_for< Measurement::Rotation >("Rotation");
	expose_pushsink_for< Measurement::Matrix4x4 >("Matrix4x4");
	expose_pushsink_for< Measurement::Matrix3x3 >("Matrix3x3");

//	expose_pushsink_for< Measurement::PositionList >("PositionList");
//	expose_pushsink_for< Measurement::PositionList2 >("PositionList2");
//	expose_pushsink_for< Measurement::ErrorPositionList >("ErrorPositionList");
//	expose_pushsink_for< Measurement::ErrorPositionList2 >("ErrorPositionList2");

	expose_pullsink_for< Measurement::Button >("Button");
	expose_pullsink_for< Measurement::Pose >("Pose");
	expose_pullsink_for< Measurement::ErrorPose >("ErrorPose");
	expose_pullsink_for< Measurement::Position2D >("Position2D");
	expose_pullsink_for< Measurement::Position >("Position");
	expose_pullsink_for< Measurement::ErrorPosition >("ErrorPosition");
	expose_pullsink_for< Measurement::Rotation >("Rotation");
	expose_pullsink_for< Measurement::Matrix4x4 >("Matrix4x4");
	expose_pullsink_for< Measurement::Matrix3x3 >("Matrix3x3");
	expose_pullsink_for< Measurement::Vector4D >("Vector4D");

//	expose_pullsink_for< Measurement::PositionList >("PositionList");
//	expose_pullsink_for< Measurement::PositionList2 >("PositionList2");
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
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSink< Measurement::Button > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSink< Measurement::Pose > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSink< Measurement::ErrorPose > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSink< Measurement::Position > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSink< Measurement::Position2D > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSink< Measurement::Rotation > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSink< Measurement::Matrix4x4 > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSink< Measurement::Matrix3x3 > >)

//		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSink< Measurement::PositionList > >)
//		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSink< Measurement::PositionList2 > >)
//		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSink< Measurement::ErrorPositionList > >)
//		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPushSink< Measurement::ErrorPositionList2 > >)


		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Button > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Pose > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::ErrorPose > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Position > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Position2D > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Rotation > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Matrix4x4 > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Matrix3x3 > >)
		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::Vector4D > >)

//		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::PositionList > >)
//		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::PositionList2 > >)
//		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::ErrorPositionList > >)
//		.def("componentByName", &Facade::AdvancedFacade::componentByName< Components::ApplicationPullSink< Measurement::ErrorPositionList2 > >)

		// push sinks
		.def("setCallback", &setWrappedCallbackFacade< Measurement::Button >)
		.def("setCallback", &setWrappedCallbackFacade< Measurement::Pose >)
		.def("setCallback", &setWrappedCallbackFacade< Measurement::ErrorPose >)
		.def("setCallback", &setWrappedCallbackFacade< Measurement::Position >)
		.def("setCallback", &setWrappedCallbackFacade< Measurement::Position2D >)
		.def("setCallback", &setWrappedCallbackFacade< Measurement::Rotation >)
		.def("setCallback", &setWrappedCallbackFacade< Measurement::Matrix4x4 >)
		.def("setCallback", &setWrappedCallbackFacade< Measurement::Matrix3x3 >)

//		.def("setCallback", &setWrappedCallbackFacade< Measurement::PositionList >)
//		.def("setCallback", &setWrappedCallbackFacade< Measurement::PositionList2 >)
//		.def("setCallback", &setWrappedCallbackFacade< Measurement::ErrorPositionList >)
//		.def("setCallback", &setWrappedCallbackFacade< Measurement::ErrorPositionList2 >)








		//.def("addDataflowObserver", &Facade::AdvancedFacade::addDataflowObserver)
		//.def("removeDataflowObserver", &Facade::AdvancedFacade::removeDataflowObserver)
		;


}
