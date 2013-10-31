#include <boost/numpy.hpp>
#include <complex>

#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/to_python_converter.hpp>

#include <utDataflow/Component.h>
#include <utDataflow/DataflowNetwork.h>
#include <utDataflow/Port.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

#include <iostream>

using namespace Ubitrack;

namespace bp = boost::python;
namespace bn = boost::numpy;

namespace {

}


BOOST_PYTHON_MODULE(_utdataflow)
{
	bp::class_< Dataflow::Component, boost::shared_ptr< Dataflow::Component >, boost::noncopyable>("Component", bp::init< const std::string& >())
			.def("getName", &Dataflow::Component::getName
					,bp::return_value_policy<bp::copy_const_reference>()
					)
			.def("addPort", &Dataflow::Component::addPort)
			.def("removePort", &Dataflow::Component::removePort)
			.def("getPortByName", &Dataflow::Component::getPortByName
					,bp::return_internal_reference<>()
					)
			.def("start", &Dataflow::Component::start)
			.def("stop", &Dataflow::Component::stop)
			.def("setEventPriority", &Dataflow::Component::setEventPriority)
			.def("getEventPriority", &Dataflow::Component::getEventPriority)
			;

	bp::class_< Dataflow::Port, boost::shared_ptr< Dataflow::Port >, boost::noncopyable>("Port", bp::init<const std::string&, Dataflow::Component& >())
			.def("getName", &Dataflow::Port::getName
					,bp::return_value_policy<bp::copy_const_reference>()
					)
			.def("fullName", &Dataflow::Port::fullName
					,bp::return_value_policy<bp::manage_new_object>()
					)
			.def("getComponent", &Dataflow::Port::getComponent
					,bp::return_internal_reference<>()
					)
			.def("connect", &Dataflow::Port::connect)
			.def("disconnect", &Dataflow::Port::disconnect)
			;


}
