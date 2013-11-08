#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <boost/numpy.hpp>
#include <numpy/arrayobject.h>
#include <complex>

#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/to_python_converter.hpp>

#include <utGraph/UTQLDocument.h>
#include <utGraph/UTQLReader.h>
#include <utGraph/Graph.h>


#include <iostream>

using namespace Ubitrack;

namespace bp = boost::python;
namespace bn = boost::numpy;

namespace {

boost::shared_ptr< Graph::UTQLDocument > UTQLReader_processInput( std::istream& input ) {
	// next todo .. factor out streambuf headers and make them available for include
	// then load utqlfile and test basic graph wrapping
	boost::shared_ptr< Graph::UTQLDocument > ret; //.reset(new ...)
	return ret;
	}
}

typedef Graph::Graph< Graph::UTQLNode, Graph::UTQLEdge > UTQLGraph;

BOOST_PYTHON_MODULE(_utgraph)
{


	bp::class_< UTQLGraph::Node, boost::shared_ptr< UTQLGraph::Node > >("UTQLGraphNode", bp::init<const std::string&, const Graph::UTQLNode&>())
		.def_readwrite("Name", &UTQLGraph::Node::m_Name)
//		.def_readwrite("InEdges", &UTQLGraph::Node::m_InEdges)
//		.def_readwrite("OutEdges", &UTQLGraph::Node::m_OutEdges)
		;

	bp::class_< UTQLGraph::Edge, boost::shared_ptr< UTQLGraph::Edge > >("UTQLGraphEdge", bp::no_init)
		// .def(bp::init<const std::string&, Graph::GraphEdge< Graph::UTQLNode, Graph::UTQLEdge >::WeakNodePtr, Graph::GraphEdge< Graph::UTQLNode, Graph::UTQLEdge >::WeakNodePtr >())
		.def_readwrite("Name", &UTQLGraph::Edge::m_Name)
//		.def_readwrite("Source", &UTQLGraph::Edge::m_Source)
//		.def_readwrite("Target", &UTQLGraph::Edge::m_Target)
		;


	bp::class_< UTQLGraph, boost::shared_ptr< UTQLGraph > >("UTQLGraph", bp::init<>())
//		.def("addNode", &UTQLGraph::addNode)
		.def("hasNode", &UTQLGraph::hasNode)
//		.def("getNode", &UTQLGraph::getNode
//				,bp::return_internal_reference<>()
//				)
		//.def("removeNode", &UTQLGraph::removeNode)
		//.def("addEdge", &UTQLGraph::addEdge)
		.def("hasEdge", &UTQLGraph::hasEdge)
//		.def("getEdge", &UTQLGraph::getEdge
//				,bp::return_internal_reference<>()
//				)
		//.def("removeEdge", &UTQLGraph::removeEdge)
		.def("size", &UTQLGraph::size)
		.def("order", &UTQLGraph::order)
		.def("empty", &UTQLGraph::order)
		.def("null", &UTQLGraph::null)
		;

	bp::class_< Graph::UTQLDocument, boost::shared_ptr< Graph::UTQLDocument > >("UTQLDocument", bp::init<bool>())
//		.def("addSubgraph", &Graph::UTQLDocument::addSubgraph)
		.def("hasSubgraphById", &Graph::UTQLDocument::hasSubgraphById)
//		.def("getSubgraphById", &Graph::UTQLDocument::getSubgraphById
//				,bp::return_internal_reference<>()
//				)
//		.def("removeSubgraphById", &Graph::UTQLDocument::removeSubgraphById)
//		.def("exportDot", &Graph::UTQLDocument::exportDot)
		.def("isRequest", &Graph::UTQLDocument::isRequest)
		// properties
		//.def_readwrite("SubGraphById", &Graph::UTQLDocument::m_SubgraphById)
		//.def_readwrite("SubgraphByName", &Graph::UTQLDocument::m_SubgraphByName)
		//.def_readwrite("Subgraphs", &Graph::UTQLDocument::m_Subgraphs)
		;

	bp::class_< Graph::UTQLSubgraph, boost::shared_ptr< Graph::UTQLSubgraph >, bp::bases<UTQLGraph> >("UTQLSubgraph", bp::init<std::string, std::string>())
		.def_readwrite("ID", &Graph::UTQLSubgraph::m_ID)
		.def_readwrite("Name", &Graph::UTQLSubgraph::m_Name)
//		.def_readwrite("onlyBestEdgeMatch", &Graph::UTQLSubgraph::m_onlyBestEdgeMatch)
		//.def_readwrite("bestMatchExpression", &Graph::UTQLSubgraph::m_bestMatchExpression)
//		.def_readwrite("DataflowConfiguration", &Graph::UTQLSubgraph::m_DataflowConfiguration)
//		.def_readwrite("DataflowAttributes", &Graph::UTQLSubgraph::m_DataflowAttributes)
		.def_readwrite("DataflowClass", &Graph::UTQLSubgraph::m_DataflowClass)
		;

	bp::class_< Graph::UTQLEdge, boost::shared_ptr< Graph::UTQLEdge > >("UTQLEdge", bp::no_init)
		//.def(bp::init<Graph::InOutAttribute::Tag>())
//		.def_readwrite("EdgeReference", &Graph::UTQLEdge::m_EdgeReference)
		;

	bp::class_< Graph::UTQLNode, boost::shared_ptr< Graph::UTQLNode > >("UTQLNode", bp::no_init)
		// .def(bp::init<Graph::InOutAttribute::Tag>())
		.def_readwrite("QualifiedName", &Graph::UTQLNode::m_QualifiedName)
		;

	bp::class_< Graph::EdgeReference, boost::shared_ptr< Graph::EdgeReference > >("EdgeReference", bp::no_init)
		// .def(bp::init<const std::string&, const std::string&>())
		.def("getSubgraphId", &Graph::EdgeReference::getSubgraphId
				,bp::return_value_policy<bp::copy_const_reference>()
				)
		.def("getEdgeName", &Graph::EdgeReference::getEdgeName
				,bp::return_value_policy<bp::copy_const_reference>()
				)
		.def("empty", &Graph::EdgeReference::empty)
		;


	bp::def("readUTQLDocument", &UTQLReader_processInput);



}
