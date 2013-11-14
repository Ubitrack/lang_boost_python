#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <boost/numpy.hpp>
#include <numpy/arrayobject.h>
#include <complex>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/to_python_converter.hpp>


#include <utGraph/UTQLDocument.h>
#include <utGraph/UTQLReader.h>
#include <utGraph/Graph.h>


#include <iostream>

#include <ubitrack_python/wrap_streambuf.h>

using namespace Ubitrack;
using namespace Ubitrack::Python;

namespace bp = boost::python;
namespace bn = boost::numpy;

// type declaration shortcut
typedef Graph::Graph< Graph::UTQLNode, Graph::UTQLEdge > UTQLGraph;


namespace {

template< class Key, class Val >
struct map_item
{
    typedef std::map<Key,Val> Map;

    static Val& get(Map& self, const Key idx) {
      if( self.find(idx) != self.end() ) return self[idx];
      PyErr_SetString(PyExc_KeyError,"Map key not found");
      bp::throw_error_already_set();
    }

    static void set(Map& self, const Key idx, const Val val) { self[idx]=val; }

    static void del(Map& self, const Key n) { self.erase(n); }

    static bool in(Map const& self, const Key n) { return self.find(n) != self.end(); }

    static bp::list keys(Map const& self)
    {
        bp::list t;
        for(typename Map::const_iterator it=self.begin(); it!=self.end(); ++it)
            t.append(it->first);
        return t;
    }
    static bp::list values(Map const& self)
    {
        bp::list t;
        for(typename Map::const_iterator it=self.begin(); it!=self.end(); ++it)
            t.append(it->second);
        return t;
    }
    static bp::list items(Map const& self)
    {
        bp::list t;
        for(typename Map::const_iterator it=self.begin(); it!=self.end(); ++it)
            t.append( bp::make_tuple(it->first, it->second) );
        return t;
    }
};


boost::shared_ptr< Graph::UTQLDocument > UTQLReader_processInput( streambuf& sb ) {
	streambuf::istream stream(sb);
	// then load utqlfile and test basic graph wrapping
	boost::shared_ptr< Graph::UTQLDocument > doc;
	doc = Graph::UTQLReader::processInput( stream );
	return doc;
	}


UTQLGraph::NodePtr get_SourceNode_from_weak_ptr(const UTQLGraph::Edge& e) {
	UTQLGraph::NodePtr sp = e.m_Source.lock ();
	return sp;
}

UTQLGraph::NodePtr get_TargetNode_from_weak_ptr(const UTQLGraph::Edge& e) {
	UTQLGraph::NodePtr sp = e.m_Target.lock ();
	return sp;
}

bp::list get_InEdgeList_from_weak_ptr(const UTQLGraph::Node& n) {
	bp::list edges;
	for (UTQLGraph::Node::EdgeList::const_iterator it = n.m_InEdges.begin(); it != n.m_InEdges.end(); it++) {
		UTQLGraph::EdgePtr ep = it->lock ();
		if (ep) {
			edges.append(*(ep.get()));
		}
	}
	return edges;
}

bp::list get_OutEdgeList_from_weak_ptr(const UTQLGraph::Node& n) {
	bp::list edges;
	for (UTQLGraph::Node::EdgeList::const_iterator it = n.m_OutEdges.begin(); it != n.m_OutEdges.end(); it++) {
		UTQLGraph::EdgePtr ep = it->lock ();
		if (ep) {
			edges.append(*(ep.get()));
		}
	}
	return edges;
}

}


