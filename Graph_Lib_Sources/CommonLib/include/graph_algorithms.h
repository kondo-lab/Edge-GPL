/* version 3 */
#pragma once

#include "socket.h"
#include "graph.h"
#include "graph_structure.h"
#include "cloud_server.h"
#include "metis.h"

#include <boost/graph/visitors.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/betweenness_centrality.hpp>
#include <boost/graph/bc_clustering.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

namespace mGraphHelper {
	// BFS / DFS visitor
	template <class Tag, typename VertexMap, typename EdgeMap, typename NameMap>
	struct visitor_graph : public base_visitor<visitor_graph<Tag, VertexMap, EdgeMap, NameMap>>
	{
		typedef Tag event_filter;
		visitor_graph(VertexMap v_map, EdgeMap e_map, NameMap n_map, int format)
			: m_vertexmap(v_map), m_edgemap(e_map), m_namemap(n_map), m_format(format) {}
		template <class Edge, class Graph>
		void operator() (Edge edge, Graph &graph)
		{
			// get vertex and edge
			string v_from = m_namemap[source(edge, graph)],
			       v_to = m_namemap[target(edge, graph)];
			string e_index = "index=" + std::to_string(graph[edge].index),
			       e_name = "name=\"" + graph[edge].name + "\"",
			       e_weight = "weight=" + std::to_string(graph[edge].weight);
			// save vertex and edge
			put(m_vertexmap, vi++, v_to);
			switch (m_format) {
			case ADJACENCY_LIST:
			case ADJACENCY_MATRIX:
			case COMPRESSED_SPARSE_ROW:
				put(m_edgemap, ei++, v_from + "->" + v_to
					+ " [" + e_index + ", " + e_name + ", " + e_weight + "]");
				break;
			case ADJACENCY_LIST_UNDIRECTED:
            case ADJACENCY_MATRIX_UNDIRECTED:
				put(m_edgemap, ei++, v_from + "--" + v_to
					+ " [" + e_index + ", " + e_name + ", " + e_weight + "]");
				break;
			}
			#ifdef DEBUG_EXPORT_GRAPH_IMAGE
				const_cast<string&>(graph[target(edge, graph)].color) = "red";
				const_cast<string&>(graph[edge].color) = "red";
			#endif
		}
		VertexMap m_vertexmap;
		EdgeMap m_edgemap;
		NameMap m_namemap;
		int vi = 1,
		    ei = 0,
		    m_format = ADJACENCY_LIST;
	};
	template <class Tag, typename VertexMap, typename EdgeMap, typename NameMap>
	visitor_graph<Tag, VertexMap, EdgeMap, NameMap>
		record_vertices(VertexMap v_map, EdgeMap e_map, NameMap n_map, int format, Tag)
	{
		return visitor_graph<Tag, VertexMap, EdgeMap, NameMap>(v_map, e_map, n_map, format);
	}

	// Customized clustering condition
	class clustering_threshold : public bc_clustering_threshold<double>
	{
		typedef bc_clustering_threshold<double> inherited;

		public:
			template <typename Graph>
			clustering_threshold(double threshold, const Graph &graph, bool normalize)
			: inherited(threshold, graph, normalize) { }

			template <typename Graph>
			bool operator()(double max_centrality,
				typename boost::graph_traits<Graph>::edge_descriptor edge,
					const Graph &graph)
			{
				return inherited::operator()(max_centrality, edge, graph);
			}
	};
	template <typename Graph, typename Done, typename EdgeMap>
	void betweenness_centrality_clustering(Graph& graph, Done done, EdgeMap edge_centrality)
	{
		typedef typename property_traits<EdgeMap>::value_type centrality_type;
		typedef typename graph_traits<Graph>::edge_iterator edge_iterator;
		typedef typename graph_traits< Graph >::edge_descriptor edge_descriptor;

		bool no_edges = (num_edges(graph) == 0);
		if (no_edges)
			return;

		// Function object that compares the centrality of edges
		indirect_cmp<EdgeMap, std::less<centrality_type>> cmp(edge_centrality);

		boost::shared_array_property_map<double,
			typename boost::property_map<Graph, vertex_index_t>::const_type>
				centrality_map(
					num_vertices(graph),
					get(boost::vertex_index, graph)
				);
		bool is_done;
		do {
			brandes_betweenness_centrality(graph, centrality_map);
			std::pair<edge_iterator, edge_iterator> edges_iters = edges(graph);
			edge_descriptor edge = *max_element(
				edges_iters.first, edges_iters.second,
				cmp
			);
			is_done = done(get(get(&Edge::index, graph), edge), edge, graph);
			if (!is_done)
				remove_edge(edge, graph);
		} while (!is_done && !no_edges);
	}
}

