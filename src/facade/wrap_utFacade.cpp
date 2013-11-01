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

//
//template< typename T >
//void pushconsumer_setCallback(bp::object const & cb) {
//
//
//}
//
//
//struct expose_pushconsumer
//{
//	template< class EventType >
//    static void expose(const std::string& name)
//    {
//		bp::class_< Dataflow::ApplicationPushSink< EventType >, boost::shared_ptr< Dataflow::ApplicationPushSink< EventType >  >(name, bp::no_init)
//			.def("setCallback", &get_measurement< T>
//					//,return_value_policy<reference_existing_object>()
//					,bp::return_value_policy<bp::copy_const_reference>()
//					//,return_internal_reference<>()
//					)
//            ;
//    }
//};


//template< class EventType >
//struct apppushsink_exposer
//{
//    template <class C>
//    static void expose(C const& c)
//    {
//        const_cast<C&>(c)
//				.def(bp::init<Measurement::Timestamp>())
//				.def(bp::init<Measurement::Timestamp, const typename T:: value_type& >())
//				.def("time", (Measurement::Timestamp (T::*)() const)&T::time)
//				.def("set_time", (void (T::*)(Measurement::Timestamp))&T::time)
//				.def("invalid", (bool (T::*)())&T::invalid)
//				.def("invalidate", (void (T::*)())&T::invalidate)
//				.def("get", &get_measurement< T>
//						//,return_value_policy<reference_existing_object>()
//        				,bp::return_value_policy<bp::copy_const_reference>()
//						//,return_internal_reference<>()
//						)
//				.def(bp::self_ns::str(bp::self_ns::self))
//            ;
//    }
//};




}


BOOST_PYTHON_MODULE(_utfacade)
{
	bp::class_< Components::ApplicationPushSink< Measurement::Button >, boost::shared_ptr< Components::ApplicationPushSink< Measurement::Button > >, bp::bases< Dataflow::Component >, boost::noncopyable>("ApplicationPushSinkButton", bp::no_init)
		.def("setCallback", &Components::ApplicationPushSink< Measurement::Button >::setCallback)
		;


	bp::class_< Facade::AdvancedFacade, boost::shared_ptr< Facade::AdvancedFacade >, boost::noncopyable>("AdvancedFacade", bp::init< bp::optional< const std::string& > >())
		.def("loadDataflow", (void (Facade::AdvancedFacade::*)(const std::string&, bool))&Facade::AdvancedFacade::loadDataflow)
		.def("loadDataflow", (void (Facade::AdvancedFacade::*)(std::istream&, bool))&Facade::AdvancedFacade::loadDataflow)
		.def("clearDataflow", &Facade::AdvancedFacade::clearDataflow)
		.def("startDataflow", &Facade::AdvancedFacade::startDataflow)
		.def("stopDataflow", &Facade::AdvancedFacade::stopDataflow)
		.def("killEverything", &Facade::AdvancedFacade::killEverything)
		//.def("componentByName", &Facade::AdvancedFacade::componentByName<Ubitrack::Dataflow::Component>)
		//.def("setCallback", &Facade::AdvancedFacade::setCallback<EventType>)
		//.def("addDataflowObserver", &Facade::AdvancedFacade::addDataflowObserver)
		//.def("removeDataflowObserver", &Facade::AdvancedFacade::removeDataflowObserver)
		;


}
