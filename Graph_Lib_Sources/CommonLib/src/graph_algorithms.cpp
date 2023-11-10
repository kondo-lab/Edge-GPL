/* version 3 */
// BEGIN GRAPH_EXTEND
#include "../include/graph_algorithms.h"

namespace mGraphExtend
{
	// Find the next vertex with shortest path
	// (A more efficient implementation uses a priority queue - this is a future work)
	int find_min_idx(vector<spath>& d, vector<bool>& explored)
	{
		int pos = -1;

		for (int i = 0; i < d.size(); i++)
		{
			if (!explored[i])
			{
				if (pos == -1 || d[i].min_dist < d[pos].min_dist)
					pos = i;
			}
		}

		return pos;
	}

	// Find the index of a vertex with a given ID
	int find_vertex_idx(vector<vertex>& graph, int v)
	{
		for (int i = 0; i < graph.size(); i++)
		{
			if (graph[i].vertex_id == v)
				return i;
		}

		return -1;
	}

	// Get shortest paths by going backward from vertex v to source vertex
	void get_spath(vector<vertex>& graph, vector<spath>& d, int v, vector<vector<int>>& path, int path_idx)
	{
		path[path_idx].push_back(v);
		int vi = find_vertex_idx(graph, v);

		if (d[vi].predecessors.empty()) // already return to source vertex
			return;

		if (d[vi].predecessors.size() > 1) // multiple paths to here
		{
			for (int i = 1; i < d[vi].predecessors.size(); i++)
			{
				path.push_back(path[path_idx]); // duplicate the path from here to destination
				get_spath(graph, d, d[vi].predecessors[i], path, path.size() - 1);
			}
		}

		get_spath(graph, d, d[vi].predecessors[0], path, path_idx);
	}

	void get_spath_bf(vector<spath>& d, int v, vector<vector<int>>& path, int path_idx)
	{
		path[path_idx].push_back(v);
		
		if (d[v].predecessors.empty()) // already return to source vertex
			return;

		if (d[v].predecessors.size() > 1) // multiple paths to here
		{
			for (int i = 1; i < d[v].predecessors.size(); i++)
			{
				path.push_back(path[path_idx]); // duplicate the path from here to destination
				get_spath_bf(d, d[v].predecessors[i], path, path.size() - 1);
			}
		}

		get_spath_bf(d, d[v].predecessors[0], path, path_idx);
	}

	bool in_vector(int val, vector<int>& vec)
	{
		for (int v : vec)
			if (v == val)
				return true;
		
		return false;
	}

	// Dijkstra algorithm which can find multiple shortest paths.
	// Return true if there exists a path from source to destination,
	// and false otherwise.
	bool dijkstra(vector<vertex>& graph, int source_vertex, int dest_vertex, vector<vector<int>>& path)
	{
		PRINT_UI("Dijkstra called");

		int num_v = graph.size();     // number of vertices
		vector<spath> d(num_v);       // shortest paths to be updated after each step
		vector<bool> explored(num_v); // which vertices have already been explored?

		// Initialize d[vi]
		for (int i = 0; i < num_v; i++)
		{
			d[i].vertex_id = graph[i].vertex_id;

			if (d[i].vertex_id == source_vertex)
				d[i].min_dist = 0;
			else
				d[i].min_dist = INT_MAX;

			explored[i] = false;
		}

		for (int i = 0; i < num_v; i++)
		{
			int ui = find_min_idx(d, explored);
			if (d[ui].min_dist == INT_MAX)
				return false;

			explored[ui] = true;
			if (d[ui].vertex_id == dest_vertex)
				break;

			for (auto& av : graph[ui].adj_vertices)
			{
				int vi = find_vertex_idx(graph, av.vertex_id);
				int w = av.weight;
				if (!explored[vi])
				{
					if (d[vi].min_dist >= d[ui].min_dist + w)
					{
						if (d[vi].min_dist > d[ui].min_dist + w)
						{
							d[vi].min_dist = d[ui].min_dist + w;
							d[vi].predecessors.clear();
						}

						d[vi].predecessors.push_back(d[ui].vertex_id);
					}
				}
			}
		}

		//for (spath& sp : d)
		//{
		//	cout << sp.min_dist;
		//	for (int i : sp.predecessors)
		//		cout << " " << i;
		//	cout << endl;
		//}

		path.clear();
		path.push_back(vector<int>());
		get_spath(graph, d, dest_vertex, path, 0);

		for (auto& p : path)
			reverse(p.begin(), p.end());

		return true;
	}

