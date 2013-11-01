#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <boost/numpy.hpp>
#include <numpy/arrayobject.h>
#include <complex>

#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/to_python_converter.hpp>

#include <utFacade/AdvancedFacade.h>


#include <iostream>

using namespace Ubitrack;

namespace bp = boost::python;
namespace bn = boost::numpy;

namespace {

}


BOOST_PYTHON_MODULE(_utfacade)
{
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