// BEGIN GRAPH_EXTEND
namespace mGraphExtend
{
	// Adjacent vertex
	struct adj_vertex
	{
		int vertex_id;
		int weight; // Weight of the corresponding edge

		adj_vertex(int v = 0, int w = 0)
		{
			set_vals(v, w);
		}

		void set_vals(int v, int w)
		{
			vertex_id = v;
			weight = w;
		}
	};

	struct vertex
	{
		int vertex_id = 0;
		vector<adj_vertex> adj_vertices; // List of adjacent vertices
	};

	// Shortest path to a vertex
	struct spath
	{
		int vertex_id = 0;
		int min_dist = 0;         // Min distance from source vertex to this vertex
		vector<int> predecessors; // Vertices right before this vertex on shortest paths

		// For use with priority queue
		// (A more efficient implementation of Dijkstra uses prority queue)
		//bool operator>(const spath& sp2) const
		//{
		//    return (this->min_dist > sp2.min_dist);
		//}
	};

	int find_min_idx(vector<spath>& d, vector<bool>& explored);
	int find_vertex_idx(vector<vertex>& graph, int v);
	void get_spath(vector<vertex>& graph, vector<spath>& d, int v, vector<vector<int>>& path, int path_idx);
	void get_spath_bf(vector<spath>& d, int v, vector<vector<int>>& path, int path_idx);
	bool in_vector(int val, vector<int>& vec);
	bool dijkstra(vector<vertex>& graph, int source_vertex, int dest_vertex, vector<vector<int>>& path);
	bool bellman_ford(Full_Graph_EL& graph, int source_vertex, int dest_vertex, vector<vector<int>>& path);
	STATUS run_sps_all_paths_el(Full_Graph_EL& graph, string name_from, string name_to, string& list);
	
	template <typename Graph>
	STATUS run_sps_all_paths(Graph& graph, string name_from, string name_to, string& list)
	{
		// Get vertex descriptor of source and destination vertices
		typename graph_traits<Graph>::vertex_descriptor vertex_from, vertex_to;

		if (get_vertex_from_name(vertex_from, graph, name_from) == FAIL ||
			get_vertex_from_name(vertex_to, graph, name_to) == FAIL) {
			PRINT_STATUS("Vertex '" + name_from + "' or vertex '"
				+ name_to + "' does not exist.");
			return FAIL;
		}

		// Graph in the format the new Dijkstra algorithm needs
		vector<vertex> graphx(num_vertices(graph));
		vector<string> v_names(num_vertices(graph)); // Name of vertices

		// Index of vertices
		typename property_map<Graph, vertex_index_t>::type index = get(vertex_index, graph);

		typename graph_traits<Graph>::vertex_iterator vi_start, vi_end;
		typename graph_traits<Graph>::out_edge_iterator ei_start, ei_end;

		// Build graph in the format of new Dijkstra
		tie(vi_start, vi_end) = vertices(graph);
		for (int i = 0; vi_start != vi_end; ++vi_start, ++i)
		{
			graphx[i].vertex_id = index[*vi_start]; // vertex ID
			v_names[i] = graph[*vi_start].name;     // vertex name

			// Scan edges coming out of each vertex
			tie(ei_start, ei_end) = out_edges(*vi_start, graph);
			for (; ei_start != ei_end; ++ei_start)
			{
				// Get adjacent vertex and weight of the corresponding edge
				adj_vertex av(index[target(*ei_start, graph)], graph[*ei_start].weight);
				graphx[i].adj_vertices.push_back(av);
			}
		}

		int source_v = index[vertex_from];
		int dest_v = index[vertex_to];

		vector<vector<int>> path;
		list = "";

		if (dijkstra(graphx, source_v, dest_v, path))
		{
			vector<string> paths; // paths as list string
			for (int i = 0; i < path.size(); i++)
			{
				string str_path = "";
				for (int v : path[i])
					str_path += v_names[v] + " ";
				paths.push_back(str_path);
			}
			// ref: https://stackoverflow.com/a/24477023
			sort( paths.begin(), paths.end() ); // sort paths
			paths.erase( unique( paths.begin(), paths.end() ), paths.end() ); // remove duplicated paths
			// Add to result
			for (int i = 0; i < paths.size(); i++)
			{
				list += "Path " + to_string(i + 1) + ": " + paths[i] + "\n";
			}
		}

		return SUCCESS;
	}

	struct metis_data
	{
		vector<idx_t> xadj;
		vector<idx_t> adjncy;
		vector<idx_t> adjwgt;
		vector<int> e_direction;
		vector<string> v_names;
		vector<int> vwgt;
	};