BOOST_PYTHON_MODULE(_utgraph)
{


	bp::class_< UTQLGraph::Node, boost::shared_ptr< UTQLGraph::Node > >("UTQLGraphNode", bp::init<const std::string&, const Graph::UTQLNode&>())
		.def_readwrite("Name", &UTQLGraph::Node::m_Name)
		.def("getInEdges", &get_InEdgeList_from_weak_ptr)
		.def("getOutEdges", &get_OutEdgeList_from_weak_ptr)
		;

	bp::class_< UTQLGraph::Edge, boost::shared_ptr< UTQLGraph::Edge > >("UTQLGraphEdge", bp::no_init)
		// .def(bp::init<const std::string&, Graph::GraphEdge< Graph::UTQLNode, Graph::UTQLEdge >::WeakNodePtr, Graph::GraphEdge< Graph::UTQLNode, Graph::UTQLEdge >::WeakNodePtr >())
		.def_readwrite("Name", &UTQLGraph::Edge::m_Name)
		.def("getSource", &get_SourceNode_from_weak_ptr)
		.def("getTarget", &get_TargetNode_from_weak_ptr)
		;


	bp::class_< UTQLGraph, boost::shared_ptr< UTQLGraph > >("UTQLGraph", bp::no_init)
		.def("addNode", &UTQLGraph::addNode)
		.def("hasNode", &UTQLGraph::hasNode)
		.def("getNode", &UTQLGraph::getNode)

//		.def("removeNode", &UTQLGraph::removeNode)
//		.def("addEdge", &UTQLGraph::addEdge)
		.def("hasEdge", &UTQLGraph::hasEdge)
		.def("getEdge", &UTQLGraph::getEdge)

//		.def("removeEdge", &UTQLGraph::removeEdge)
		.def("size", &UTQLGraph::size)
		.def("order", &UTQLGraph::order)
		.def("empty", &UTQLGraph::order)
		.def("null", &UTQLGraph::null)

		.def_readonly("Nodes", &UTQLGraph::m_Nodes)
		.def_readonly("Edges", &UTQLGraph::m_Edges)
		;

	bp::class_< Graph::UTQLDocument, boost::shared_ptr< Graph::UTQLDocument > >("UTQLDocument", bp::init<bool>())
		.def("addSubgraph", &Graph::UTQLDocument::addSubgraph)
		.def("hasSubgraphById", &Graph::UTQLDocument::hasSubgraphById)
//		.def("getSubgraphById", &Graph::UTQLDocument::getSubgraphById
//				,bp::return_internal_reference<>()
//				)
		.def("removeSubgraphById", &Graph::UTQLDocument::removeSubgraphById)
		.def("exportDot", &Graph::UTQLDocument::exportDot)
		.def("isRequest", &Graph::UTQLDocument::isRequest)
		// properties
		.def_readonly("SubgraphById", &Graph::UTQLDocument::m_SubgraphById)
//		.def_readonly("SubgraphByName", &Graph::UTQLDocument::m_SubgraphByName)
//		.def_readonly("Subgraphs", &Graph::UTQLDocument::m_Subgraphs)
		;

	bp::class_< Graph::UTQLSubgraph, boost::shared_ptr< Graph::UTQLSubgraph >, bp::bases< UTQLGraph > >("UTQLSubgraph", bp::init<std::string, std::string>())
		.def_readwrite("ID", &Graph::UTQLSubgraph::m_ID)
		.def_readwrite("Name", &Graph::UTQLSubgraph::m_Name)
//		.def_readwrite("onlyBestEdgeMatch", &Graph::UTQLSubgraph::m_onlyBestEdgeMatch)
		//.def_readwrite("bestMatchExpression", &Graph::UTQLSubgraph::m_bestMatchExpression)
		.def_readwrite("DataflowConfiguration", &Graph::UTQLSubgraph::m_DataflowConfiguration)
		.def_readwrite("DataflowAttributes", &Graph::UTQLSubgraph::m_DataflowAttributes)
		.def_readwrite("DataflowClass", &Graph::UTQLSubgraph::m_DataflowClass)
		;


	bp::class_< Graph::UTQLEdge, boost::shared_ptr< Graph::UTQLEdge > >("UTQLEdge", bp::no_init)
		//.def(bp::init<Graph::InOutAttribute::Tag>())
		.def_readwrite("EdgeReference", &Graph::UTQLEdge::m_EdgeReference)
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

	bp::class_<Graph::UTQLDocument::SubgraphIDMap>("UTQLDocument_SubgraphIDMap")
	  .def("__len__", &Graph::UTQLDocument::SubgraphIDMap::size)
	  .def("__getitem__", &map_item< std::string, boost::shared_ptr< Graph::UTQLSubgraph > >().get
			  ,bp::return_value_policy<bp::copy_non_const_reference>()
			  )
	  .def("__setitem__", &map_item< std::string, boost::shared_ptr< Graph::UTQLSubgraph > >().set)
	  .def("__delitem__", &map_item< std::string, boost::shared_ptr< Graph::UTQLSubgraph > >().del)
	  .def("clear", &Graph::UTQLDocument::SubgraphIDMap::clear)
	  .def("__contains__", &map_item< std::string, boost::shared_ptr< Graph::UTQLSubgraph > >().in)
	  .def("has_key", &map_item< std::string, boost::shared_ptr< Graph::UTQLSubgraph > >().in)
	  .def("keys", &map_item< std::string, boost::shared_ptr< Graph::UTQLSubgraph > >().keys)
	  .def("values", &map_item< std::string, boost::shared_ptr< Graph::UTQLSubgraph > >().values)
	  .def("items", &map_item< std::string, boost::shared_ptr< Graph::UTQLSubgraph > >().items)
	  ;


	bp::class_<UTQLGraph::NodeMap>("UTQLSubgraph_NodeMap")
	  .def("__len__", &UTQLGraph::NodeMap::size)
	  .def("__getitem__", &map_item< std::string, UTQLGraph::NodePtr >().get
			  ,bp::return_value_policy<bp::copy_non_const_reference>()
			  )
	  .def("__setitem__", &map_item< std::string, UTQLGraph::NodePtr >().set)
	  .def("__delitem__", &map_item< std::string, UTQLGraph::NodePtr >().del)
	  .def("clear", &UTQLGraph::NodeMap::clear)
	  .def("__contains__", &map_item< std::string, UTQLGraph::NodePtr >().in)
	  .def("has_key", &map_item< std::string, UTQLGraph::NodePtr >().in)
	  .def("keys", &map_item< std::string, UTQLGraph::NodePtr >().keys)
	  .def("values", &map_item< std::string, UTQLGraph::NodePtr >().values)
	  .def("items", &map_item< std::string, UTQLGraph::NodePtr >().items)
	  ;

	bp::class_<UTQLGraph::EdgeMap>("UTQLSubgraph_EdgeMap")
	  .def("__len__", &UTQLGraph::EdgeMap::size)
	  .def("__getitem__", &map_item< std::string, UTQLGraph::EdgePtr >().get
			  ,bp::return_value_policy<bp::copy_non_const_reference>()
			  )
	  .def("__setitem__", &map_item< std::string, UTQLGraph::EdgePtr >().set)
	  .def("__delitem__", &map_item< std::string, UTQLGraph::EdgePtr >().del)
	  .def("clear", &UTQLGraph::EdgeMap::clear)
	  .def("__contains__", &map_item< std::string, UTQLGraph::EdgePtr >().in)
	  .def("has_key", &map_item< std::string, UTQLGraph::EdgePtr >().in)
	  .def("keys", &map_item< std::string, UTQLGraph::EdgePtr >().keys)
	  .def("values", &map_item< std::string, UTQLGraph::EdgePtr >().values)
	  .def("items", &map_item< std::string, UTQLGraph::EdgePtr >().items)
	  ;


//	bp::class_<std::vector< boost::shared_ptr< Graph::UTQLSubgraph > > >("SubgraphList")
//			.def(bp::vector_indexing_suite<std::vector< boost::shared_ptr< Graph::UTQLSubgraph > > >())
//			;

	bp::def("readUTQLDocument", &UTQLReader_processInput);



}
