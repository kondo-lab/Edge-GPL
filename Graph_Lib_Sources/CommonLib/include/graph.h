/* version 3 */
#pragma once

#include <bits/stdc++.h>

#include <boost/variant.hpp>
#include <boost/variant/variant.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/edge_list.hpp>
#include <boost/graph/iteration_macros.hpp>

using namespace std;
using namespace boost;

#define DEBUG_EXPORT_GRAPH_IMAGE
// #define PRINT_SUB_GRAPH // just print the subgraph

struct Node {
	string name = "";

	#ifdef DEBUG_EXPORT_GRAPH_IMAGE
	string color = "black";
	#endif
};

struct Edge {
	int index = 0;
	string name = "";
	int weight = 0;

	#ifdef DEBUG_EXPORT_GRAPH_IMAGE
	string color = "black";
	#endif
};

// Graph types
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Node, Edge> Graph_AL;
typedef boost::adjacency_matrix<boost::directedS, Node, Edge> Graph_AM;
typedef boost::compressed_sparse_row_graph<boost::directedS, Node, Edge> Graph_CSR;
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, Node, Edge> Graph_ALU;
typedef boost::adjacency_matrix<boost::undirectedS, Node, Edge> Graph_AMU;
// edge list
typedef std::pair<int, int> Edge_EL;
typedef std::vector<boost::variant<string*, int*, string*, int*, int>> Meta_EL;
typedef boost::edge_list<Edge_EL*> Graph_EL;
typedef std::pair<Graph_EL, Meta_EL> Full_Graph_EL;
// generic
typedef boost::variant<bool, Graph_AL*, Graph_AM*, Graph_CSR*, Graph_ALU*, Graph_AMU*, Full_Graph_EL*> GraphPtr;

enum STATUS {
	SUCCESS,
	FAIL
};

enum GRAPH_FORMAT {
	NONE,
	ADJACENCY_LIST,
	ADJACENCY_MATRIX,
	COMPRESSED_SPARSE_ROW,
	ADJACENCY_LIST_UNDIRECTED,
	ADJACENCY_MATRIX_UNDIRECTED,
	EDGE_LIST
};

// Helper funtions
template <typename Descriptor, typename Graph>
STATUS get_vertex_from_name(Descriptor &vertex, Graph &graph, string name)
{
	BGL_FORALL_VERTICES_T(vertex_i, graph, Graph)
		if ((graph[vertex_i].name).compare(name) == SUCCESS) {
			vertex = vertex_i;
			return SUCCESS;
		}
	return FAIL;
}

template <typename Descriptor, typename Graph>
STATUS get_edge_from_name(Descriptor *edge, Graph &graph, string name)
{
	BGL_FORALL_EDGES_T(edge_i, graph, Graph)
		if ((graph[edge_i].name).compare(name) == SUCCESS) {
			edge->first = edge_i;
            edge->second = true;
			return SUCCESS;
		}
	return FAIL;
}

template <typename Graph>
bool edge_exist(Graph& graph, string vname1, string vname2)
{
	typename graph_traits<Graph>::vertex_descriptor v1, v2;
	if (get_vertex_from_name(v1, graph, vname1) == FAIL ||
		get_vertex_from_name(v2, graph, vname2) == FAIL)
		return false;

	typename graph_traits<Graph>::edge_descriptor e;
	bool e_exist;
	tie(e, e_exist) = edge(v1, v2, graph);

	return e_exist;
}

template <typename Graph>
bool vertex_exist(Graph& graph, string vname)
{
	typename graph_traits<Graph>::vertex_descriptor v;
	return (get_vertex_from_name(v, graph, vname) == SUCCESS);
}
