/* version 3 */
/*
	Filename: graph_generator.cpp
	This program generates a random graph and then saves to a file "graph.gv"
	Input: size of graph (number of vertices and edges)

    How to compile: $ make graphgen
    How to run: $ ./bin/graphgen
*/
#include <bits/stdc++.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/range/irange.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/iteration_macros.hpp>

using namespace std;
using namespace boost;

struct Node {
	string name = "";
};

struct Edge {
	int index = 0;
	string name = "";
	int weight = 0;
};

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Node, Edge> GraphD;
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, Node, Edge> GraphU;

int main() {
	int num_vertices = 0,
	    num_edges = 0,
	    index = 0,
	    graph_type = 0;
	cout << "Enter number of vertices: ";
	cin >> num_vertices;
	cout << "Enter number of edges: ";
	cin >> num_edges;
	cout << "Enter graph type (0 - directed | 1 - undirected): ";
	cin >> graph_type;

	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, num_vertices);

	if (graph_type == 0) {
		GraphD graph;
		boost::graph_traits<GraphD>::vertex_descriptor source, target;

		boost::generate_random_graph(graph, num_vertices, num_edges, generator);

		index = 0;
		BGL_FORALL_VERTICES_T(vertex, graph, GraphD)
			graph[vertex].name = to_string(index++);
		index = 0;
		BGL_FORALL_EDGES_T(edge, graph, GraphD) {
			source = boost::source(edge, graph);
			target = boost::target(edge, graph);
			graph[edge].index = index++;
			graph[edge].name = to_string(source) + " - " + to_string(target);
			graph[edge].weight = distribution(generator);
		}

		boost::dynamic_properties dp(ignore_other_properties);
		dp.property("node_id", get(&Node::name, graph));
		dp.property("index", get(&Edge::index, graph));
		dp.property("name", get(&Edge::name, graph));
		dp.property("weight", get(&Edge::weight, graph));
		dp.property("label", get(&Edge::weight, graph));

		ofstream graph_file("graph.gv");
		write_graphviz_dp(graph_file, graph, dp, "node_id");
		graph_file.close();

		cout << "Exported graph to file graph.gv" << endl;
	} else if (graph_type == 1) {
		GraphU graph;
		boost::graph_traits<GraphU>::vertex_descriptor source, target;

		boost::generate_random_graph(graph, num_vertices, num_edges, generator);

		index = 0;
		BGL_FORALL_VERTICES_T(vertex, graph, GraphU)
			graph[vertex].name = to_string(index++);
		index = 0;
		BGL_FORALL_EDGES_T(edge, graph, GraphU) {
			source = boost::source(edge, graph);
			target = boost::target(edge, graph);
			graph[edge].index = index++;
			graph[edge].name = to_string(source) + " - " + to_string(target);
			graph[edge].weight = distribution(generator);
		}

		boost::dynamic_properties dp(ignore_other_properties);
		dp.property("node_id", get(&Node::name, graph));
		dp.property("index", get(&Edge::index, graph));
		dp.property("name", get(&Edge::name, graph));
		dp.property("weight", get(&Edge::weight, graph));
		dp.property("label", get(&Edge::weight, graph));

		ofstream graph_file("graph.gv");
		write_graphviz_dp(graph_file, graph, dp, "node_id");
		graph_file.close();

		cout << "Exported graph to file graph.gv" << endl;
	} else {
		cout << "Failed to generate graph" << endl;
	}

	// system("dot -Tpng graph.gv -o graph.png");

	return 0;
}