	// gpe = graph partitioning based on edge cut.
	// Param "part" is a list of indices of vertices in a subgraph.
	template <typename Graph>
	void gpe_create_subgraph(metis_data& mdata, vector<idx_t>& part, Graph& subgraph)
	{
		//subgraph.clear();

		for (idx_t i : part)
		{
			mGraph::add_vertex(subgraph, mdata.v_names[i], SHOW_LOG_GRAPH_PARTITION);

			for (idx_t j = mdata.xadj[i]; j < mdata.xadj[i + 1]; j++)
			{
				int k = mdata.adjncy[j]; // k is an adjacent vertex
				mGraph::add_vertex(subgraph, mdata.v_names[k], SHOW_LOG_GRAPH_PARTITION);

				if (typeid(subgraph) == typeid(Graph_ALU))
				{
					if (!edge_exist(subgraph, mdata.v_names[k], mdata.v_names[i]))
						mGraph::add_edge(subgraph, mdata.v_names[i], mdata.v_names[k], mdata.adjwgt[j], SHOW_LOG_GRAPH_PARTITION);
				}
				else if (mdata.e_direction[j] == 0) // i->k
					mGraph::add_edge(subgraph, mdata.v_names[i], mdata.v_names[k], mdata.adjwgt[j], SHOW_LOG_GRAPH_PARTITION);
				else // k->i
					mGraph::add_edge(subgraph, mdata.v_names[k], mdata.v_names[i], mdata.adjwgt[j], SHOW_LOG_GRAPH_PARTITION);
			}
		}
	}

	// gpv = graph partitioning based on vertex cut.
	// Param "part" is the result of graph partitioning returned by METIS.
	// Param "sglist" is the list of subgraphs to be created.
	template <typename Graph>
	void gpv_create_subgraphs(Graph& graph, idx_t* part, vector<Graph>& sglist)
	{
        typename property_map<Graph, vertex_index_t>::type index = get(vertex_index, graph);
        srand(time(NULL));

        BGL_FORALL_EDGES_T(e, graph, Graph)
        {
            int sgi;
            int si = index[source(e, graph)];
            int ti = index[target(e, graph)];

            if (part[si] == part[ti] || rand() < RAND_MAX / 2.0)
                sgi = part[si];
            else
                sgi = part[ti];

            mGraph::add_vertex(sglist[sgi], graph[si].name, SHOW_LOG_GRAPH_PARTITION);
            mGraph::add_vertex(sglist[sgi], graph[ti].name, SHOW_LOG_GRAPH_PARTITION);
            mGraph::add_edge(sglist[sgi], graph[si].name, graph[ti].name, graph[e].weight, SHOW_LOG_GRAPH_PARTITION);
        }
	}

	template <typename Graph>
	void create_metis_data(Graph& graph, metis_data& mdata)
	{
		mdata.xadj.clear();
		mdata.adjncy.clear();
		mdata.adjwgt.clear();
		mdata.e_direction.clear();
		mdata.v_names.clear();
		mdata.vwgt.clear();
		
		typename property_map<Graph, vertex_index_t>::type index = get(vertex_index, graph);
		typename graph_traits<Graph>::vertex_descriptor v;
		typename graph_traits<Graph>::edge_descriptor e;
		int degree;

		BGL_FORALL_VERTICES_T(u, graph, Graph)
		{
			mdata.xadj.push_back(mdata.adjncy.size());
			mdata.v_names.push_back(graph[u].name);

			degree = 0;

			BGL_FORALL_OUTEDGES_T(u, e, graph, Graph)
			{
				v = target(e, graph); // e = u->v
				mdata.adjncy.push_back(index[v]);
				mdata.adjwgt.push_back(graph[e].weight);
				mdata.e_direction.push_back(0);
				degree++;
			}

			if (typeid(graph) == typeid(Graph_AL))
			{
				BGL_FORALL_VERTICES_T(v, graph, Graph)
				{
					bool e_exist;
					tie(e, e_exist) = edge(v, u, graph); // e = v->u
					if (e_exist)
					{
						mdata.adjncy.push_back(index[v]);
						mdata.adjwgt.push_back(graph[e].weight);
						mdata.e_direction.push_back(1);
						degree++;
					}
				}
			}

			mdata.vwgt.push_back(degree);
		}

		mdata.xadj.push_back(mdata.adjncy.size());

		//cout << "mdata.xadj:" << endl;
		//for (auto i : mdata.xadj)
		//	cout << i << " ";
		//cout << endl;

		//cout << "mdata.adjncy:" << endl;
		//for (auto i : mdata.adjncy)
		//	cout << i << " ";
		//cout << endl;

		//cout << "mdata.adjwgt:" << endl;
		//for (auto i : mdata.adjwgt)
		//	cout << i << " ";
		//cout << endl;

		//cout << "mdata.e_direction:" << endl;
		//for (auto i : mdata.e_direction)
		//	cout << i << " ";
		//cout << endl;

		//cout << "mdata.v_names:" << endl;
		//for (auto& s : mdata.v_names)
		//	cout << s << " ";
		//cout << endl;

		//cout << "mdata.vwgt:" << endl;
		//for (auto w : mdata.vwgt)
		//	cout << w << " ";
		//cout << endl;
	}