	bool bellman_ford(Full_Graph_EL& graph, int source_vertex, int dest_vertex, vector<vector<int>>& path)
	{
		PRINT_UI("Bellman-Ford called");

		Graph_EL& graph_el = graph.first;
		int* weights = boost::get<int*>(graph.second[3]);
		int num_v = boost::get<int>(graph.second[4]);
		typename boost::graph_traits<Graph_EL>::vertex_descriptor u, v;

		vector<spath> d(num_v); // shortest paths to be updated after each step

		// Initialize d[i]
		for (int i = 0; i < num_v; i++)
			d[i].min_dist = INT_MAX;
		d[source_vertex].min_dist = 0;

		for (int i = 1; i <= num_v - 1; i++)
		{
			BGL_FORALL_EDGES_T(e, graph_el, Graph_EL)
			{
				u = source(e, graph_el);
				v = target(e, graph_el);

				if (d[u].min_dist < INT_MAX && d[v].min_dist >= d[u].min_dist + weights[e])
				{
					if (d[v].min_dist > d[u].min_dist + weights[e])
					{
						d[v].min_dist = d[u].min_dist + weights[e];
						d[v].predecessors.clear();
					}

					if (!in_vector(u, d[v].predecessors))
						d[v].predecessors.push_back(u);
				}
			}
		}		

		// Check for a negative-weight cycle
		BGL_FORALL_EDGES_T(e, graph_el, Graph_EL)
		{
			u = source(e, graph_el);
			v = target(e, graph_el);

			if (d[u].min_dist < INT_MAX && d[v].min_dist > d[u].min_dist + weights[e])
				return false;
		}

		if (d[dest_vertex].min_dist == INT_MAX)
			return false;

		//for (spath& sp : d)
		//{
		//	cout << sp.min_dist;
		//	for (int i : sp.predecessors)
		//		cout << " " << i;
		//	cout << endl;
		//}

		path.clear();
		path.push_back(vector<int>());
		get_spath_bf(d, dest_vertex, path, 0);

		for (auto& p : path)
			reverse(p.begin(), p.end());

		return true;
	}

	STATUS run_sps_all_paths_el(Full_Graph_EL& graph, string name_from, string name_to, string& list)
	{
		string* v_names = boost::get<string*>(graph.second[0]);
		int num_v = boost::get<int>(graph.second[4]);

		int source_v = -1, dest_v = -1;
		for (int i = 0; i < num_v; i++)
		{
			if (source_v == -1 && v_names[i] == name_from)
				source_v = i;
			if (dest_v == -1 && v_names[i] == name_to)
				dest_v = i;
			if (source_v != -1 && dest_v != -1)
				break;
		}

		if (source_v == -1 || dest_v == -1)
		{
			PRINT_STATUS("Vertex '" + name_from + "' or vertex '"
				+ name_to + "' does not exist.");
			return FAIL;
		}

		vector<vector<int>> path;
		list = "";

		if (bellman_ford(graph, source_v, dest_v, path))
		{
			for (int i = 0; i < path.size(); i++)
			{
				list += "Path " + to_string(i + 1) + ": ";
				for (int v : path[i])
					list += v_names[v] + " ";
				list += "\n";
			}
		}

		return SUCCESS;
	}
}
// END GRAPH_EXTEND
