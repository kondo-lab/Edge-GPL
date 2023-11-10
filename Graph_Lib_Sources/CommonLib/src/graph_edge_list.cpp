/* version 3 */
#include "../include/graph_edge_list.h"

namespace mGraphEL {
	STATUS print_graph(Full_Graph_EL &graph, string &strgraph)
	{
		// extract graph info
		Graph_EL &graph_el = graph.first;
		string *vertices = boost::get<string*>(graph.second[0]);
		int *indexes = boost::get<int*>(graph.second[1]);
		string *names = boost::get<string*>(graph.second[2]);
		int *weights = boost::get<int*>(graph.second[3]);
		int n_vertex = boost::get<int>(graph.second[4]);
		typename boost::graph_traits<Graph_EL>::vertex_descriptor v_source, v_target;
		// construct graph string
		strgraph = "Number of vertices = " + std::to_string(n_vertex) + "\n";
		BGL_FORALL_EDGES_T(edge_i, graph_el, Graph_EL) {
			// extract edge endpoints
			v_source = source(edge_i, graph_el);
			v_target = target(edge_i, graph_el);
			// construct edge string
			string edge = "";
			edge += vertices[v_source] + " - " + vertices[v_target];
			edge += " [index=" + std::to_string(*(indexes + edge_i)) + ",";
			edge += " name=\"" + *(names + edge_i) + "\",";
			edge += " weight=" + std::to_string(*(weights + edge_i)) + "]";
			strgraph += edge + "\n";
		}
		return SUCCESS;
	}
	STATUS get_num_vertex(Full_Graph_EL &graph, int &num_vertex)
	{
		num_vertex = boost::get<int>(graph.second[4]);
		return SUCCESS;
	}
	STATUS get_edge_in_vertex(Full_Graph_EL &graph, string name,
		vector<string> &vertices, vector<int> &weights)
	{
		// extract graph info
		Graph_EL &graph_el = graph.first;
		string *vertices_el = boost::get<string*>(graph.second[0]);
		string *names_el = boost::get<string*>(graph.second[2]);
		int *weights_el = boost::get<int*>(graph.second[3]);
		int n_vertex = boost::get<int>(graph.second[4]);
		// 1. Verify if vertex exists
		typename boost::graph_traits<Graph_EL>::vertex_descriptor vertex;
		vertex = std::distance(vertices_el, std::find(vertices_el, vertices_el + n_vertex, name));
		if (vertex == n_vertex) {
			PRINT_STATUS("Vertex '" + name + "' does not exist.");
			return FAIL;
		}
		// 2. Get in-edges
		#ifdef DEBUG_TIME
			// Mark the start time for measurement
			auto start = high_resolution_clock::now();
		#endif
		BGL_FORALL_EDGES_T(edge_i, graph_el, Graph_EL)
			if (target(edge_i, graph_el) == vertex) {
				vertices.push_back(vertices_el[source(edge_i, graph_el)]);
				weights.push_back(weights_el[edge_i]);
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
	STATUS get_edge_out_vertex(Full_Graph_EL &graph, string name,
		vector<string> &vertices, vector<int> &weights)
	{
		// extract graph info
		Graph_EL &graph_el = graph.first;
		string *vertices_el = boost::get<string*>(graph.second[0]);
		string *names_el = boost::get<string*>(graph.second[2]);
		int *weights_el = boost::get<int*>(graph.second[3]);
		int n_vertex = boost::get<int>(graph.second[4]);
		// 1. Verify if vertex exists
		typename boost::graph_traits<Graph_EL>::vertex_descriptor vertex;
		vertex = std::distance(vertices_el, std::find(vertices_el, vertices_el + n_vertex, name));
		if (vertex == n_vertex) {
			PRINT_STATUS("Vertex '" + name + "' does not exist.");
			return FAIL;
		}
		// 2. Get in-edges
		#ifdef DEBUG_TIME
			// Mark the start time for measurement
			auto start = high_resolution_clock::now();
		#endif
		BGL_FORALL_EDGES_T(edge_i, graph_el, Graph_EL)
			if (source(edge_i, graph_el) == vertex) {
				vertices.push_back(vertices_el[target(edge_i, graph_el)]);
				weights.push_back(weights_el[edge_i]);
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
	STATUS run_sps(Full_Graph_EL &graph, string name_from, string name_to, string &list)
	{
		Graph_EL &graph_el = graph.first;
		string *vertices = boost::get<string*>(graph.second[0]);
		int *weights = boost::get<int*>(graph.second[3]);
		int n_vertex = boost::get<int>(graph.second[4]);

		vector<int> predecessors(n_vertex); 
		for (int i = 0; i < n_vertex; ++i)
			predecessors[i] = i;
		vector<float> distance(n_vertex);
		std::fill(distance.begin(), distance.end(), (std::numeric_limits <float>::max)());
		distance[0] = 0;

		boost::bellman_ford_shortest_paths(graph_el, int (n_vertex),
			weight_map(boost::make_iterator_property_map(weights,
					get(boost::edge_index, graph_el),
					*weights
			)).
			distance_map(distance.data()).
			predecessor_map(predecessors.data()));

		try {
			// get vertex index by name
			typename boost::graph_traits<Graph_EL>::vertex_descriptor v_source, v_target;
			v_source = std::distance(vertices,
				std::find(vertices, vertices + n_vertex, name_from));
			v_target = std::distance(vertices,
				std::find(vertices, vertices + n_vertex, name_to));
			if (v_source == n_vertex || v_target == n_vertex)
				throw FAIL;
			// populate vertex list
			vector<string> vertexlist;
			int vertex = v_target;
			while (vertex != predecessors[vertex]) {
				vertexlist.insert(
					vertexlist.begin(),
					*(vertices + vertex)
				);
				vertex = predecessors[vertex];
			}
			if (vertex == v_source) {
				vertexlist.insert(
					vertexlist.begin(),
					*(vertices + vertex)
				);

				// Copy final result to data to be sent
				list += vertexlist[0] + "\n";
				for (int i = 1; i < vertexlist.size(); i++)
					list += vertexlist[i] + "\n";
			}
		} catch (...) {
			PRINT_STATUS("Vertex '" + name_from + "' or vertex '"
				+ name_to +"' does not exist.");
			return FAIL;
		}

		return SUCCESS;
	}
}