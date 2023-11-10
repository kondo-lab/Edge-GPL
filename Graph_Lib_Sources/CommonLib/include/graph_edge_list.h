/* version 3 */
#include "socket.h"

#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/range/irange.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/property_map/property_map.hpp>

namespace mGraphHelperEL {
	/*
		Name: get_el_info
		Function: Separate a graph's edge information into a list of pairs of vertices,
			a list of weights, and the number of vertices in the graph in order to
			construct an edge list graph.
		Input: Any other graph format.
		Output: SUCCESS if the algorithm runs successfully; FAIL otherwise.
		Return: List of pairs of vertices, list of weights, number of vertices
	*/
	template <typename Graph>
	STATUS get_el_info(Graph &graph, std::vector<Edge_EL> &edges_el, std::vector<string> &vertices_el,
		std::vector<int> &indexes, std::vector<string> &names, std::vector<int> &weights,
		int &n_vertices)
	{
		n_vertices = boost::num_vertices(graph);
		vertices_el.resize(n_vertices);
		BGL_FORALL_VERTICES_T(vertex_i, graph, Graph)
			vertices_el[vertex_i] = graph[vertex_i].name;
		BGL_FORALL_EDGES_T(edge_i, graph, Graph) {
			edges_el.push_back(std::make_pair(
				source(edge_i, graph),
				target(edge_i, graph)
			));
			indexes.push_back(graph[edge_i].index);
			names.push_back(graph[edge_i].name);
			weights.push_back(graph[edge_i].weight);
		}
		return SUCCESS;
	}
}

namespace mGraphEL {
	/*
		Name: print_graph
		Function: Constructs a custom string representation of the edge list graph.
		Input: Edge list graph.
		Output: SUCCESS if the algorithm runs successfully; FAIL otherwise.
		Return: String representation of edge list graph.
	*/
	STATUS print_graph(Full_Graph_EL &graph, string &strgraph);
	/*
		Name: get_num_vertex
		Function: Returns the number of vertices in the edge list graph.
		Input: none
		Output: SUCCESS
		Return: Number of vertices.
	*/
	STATUS get_num_vertex(Full_Graph_EL &graph, int &num_vertex);	/*
		Name: get_edge_in_vertex
		Function: Retrieves the in-edges of the supplied vertex.
		Input: Vertex name.
		Output: FAIL if the supplied vertex does not exist; SUCCESS otherwise.
		Return: Names and weights of in-edges.
	*/
	STATUS get_edge_in_vertex(Full_Graph_EL &graph, string name,
		vector<string> &names, vector<int> &weights);
	/*
		Name: get_edge_out_vertex
		Function: Retrieves the out-edges of the supplied vertex.
		Input: Vertex name.
		Output: FAIL if the supplied vertex does not exist; SUCCESS otherwise.
		Return: Names and weights of out-edges.
	*/
	STATUS get_edge_out_vertex(Full_Graph_EL &graph, string name,
		vector<string> &names, vector<int> &weights);
	/*
		Name: run_sps
		Function: Apply Bellman-Ford Shortest Path algorithm on edge list graph.
		Input: Edge list graph, names of source and target vertices.
		Output: SUCCESS if the algorithm runs successfully; FAIL otherwise.
		Return: List of vertices and edges traversed.
	*/
	STATUS run_sps(Full_Graph_EL &graph, string name_from, string name_to, string &list);
	/*
		Name: change_graph_format
		Function: Changes the edge list to specified format.
		Input: Graph format to change into
		Output/Return: SUCCESS if the format is changed; FAIL otherwise.
	*/
	// Overload: Conversion from edge list
	template <typename NewGraph>
	STATUS change_graph_format(Full_Graph_EL &current_graph, NewGraph &new_graph, int new_format)
	{
		Graph_EL &graph = current_graph.first;
		string *vertices = boost::get<string*>(current_graph.second[0]);
		int *indexes = boost::get<int*>(current_graph.second[1]);
		string *names = boost::get<string*>(current_graph.second[2]);
		int *weights = boost::get<int*>(current_graph.second[3]);
		int n_vertex = boost::get<int>(current_graph.second[4]);
		#ifdef DEBUG_TIME
			// Mark the start time for measurement
			auto start = high_resolution_clock::now();
		#endif
		// 1. Copy vertex data
		typename boost::graph_traits<NewGraph>::vertex_descriptor vertex;
		for (int vertex_i = 0; vertex_i < n_vertex; vertex_i++) {
			switch (new_format) {
			case ADJACENCY_LIST:
			case ADJACENCY_LIST_UNDIRECTED:
				vertex = boost::add_vertex(new_graph);
				new_graph[vertex].name = vertices[vertex_i];
				break;
			default:
				new_graph[vertex_i].name = vertices[vertex_i];
				break;
			}
		}
		// 2. Copy edge data
		std::pair<
			typename boost::graph_traits<NewGraph>::edge_descriptor,
			bool
		> edge;
		typename boost::graph_traits<NewGraph>::vertex_descriptor v_source, v_target, v_temp;
		std::regex reg_name("([a-zA-Z0-9_.-]+)( - )([a-zA-Z0-9_.-]+)");
		BGL_FORALL_EDGES_T(edge_i, graph, Graph_EL) {
			// 2.1 Extract edge endpoints' information
			smatch matches;
			regex_search(*(names + edge_i), matches, reg_name);
			string vname_source = matches.str(1),
			       vname_target = matches.str(3);
			// 2.2 Verify endpoint order
			v_source = source(edge_i, graph);
			v_target = target(edge_i, graph);
			if (vertices[v_source] != vname_source) {
				v_temp = v_source;
				v_source = v_target;
				v_target = v_temp;
			}
			// 2.3 Add edge to new graph
			edge = boost::add_edge(v_source, v_target, new_graph);
			new_graph[edge.first].index = *(indexes + edge_i);
			new_graph[edge.first].name = vertices[v_source] + " - " + vertices[v_target];
			new_graph[edge.first].weight = *(weights + edge_i);
		}
		#ifdef DEBUG_TIME
			// Mark the end time for measurement
			auto stop = high_resolution_clock::now();
			// Get thread runtime duration
			auto duration = duration_cast<microseconds>(stop - start);
			std::cout << "[INFO] Algorithm runtime = "
				<< duration.count() << "[µs]"
				<< endl;
		#endif
		return SUCCESS;
	}
}