	/*	
	Function: Partitions a graph into a number of subgraphs using the METIS
	    library (http://glaros.dtc.umn.edu/gkhome/metis/metis/overview).
	Input: A graph, number of partitions, and prefix of filenames for storing
	    subgraphs on disk.
	Output: SUCCESS if the algorithm runs successfully; FAIL otherwise.	
	*/
	template <typename Graph>
	STATUS gpart_edge_cut(Graph& graph, int n_parts, string filename_prefix)
	{
		if (num_vertices(graph) == 0)
		{
			PRINT_STATUS("Graph is empty.");
			return FAIL;
		}

		idx_t options[METIS_NOPTIONS];
		METIS_SetDefaultOptions(options);
		options[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_CUT;
		options[METIS_OPTION_NUMBERING] = 0;
		
		metis_data mdata;
		create_metis_data(graph, mdata);

		idx_t nvtxs = num_vertices(graph);
		idx_t ncon = 1;
		idx_t nparts = n_parts;
		idx_t objval;
		idx_t part[nvtxs];

		// METIS_PartGraphKway: Higher load imbalance.
		// METIS_PartGraphRecursive: Lower load imbalance => can avoid empty subgraphs
		// when they are too small (i.e., containing a few vertices).
		if (METIS_PartGraphRecursive(&nvtxs, &ncon, mdata.xadj.data(), mdata.adjncy.data(),
			NULL, NULL, mdata.adjwgt.data(), &nparts, NULL,
			NULL, options, &objval, part) != METIS_OK)
		{
			PRINT_STATUS("METIS error.");
			return FAIL;
		}

		//for (auto i : part)
		//	cout << i << " ";
		//cout << endl;
		//cout << "objval = " << objval << endl;

		vector<vector<idx_t>> part_list(nparts);
		for (idx_t i = 0; i < nvtxs; i++)
			part_list[part[i]].push_back(i);

		for (int i = 0; i < nparts; i++)
		{
			Graph subgraph(0);
			string filename = filename_prefix + to_string(i + 1) + EXT_GRAPHVIZ;
			gpe_create_subgraph(mdata, part_list[i], subgraph);

			if (mGraph::export_graph(subgraph, filename, false, false, false) == FAIL)
			{
				PRINT_STATUS("Failed to write a subgraph to disk.");
				return FAIL;
			}
		}

		for (int i = 0; i < nparts; i++)
		{
			string msg = "Subgraph " + to_string(i + 1) + ":";
			for (auto j : part_list[i])
				msg += " " + graph[j].name;
			PRINT_STATUS(msg);
		}

		PRINT_STATUS("Edge-cut = " + to_string(objval));

		return SUCCESS;
	}

	/*
	Function: Partitions a graph into a number of subgraphs using the METIS
		library (http://glaros.dtc.umn.edu/gkhome/metis/metis/overview).
	Input: A graph, number of partitions, and prefix of filenames for storing
		subgraphs on disk.
	Output: SUCCESS if the algorithm runs successfully; FAIL otherwise.
	*/
	template <typename Graph>
	STATUS gpart_vertex_cut(Graph& graph, int n_parts, string filename_prefix)
	{
		if (num_vertices(graph) == 0)
		{
			PRINT_STATUS("Graph is empty.");
			return FAIL;
		}

		idx_t options[METIS_NOPTIONS];
		METIS_SetDefaultOptions(options);
		options[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_CUT;
		options[METIS_OPTION_NUMBERING] = 0;

		metis_data mdata;
		create_metis_data(graph, mdata);

		idx_t nvtxs = num_vertices(graph);
		idx_t ncon = 1;
		idx_t nparts = n_parts;
		idx_t objval;
		idx_t part[nvtxs];

		// METIS_PartGraphKway: Higher load imbalance.
		// METIS_PartGraphRecursive: Lower load imbalance => can avoid empty subgraphs
		// when they are too small (i.e., containing a few vertices).
		if (METIS_PartGraphRecursive(&nvtxs, &ncon, mdata.xadj.data(), mdata.adjncy.data(),
			mdata.vwgt.data(), NULL, mdata.adjwgt.data(), &nparts, NULL,
			NULL, options, &objval, part) != METIS_OK)
		{
			PRINT_STATUS("METIS error.");
			return FAIL;
		}

		//for (auto i : part)
		//	cout << i << " ";
		//cout << endl;
		//cout << "objval = " << objval << endl;

		vector<Graph> sglist(nparts);
		gpv_create_subgraphs(graph, part, sglist);

		for (int i = 0; i < nparts; i++)
		{
			string filename = filename_prefix + to_string(i + 1) + EXT_GRAPHVIZ;
			if (mGraph::export_graph(sglist[i], filename, false, false, false) == FAIL)
			{
				PRINT_STATUS("Failed to write a subgraph to disk.");
				return FAIL;
			}
		}

		for (int i = 0; i < nparts; i++)
		{
			string msg = "Subgraph " + to_string(i + 1) + ":";
            BGL_FORALL_VERTICES_T(u, sglist[i], Graph)
				msg += " " + sglist[i][u].name;
			PRINT_STATUS(msg);
		}

		int sumv = 0;
		for (auto& sg : sglist)
			sumv += num_vertices(sg);
		char rf[10];
		sprintf(rf, "%.2f", (double)sumv / nvtxs);
		PRINT_STATUS("Replication factor = " + string(rf));

		return SUCCESS;
	}

	template <typename Graph>
	STATUS run_gpart(Graph& graph, int n_parts, int cut_type, string filename_prefix)
	{
		#ifdef DEBUG_TIME
			// Mark the start time for measurement
			auto start = high_resolution_clock::now();
		#endif

		STATUS stt;
		if (cut_type == 1)
			stt = gpart_edge_cut(graph, n_parts, filename_prefix);
		else if (cut_type == 2)
			stt = gpart_vertex_cut(graph, n_parts, filename_prefix);
		else
		{
			PRINT_STATUS("Invalid graph-cut type.")
			stt = FAIL;
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
		if (stt == SUCCESS)
		{
			mCloudServer::sg_fnames.clear();
			mCloudServer::sg_fnames.resize(n_parts);
			for (int i = 0; i < n_parts; i++)
				mCloudServer::sg_fnames[i] = filename_prefix + to_string(i + 1) + EXT_GRAPHVIZ;
		}

		return stt;
	}
}
// END GRAPH_EXTEND

namespace mGraph {
	// Generic functions
	/*
		Name: run_bfs
		Function: Applies the Breadth-First Search algorithm on the graph with
			the supplied vertex as root vertex.
		Input: Vertex name.
		Output: SUCCESS if the algorithm runs successfully; FAIL otherwise.
		Return: List of traversed vertices in the order they were traversed.
	*/
	template <typename Graph>
	STATUS run_bfs(Graph &graph, string name, int format, string &v_list, string &e_list)
	{
		typename boost::graph_traits<Graph>::vertex_descriptor vertex;

		// 1. Validate input
		if (get_vertex_from_name(vertex, graph, name) == FAIL) {
			PRINT_STATUS("Vertex " + name + " does not exist.");
			return FAIL;
		}

		#ifdef DEBUG_EXPORT_GRAPH_IMAGE
			graph[vertex].color = "blue";
		#endif

		// 2. Run algortithm
		vector<string> vertexlist(num_vertices(graph));
		vector<string> edgelist(num_edges(graph));
		vertexlist[0] = name; // The first vertex visited is the
				      // source vertex
		#ifdef DEBUG_TIME
			// Mark the start time for measurement
			auto start = high_resolution_clock::now();
		#endif
		boost::breadth_first_search(graph, vertex,
			visitor(make_bfs_visitor(
				mGraphHelper::record_vertices(
					vertexlist.data(),
					edgelist.data(),
					get(&Node::name, graph),
					format,
					on_tree_edge{}
				)
			))
		);
		#ifdef DEBUG_TIME
			// Mark the end time for measurement
			auto stop = high_resolution_clock::now();
			// Get thread runtime duration
			auto duration = duration_cast<microseconds>(stop - start);
			std::cout << "[INFO] Algorithm runtime = "
				<< duration.count() << "[µs]"
				<< endl;
		#endif

		// 3. Remove empty strings
		vertexlist.erase(
			remove_if(
				vertexlist.begin(), vertexlist.end(),
				[](string name) {return name == "";}
			),
			vertexlist.end()
		);
		edgelist.erase(
			remove_if(
				edgelist.begin(), edgelist.end(),
				[](string name) {return name == "";}
			),
			edgelist.end()
		);

		// 4. Copy final result to data to be sent
		for (int i = 0; i < vertexlist.size(); i++)
			v_list += vertexlist[i] + "\n";
		for (int i = 0; i < edgelist.size(); i++)
			e_list += edgelist[i] + "\n";

		return SUCCESS;
	}
	/*
		Name: run_dfs
		Function: Applies the Depth-First Search algorithm on the graph with
			the supplied vertex as root vertex. Only traverses the
			connected component containing the root vertex.
		Input: Vertex name.
		Output: SUCCESS if the algorithm runs successfully; FAIL otherwise.
		Return: List of traversed vertices in the order they were traversed.
	*/
	template <typename Graph>
	STATUS run_dfs(Graph &graph, string name, int format, string &v_list, string &e_list)
	{
		typename boost::graph_traits<Graph>::vertex_descriptor vertex;

		// 1. Validate input
		if (get_vertex_from_name(vertex, graph, name) == FAIL) {
			PRINT_STATUS("Vertex " + name + " does not exist.");
			return FAIL;
		}

		#ifdef DEBUG_EXPORT_GRAPH_IMAGE
			graph[vertex].color = "blue";
		#endif

		// 2. Run algorithm
		vector<boost::default_color_type> color_map(num_vertices(graph));
		vector<string> vertexlist(num_vertices(graph));
		vector<string> edgelist(num_edges(graph));
		vertexlist[0] = name; // The first vertex visited is the
				      // source vertex
		#ifdef DEBUG_TIME
			// Mark the start time for measurement
			auto start = high_resolution_clock::now();
		#endif
		boost::depth_first_visit(graph, vertex,
			make_dfs_visitor(
				mGraphHelper::record_vertices(
					vertexlist.data(),
					edgelist.data(),
					get(&Node::name, graph),
					format,
					on_tree_edge{}
				)
			),
			make_iterator_property_map(
				color_map.begin(),
				get(boost::vertex_index, graph),
				color_map[0]
			)
		);
		#ifdef DEBUG_TIME
			// Mark the end time for measurement
			auto stop = high_resolution_clock::now();
			// Get thread runtime duration
			auto duration = duration_cast<microseconds>(stop - start);
			std::cout << "[INFO] Algorithm runtime = "
				<< duration.count() << "[µs]"
				<< endl;
		#endif

		// 3. Remove empty strings
		vertexlist.erase(
			remove_if(
				vertexlist.begin(), vertexlist.end(),
				[](string name) {return name == "";}
			),
			vertexlist.end()
		);
		edgelist.erase(
			remove_if(
				edgelist.begin(), edgelist.end(),
				[](string name) {return name == "";}
			),
			edgelist.end()
		);

		// 4. Copy final result to data to be sent
		for (int i = 0; i < vertexlist.size(); i++)
			v_list += vertexlist[i] + "\n";
		for (int i = 0; i < edgelist.size(); i++)
			e_list += edgelist[i] + "\n";

		return SUCCESS;
	}
	/*
		Name: run_sps
		Function: Applies the Dijkstra's Shortest Paths algorithm on the graph
			with the supplied vertices and starting and ending points.
		Input: Source vertex name and target vertex name.
		Output: SUCCESS if the algorithm runs successfully; FAIL otherwise.
		Return: List of traversed vertices in the order they were traversed.
	*/
	template <typename Graph>
	STATUS run_sps(Graph &graph, string name_from, string name_to, string &list)
	{
		typename boost::graph_traits<Graph>::vertex_descriptor vertex_from, vertex_to;

		// 1. Validate input
		if (get_vertex_from_name(vertex_from, graph, name_from) == FAIL ||
				get_vertex_from_name(vertex_to, graph, name_to) == FAIL) {
			PRINT_STATUS("Vertex '" + name_from + "' or vertex '"
				+ name_to +"' does not exist.");
			return FAIL;
		}

		// 2. Run algortithm
		vector<int> predecessors(num_vertices(graph));
		#ifdef DEBUG_TIME
			// Mark the start time for measurement
			auto start = high_resolution_clock::now();
		#endif
		boost::dijkstra_shortest_paths(graph, vertex_from,
			weight_map(get(&Edge::weight, graph))
			.predecessor_map(predecessors.data())
		);
		#ifdef DEBUG_TIME
			// Mark the end time for measurement
			auto stop = high_resolution_clock::now();
			// Get thread runtime duration
			auto duration = duration_cast<microseconds>(stop - start);
			std::cout << "[INFO] Algorithm runtime = "
				<< duration.count() << "[µs]"
				<< endl;
		#endif

		// 3. Save path as a list of vertex names
		typename boost::property_map<Graph, string Node::*>::type
			vertexnames = get(&Node::name, graph);
		vector<string> vertexlist;
		int vertex = vertex_to;
		while (vertex != predecessors[vertex]) {
			vertexlist.insert(
				vertexlist.begin(),
				vertexnames[vertex]
			);
			vertex = predecessors[vertex];
		}
		if (vertex == vertex_from) {
			vertexlist.insert(
				vertexlist.begin(),
				vertexnames[vertex]
			);

			#ifdef DEBUG_EXPORT_GRAPH_IMAGE
				graph[vertex_from].color = "blue";
			#endif

			// Copy final result to data to be sent
			list += vertexlist[0] + "\n";
			for (int i = 1; i < vertexlist.size(); i++) {
				list += vertexlist[i] + "\n";

				#ifdef DEBUG_EXPORT_GRAPH_IMAGE
				get_vertex_from_name(vertex_from, graph, vertexlist[i - 1]);
				get_vertex_from_name(vertex_to, graph, vertexlist[i]);
				graph[vertex_to].color = "red";
				std::pair<
					typename boost::graph_traits<Graph>::edge_descriptor,
					bool
				> ed = boost::edge(vertex_from, vertex_to, graph);
				if (ed.second == true)
					graph[ed.first].color = "red";
				#endif
			}

			#ifdef DEBUG_EXPORT_GRAPH_IMAGE
				graph[vertex_to].color = "blue";
			#endif
		}
		return SUCCESS;
	}
	/*
		Name: centrality_analysis
		Function: Applies the Brandes Betweenness Centrality algorithm on
			the graph to find the centrality of each vertex.
		Input: none
		Output: SUCCESS if the algorithm runs successfully; FAIL otherwise.
		Return: List containing the names of the vertices with the highest
			centrality.
	*/
	template <typename Graph>
	STATUS centrality_analysis(Graph &graph, string &list)
	{
		boost::shared_array_property_map<double,
			typename boost::property_map<Graph, vertex_index_t>::const_type>
				centrality_map(
					num_vertices(graph),
					get(boost::vertex_index, graph)
				);
		typename boost::property_map<Graph, string Node::*>::type
			vertexnames = get(&Node::name, graph);

		// 1. Run algorithm
		#ifdef DEBUG_TIME
			// Mark the start time for measurement
			auto start = high_resolution_clock::now();
		#endif
		boost::brandes_betweenness_centrality(graph, centrality_map);
		#ifdef DEBUG_TIME
			// Mark the end time for measurement
			auto stop = high_resolution_clock::now();
			// Get thread runtime duration
			auto duration = duration_cast<microseconds>(stop - start);
			std::cout << "[INFO] Algorithm runtime = "
				<< duration.count() << "[µs]"
				<< endl;
		#endif

		// 2. Filter the vertices with maximum centrality
		vector<string> vertexlist;
		double max = 0;
		for (int i = 0; i < num_vertices(graph); i++) {
			if (centrality_map[i] > max) {
				max = centrality_map[i];
				vertexlist.clear();
				vertexlist.push_back(vertexnames[i]);
			} else if (centrality_map[i] == max) {
				vertexlist.push_back(vertexnames[i]);
			}
		}

		// 3. Copy final result to data to be sent
		for (int i = 0; i < vertexlist.size(); i++) {
			list += vertexlist[i] + "\n";

			#ifdef DEBUG_EXPORT_GRAPH_IMAGE
				typename boost::graph_traits<Graph>::vertex_descriptor vertex;
				get_vertex_from_name(vertex, graph, vertexlist[i]);
				graph[vertex].color = "red";
			#endif
		}

		return SUCCESS;
	}
	/*
		Name: clustering
		Function: Applies the Edge Betweenness Clustering algortithm on
			the graph. This algorithm stops when the maximum
			centrality in the graph is lower than or equal to the
			supplied threshold value.
		Input: Threshold value.
		Output: SUCCESS if the algorithm runs successfully;
			FAIL otherwise.
		Return: List of remaining edges' names
	*/
	template <typename Graph>
	STATUS clustering(Graph graph, double threshold, string &list)
	{
		try {
			// 1. Run algorithm
			vector<int> edge_centrality(num_edges(graph));
			#ifdef DEBUG_TIME
				// Mark the start time for measurement
				auto start = high_resolution_clock::now();
			#endif
			mGraphHelper::betweenness_centrality_clustering(
				graph,
				mGraphHelper::clustering_threshold(
					threshold, graph, false
				),
				make_iterator_property_map(
					edge_centrality.begin(),
					get(&Edge::index, graph)
				)
			);
			#ifdef DEBUG_TIME
				// Mark the end time for measurement
				auto stop = high_resolution_clock::now();
				// Get thread runtime duration
				auto duration = duration_cast<microseconds>(stop - start);
				std::cout << "[INFO] Algorithm runtime = "
					<< duration.count() << "[µs]"
					<< endl;
			#endif

			// 2. Make a list of the remaining edges' names
			BGL_FORALL_EDGES_T(edge_i, graph, Graph)
				list += graph[edge_i].name + "\n";

			return SUCCESS;
		} catch (...) {
			return FAIL;
		}
	}
	/*
		Name: run_data_concat
		Function: Lets the current server (server A) act as a client and send
			CMD_DFS to another server (server B) with the supplied vertex
			as root vertex as well as receive the returned data.
		Input: Root vertex name, server B info (ip address and ports).
		Output: SUCCESS if the algorithm runs successfully; FAIL otherwise.
		Return: List of DFS vertices.
	*/
	template <typename Graph>
	STATUS run_data_concat(Graph &graph, string vertexlist, string edgelist,
		int format, int &added_edges)
	{
		// 1. Parse input data
		string line;
		stringstream ss_vertices(vertexlist);
		stringstream ss_edges(edgelist);
		vector<string> v_vertices, v_edges;
		while(getline(ss_vertices, line, '\n'))
			v_vertices.push_back(line);
		while(getline(ss_edges, line, '\n'))
			v_edges.push_back(line);

		// 2. Validation
		typename boost::graph_traits<Graph>::vertex_descriptor v_descriptor;
		// 2.1 Validate vertices
		for (int i = 1; i < v_vertices.size(); i++)
			if (get_vertex_from_name(v_descriptor, graph, v_vertices[i]) == SUCCESS)
				return FAIL;
		// 2.2 Validate edges
		if ((format == ADJACENCY_LIST && v_edges[0].find("->") == std::string::npos)
			|| (format == ADJACENCY_LIST_UNDIRECTED && v_edges[0].find("--") == std::string::npos))
			return FAIL;

		// 3. Add returned vertices to current graph
		for (int i = 1; i < v_vertices.size(); i++) {
			v_descriptor = boost::add_vertex(graph);
			graph[v_descriptor].name = v_vertices[i];
		}

		// 4. Add returned edges to current graph
		int current_index = -1;
		BGL_FORALL_EDGES_T(edge_i, graph, Graph)
			if (graph[edge_i].index > current_index)
				current_index = graph[edge_i].index;
		regex reg_edge, reg_delimiter;
		switch (format) {
		case ADJACENCY_LIST:
			reg_delimiter.assign("(->)");
			reg_edge.assign("([a-zA-Z0-9_.-]+->[a-zA-Z0-9_.-]+)|(weight=[0-9]+)");
			break;
		case ADJACENCY_LIST_UNDIRECTED:
			reg_delimiter.assign("(--)");
			reg_edge.assign("([a-zA-Z0-9_.-]+--[a-zA-Z0-9_.-]+)|(weight=[0-9]+)");
			break;
		}
		for (int i = 0; i < v_edges.size(); i++) {
			sregex_iterator current(v_edges[i].begin(), v_edges[i].end(), reg_edge);
			sregex_iterator last;
			string source, target, weight;
			// 3.1 Extract edge information
			int index = 0;
			while (current != last) {
				smatch match = *current;
				string element = match.str();
				if (index == 0) {
					element = regex_replace(element, reg_delimiter, " ");
					stringstream stream(element);
					getline(stream, source, ' ');
					getline(stream, target, ' ');
					index = 1;
				} else {
					while (!(element.front() >= '0' && element.front() <= '9'))
						element.erase(element.begin());
					while (!(element.back() >= '0' && element.back() <= '9'))
						element.erase(element.end());
					weight = element;
					index = 0;
				}
				current++;
			}
			// 3.2 Add edge
			typename boost::graph_traits<Graph>::vertex_descriptor vertex_from, vertex_to;
			get_vertex_from_name(vertex_from, graph, source);
			get_vertex_from_name(vertex_to, graph, target);
			std::pair<
				typename boost::graph_traits<Graph>::edge_descriptor,
				bool
			> e_descriptor = boost::add_edge(vertex_from, vertex_to, graph);
			graph[e_descriptor.first].index = ++current_index;
			graph[e_descriptor.first].name = source + " - " + target;
			graph[e_descriptor.first].weight = std::stoi(weight);
			// 3.3 Increment edges counter if a new edge was added
			added_edges++;
		}

		return SUCCESS;
	}
	
    template <typename Graph>
	STATUS run_sps_all_paths(Graph &graph, string name_from, string name_to, string &list)
	{
		return mGraphExtend::run_sps_all_paths(graph, name_from, name_to, list);
	}
    // --------------------------------------------------------------------
}