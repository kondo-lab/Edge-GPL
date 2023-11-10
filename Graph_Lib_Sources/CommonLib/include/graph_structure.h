/* version 3 */
#pragma once

#include "socket.h"

#include <boost/property_map/property_map.hpp>
#include <boost/range/irange.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/filtered_graph.hpp>

#define EXT_GRAPHVIZ ".gv"

namespace mGraph {
    // Non-generic functions
    STATUS import_graph(string &graph, string name, int format, bool is_encryption);
    STATUS export_graph(string &graph, string name, bool is_encryption);
    STATUS list_graph_formats(string &formats);
    STATUS get_current_format(int &format, string &strformat);

    // Generic functions
    /*
        Name: add_vertex
        Function: Adds the supplied vertex to graph if it hasn't been added
            already.
        Input: Vertex name.
        Output/Return: SUCCESS if the vertex is added; FAIL otherwise.
    */
    template <typename Graph>
    STATUS add_vertex(Graph &graph, string name, bool is_print_log = true)
    {
        typename boost::graph_traits<Graph>::vertex_descriptor vertex;
        // 1. Verify if the vertex already exists
        if (get_vertex_from_name(vertex, graph, name) == SUCCESS) {
            if (is_print_log)
                PRINT_STATUS("Vertex '" + name + "' already exists.");
            return FAIL;
        }
        // 2. Add new vertex
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        vertex = boost::add_vertex(graph);    // Add vertex
        graph[vertex].name = name;        // Set vertex name    
        #ifdef DEBUG_TIME
        if (is_print_log)
        {
            // Mark the end time for measurement
            auto stop = high_resolution_clock::now();
            // Get thread runtime duration
            auto duration = duration_cast<microseconds>(stop - start);
            std::cout << "[INFO] Algorithm runtime = "
                << duration.count() << "[µs]"
                << endl;
        }
        #endif
        // 3. Finish
        if (is_print_log)
            PRINT_STATUS("Vertex '" + name + "' added.");
        return SUCCESS;
    }
    /*
        Name: remove_vertex
        Function: Removes the supplied vertex from graph if it is in the graph.
        Input: Vertex name.
        Output/Return: SUCCESS if the vertex is removed; FAIL otherwise.
    */
    template <typename Graph>
    STATUS remove_vertex(Graph &graph, string name)
    {
        // 1. Verify if the vertex exists
        typename boost::graph_traits<Graph>::vertex_descriptor vertex;
        if (get_vertex_from_name(vertex, graph, name) == FAIL) {
            PRINT_STATUS("Vertex '" + name + "' does not exist.");
            return FAIL;
        }
        // 2. Remove vertex
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        boost::clear_vertex(vertex, graph);    // Clear edges related to vertex
        boost::remove_vertex(vertex, graph);    // Remove vertex
        #ifdef DEBUG_TIME
            // Mark the end time for measurement
            auto stop = high_resolution_clock::now();
            // Get thread runtime duration
            auto duration = duration_cast<microseconds>(stop - start);
            std::cout << "[INFO] Algorithm runtime = "
                << duration.count() << "[µs]"
                << endl;
        #endif
        // 3. Finish
        PRINT_STATUS("Vertex '" + name + "' removed.");
        return SUCCESS;
    }
    /*
        Name: get_num_vertex
        Function: Returns the number of vertices in the graph.
        Input: none
        Output: SUCCESS
        Return: Number of vertices.
    */
    template <typename Graph>
    STATUS get_num_vertex(Graph &graph, int &num_vertex)
    {
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        num_vertex = boost::num_vertices(graph);    // Get number of vertices
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
    /*
        Name: add_edge
        Function: Adds the supplied edge to graph if it hasn't been added
            already. If the edge already exists, update the edge's weight
            to the supplied weight.
        Input: The names of the source and target vertices, and the edge weight.
        Output: SUCCESS if the edge is added; FAIL otherwise.
    */
    template <typename Graph>
    STATUS add_edge(Graph &graph, string name_from, string name_to, int weight, bool is_print_log = true)
    {
        // 1. Initialize variables
        string name_edge = name_from + " - " + name_to;
        typename boost::graph_traits<Graph>
            ::vertex_descriptor vertex_from, vertex_to;
        std::pair<
            typename boost::graph_traits<Graph>::edge_descriptor,
            bool
        > edge;
        int current_index = -1;
        // 2. Verify if both vertices exist
        if (get_vertex_from_name(vertex_from, graph, name_from) == FAIL||
                get_vertex_from_name(vertex_to, graph, name_to) == FAIL) {
            if (is_print_log)
                PRINT_STATUS("Vertex '" + name_from + "' or vertex '"
                    + name_to +"' does not exist.");
            return FAIL;
        }
        // 3. Verify if edge already exists and if it does, update only the edge weight
        if (get_edge_from_name(&edge, graph, name_edge) == SUCCESS) {
            graph[edge.first].weight = weight;
            return SUCCESS;
        }
        // 4. Add new edge
        BGL_FORALL_EDGES_T(edge, graph, Graph)    // Get the highest edge index value
            if (graph[edge].index > current_index)
                current_index = graph[edge].index;
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        edge = boost::add_edge(vertex_from, vertex_to, graph);    // Add edge
        #ifdef DEBUG_TIME
        if (is_print_log)
        {
            // Mark the end time for measurement
            auto stop = high_resolution_clock::now();
            // Get thread runtime duration
            auto duration = duration_cast<microseconds>(stop - start);
            std::cout << "[INFO] Algorithm runtime = "
                << duration.count() << "[µs]"
                << endl;
        }
        #endif
        graph[edge.first].index = current_index + 1;    // Set edge index
        graph[edge.first].name = name_edge;        // Set edge name
        graph[edge.first].weight = weight;        // Set edge weight
        // 5. Finish
        if (is_print_log)
            PRINT_STATUS("Edge (" + name_from + ", " + name_to + ", " + to_string(weight) + ") added.");
        return SUCCESS;
    }
    /*
        Name: remove_edge
        Function: Removes the supplied edge if said edge is present in the graph.
        Input: The names of the source and target vertices.
        Output/Return: SUCCESS if the edge is removed; FAIL otherwise.
    */
    template <typename Graph>
    STATUS remove_edge(Graph &graph, string name_from, string name_to)
    {
        // 1. Initialize variables
        string name_edge = name_from + " - " + name_to;
        typename boost::graph_traits<Graph>
            ::vertex_descriptor vertex_from, vertex_to;
        std::pair<
            typename boost::graph_traits<Graph>::edge_descriptor,
            bool
        > edge;
        // 2. Verify if both vertices exist
        if (get_vertex_from_name(vertex_from, graph, name_from) == FAIL ||
                get_vertex_from_name(vertex_to, graph, name_to) == FAIL) {
            PRINT_STATUS("Vertex '" + name_from + "' or vertex '"
                + name_to +"' does not exist.");
            return FAIL;
        }
        // 3. Verify if edge exists
        if (get_edge_from_name(&edge, graph, name_edge) == FAIL) {
            PRINT_STATUS("Edge (" + name_from + ", " + name_to + ") does not exist.");
            return FAIL;
        }
        // 4. Remove edge
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        boost::remove_edge(edge.first, graph);    // Remove edge
        #ifdef DEBUG_TIME
            // Mark the end time for measurement
            auto stop = high_resolution_clock::now();
            // Get thread runtime duration
            auto duration = duration_cast<microseconds>(stop - start);
            std::cout << "[INFO] Algorithm runtime = "
                << duration.count() << "[µs]"
                << endl;
        #endif
        // 5. Finish
        PRINT_STATUS("Edge (" + name_from + ", " + name_to + ") removed.");
        return SUCCESS;
    }
    /*
        Name: get_num_edge
        Function: Returns the number of edges in the graph.
        Input: none
        Output: SUCCESS
        Return: Number of edges.
    */
    template <typename Graph>
    STATUS get_num_edge(Graph &graph, int &num_edge)
    {
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        num_edge = boost::num_edges(graph);    // Get number of edges
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
    /*
        Name: import_graph
        Function: Reads the graph from the given file.
        Input: File name.
        Output/Return: SUCCESS if the graph is imported from file; FAIL otherwise.
    */
    // Overload: Read the file containing a graph into a graph object
    template <typename Graph>
    STATUS import_graph(Graph &graph, string name, bool is_encryption)
    {
        // 1. Initialize variables
        Graph new_graph;
        boost::dynamic_properties dp(ignore_other_properties);
        dp.property("node_id", get(&Node::name, new_graph));
        dp.property("index", get(&Edge::index, new_graph));
        dp.property("name", get(&Edge::name, new_graph));
        dp.property("weight", get(&Edge::weight, new_graph));
        // 2. Read graph
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        try {
#ifndef ENCRYPTION_OFFLINE
            is_encryption = false;
#endif
            string tmp_decrypt_name = "tmp_decrypt_file.gv";
            if (is_encryption)
                decrypt_file(name, tmp_decrypt_name);
            else
                tmp_decrypt_name = name;
            
            // 2.1 Read graph from file
            ifstream graph_file(tmp_decrypt_name);
            read_graphviz(graph_file, new_graph, dp, "node_id");
            graph_file.close();
            // 2.2 Copy graph to main graph variable
            graph.clear();
            boost::copy_graph(new_graph, graph);
            if (is_encryption)
                std::remove(tmp_decrypt_name.c_str());
        } catch (...) {
            PRINT_STATUS("Failed to import graph from '" + name + "'.");
            return FAIL;
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
        // 3. Finish
        PRINT_STATUS("Imported graph from '" + name + "'.");
        return SUCCESS;
    }
    // Overload: Read the string containing a graph into a graph object
    template <typename Graph>
    STATUS import_graph(Graph &graph, string *strgraph)
    {
        // 1. Initialize variables
        Graph new_graph;
        boost::dynamic_properties dp(ignore_other_properties);
        dp.property("node_id", get(&Node::name, new_graph));
        dp.property("index", get(&Edge::index, new_graph));
        dp.property("name", get(&Edge::name, new_graph));
        dp.property("weight", get(&Edge::weight, new_graph));
        // 2. Read graph
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        try {
            // 2.1 Read graph from string
            stringstream stream(*strgraph);
            read_graphviz(stream, new_graph, dp, "node_id");
            // 2.2 Copy graph to main graph variable
            graph.clear();
            boost::copy_graph(new_graph, graph);
        } catch (...) {
            PRINT_STATUS("Failed to import graph.");
            return FAIL;
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
        // 3. Finish
        PRINT_STATUS("Imported graph.");
        return SUCCESS;
    }
    /*
        Name: export_graph
        Function: Writes the current graph to a file.
        Input: Name of the file to be written.
        Output/Return: SUCCESS if the graph is exported; FAIL otherwise.
    */
    // Overload: Write the graph object to file with encrypt
    template <typename Graph>
    STATUS export_graph(Graph &graph, string name, bool msg = true, bool encrypt = true, bool isPrintDebugTime = true)
    {
        // 1. Initialize variables
        boost::dynamic_properties dp(ignore_other_properties);
        dp.property("node_id", get(&Node::name, graph));
        dp.property("index", get(&Edge::index, graph));
        dp.property("name", get(&Edge::name, graph));
        dp.property("weight", get(&Edge::weight, graph));
        // for export graph file (by dot command).
        //    ref: https://www.graphviz.org/download/
        //    e.g: $ dot -Tpng <dot file> -o <image file>
        #ifdef DEBUG_EXPORT_GRAPH_IMAGE
        dp.property("color", get(&Node::color, graph));
        dp.property("label", get(&Edge::weight, graph));
        dp.property("color", get(&Edge::color, graph));
        #endif
        // 2. Write graph to file
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        try {
            ofstream graph_file(name);
            write_graphviz_dp(graph_file, graph, dp, "node_id");
            graph_file.close();
#ifdef ENCRYPTION_OFFLINE
            if (encrypt)
                encrypt_file(name, name);
#endif
        } catch (...) {
            PRINT_STATUS("Failed to export graph to '" + name + "'.");
            return FAIL;
        }
        #ifdef DEBUG_TIME
            if (isPrintDebugTime)
            {
                // Mark the end time for measurement
                auto stop = high_resolution_clock::now();
                // Get thread runtime duration
                auto duration = duration_cast<microseconds>(stop - start);
                std::cout << "[INFO] Algorithm runtime = "
                    << duration.count() << "[µs]"
                    << endl;
            }
        #endif
        // 3. Finish
        if (msg)
            PRINT_STATUS("Exported graph to '" + name + "'.");
        return SUCCESS;
    }
    /*
        Name: print_graph
        Function: Exports the graph to a temporary file, readthat same file
            line by line and store each line in a vector. This vector
            will be sent line by line to client to form a complete graph.
        Input: none
        Output: SUCCESS if the graph is stored; FAIL otherwise.
        Return: The current graph in the form of a vector of strings.
    */
    template <typename Graph>
    STATUS print_graph(Graph &graph, string &strgraph)
    {
        // 1. Initialize variables
        boost::dynamic_properties dp(ignore_other_properties);
        dp.property("node_id", get(&Node::name, graph));
        dp.property("index", get(&Edge::index, graph));
        dp.property("name", get(&Edge::name, graph));
        dp.property("weight", get(&Edge::weight, graph));
        // 2. Write graph to string
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        try {
            stringstream stream;
            write_graphviz_dp(stream, graph, dp, "node_id");
            strgraph = stream.str();
        } catch (...) {
            PRINT_STATUS("Failed to print graph.");
            return FAIL;
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
        // 3. Finish
        PRINT_STATUS("Printed graph.");
        return SUCCESS;
    }
    /*
        Name: get_edge_in_vertex
        Function: Retrieves the in-edges of the supplied vertex.
        Input: Vertex name.
        Output: FAIL if the supplied vertex does not exist; SUCCESS otherwise.
        Return: Names and weights of in-edges.
    */
    template <typename Graph>
    STATUS get_edge_in_vertex(Graph &graph, string name,
        vector<string> &vertices, vector<int> &weights, bool unDirectedGraph = false)
    {
        // 1. Verify if vertex exists
        typename boost::graph_traits<Graph>::vertex_descriptor vertex;
        if (get_vertex_from_name(vertex, graph, name) == FAIL) {
            PRINT_STATUS("Vertex '" + name + "' does not exist.");
            return FAIL;
        }
        #ifdef DEBUG_EXPORT_GRAPH_IMAGE
            graph[vertex].color = "blue";
        #endif
        // 2. Get in-edges
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        BGL_FORALL_EDGES_T(edge_i, graph, Graph)
        {
            if (target(edge_i, graph) == vertex) {
                vertices.push_back(graph[source(edge_i, graph)].name);
                weights.push_back(graph[edge_i].weight);
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    graph[edge_i].color = "red";
                    graph[source(edge_i, graph)].color = "red";
                #endif
            }
            if (unDirectedGraph && source(edge_i, graph) == vertex) {
                vertices.push_back(graph[target(edge_i, graph)].name);
                weights.push_back(graph[edge_i].weight);
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    graph[edge_i].color = "red";
                    graph[target(edge_i, graph)].color = "red";
                #endif
            }
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
    /*
        Name: get_edge_out_vertex
        Function: Retrieves the out-edges of the supplied vertex.
        Input: Vertex name.
        Output: FAIL if the supplied vertex does not exist; SUCCESS otherwise.
        Return: Names and weights of out-edges.
    */
    template <typename Graph>
    STATUS get_edge_out_vertex(Graph &graph, string name,
        vector<string> &vertices, vector<int> &weights)
    {
        // 1. Verify if vertex exists
        typename boost::graph_traits<Graph>::vertex_descriptor vertex;
        if (get_vertex_from_name(vertex, graph, name) == FAIL) {
            PRINT_STATUS("Vertex '" + name + "' does not exist.");
            return FAIL;
        }
        #ifdef DEBUG_EXPORT_GRAPH_IMAGE
            graph[vertex].color = "blue";
        #endif
        // 2. Get out-edges
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        BGL_FORALL_OUTEDGES_T(vertex, edge_i, graph, Graph) {
            vertices.push_back(graph[target(edge_i, graph)].name);
            weights.push_back(graph[edge_i].weight);
            #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                graph[edge_i].color = "red";
                graph[target(edge_i, graph)].color = "red";
            #endif
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
    /*
        Name: clear_graph
        Function: Erases the graph.
        Input: none
        Output/Return: SUCCESS
    */
    template <typename Graph>
    STATUS clear_graph(Graph *graph)
    {
        delete graph;
        PRINT_STATUS("Cleared all graphs.");
        return SUCCESS;
    }
    /*
        Name: change_graph_format
        Function: Changes the current graph format to specified format.
        Input: Graph format to change into
        Output/Return: SUCCESS if the format is changed; FAIL otherwise.
    */
    // Overload: Conversion to non-CSR graph types
    template <typename CurrentGraph, typename NewGraph>
    STATUS change_graph_format(CurrentGraph &current_graph, NewGraph &new_graph, int new_format)
    {
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        // 1. Copy vertex data
        typename boost::graph_traits<NewGraph>::vertex_descriptor vertex;
        BGL_FORALL_VERTICES_T(vertex_i, current_graph, CurrentGraph) {
            switch (new_format) {
            case ADJACENCY_LIST:
            case ADJACENCY_LIST_UNDIRECTED:
                vertex = boost::add_vertex(new_graph);
                new_graph[vertex].name = current_graph[vertex_i].name;
                break;
            default:
                new_graph[vertex_i].name = current_graph[vertex_i].name;
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
        BGL_FORALL_EDGES_T(edge_i, current_graph, CurrentGraph) {
            // 2.1 Extract edge endpoints' information
            smatch matches;
            regex_search(current_graph[edge_i].name, matches, reg_name);
            string vname_source = matches.str(1),
                   vname_target = matches.str(3);
            // 2.2 Verify endpoint order
            v_source = source(edge_i, current_graph);
            v_target = target(edge_i, current_graph);
            if (new_graph[v_source].name != vname_source) {
                v_temp = v_source;
                v_source = v_target;
                v_target = v_temp;
            }
            // 2.3 Add edge to new graph
            edge = boost::add_edge(v_source, v_target, new_graph);
            new_graph[edge.first].index = current_graph[edge_i].index;
            new_graph[edge.first].name = current_graph[edge_i].name;
            new_graph[edge.first].weight = current_graph[edge_i].weight;
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
    // Overload: Convertion to CSR
    template <typename CurrentGraph, typename NewGraph>
    STATUS change_graph_format(CurrentGraph &current_graph, NewGraph &new_graph)
    {
        #ifdef DEBUG_TIME
            // Mark the start time for measurement
            auto start = high_resolution_clock::now();
        #endif
        // 1. Copy vertex data
        BGL_FORALL_VERTICES_T(vertex_i, current_graph, CurrentGraph) {
            new_graph[vertex_i].name = current_graph[vertex_i].name;
        }
        // 2. Copy edge data
        BGL_FORALL_EDGES_T(edge_i, current_graph, CurrentGraph) {
            std::pair<
                typename boost::graph_traits<NewGraph>::edge_descriptor,
                bool
            > edge = boost::edge(
                    source(edge_i, current_graph),
                    target(edge_i, current_graph),
                    new_graph
                );
            new_graph[edge.first].index = current_graph[edge_i].index;
            new_graph[edge.first].name = current_graph[edge_i].name;
            new_graph[edge.first].weight = current_graph[edge_i].weight;
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

    template <class Tag, typename VertexMapID, typename VertexMapName>
    struct visitor_graph_bfs : public base_visitor<visitor_graph_bfs<Tag, VertexMapID, VertexMapName>>
    {
        typedef Tag event_filter;
        visitor_graph_bfs(VertexMapID &v_map_id, VertexMapName &v_map_name) : v_map_id(&v_map_id), v_map_name(&v_map_name) {}

        template <class Edge, class Graph>
        void operator() (Edge edge, Graph &graph)
        {
            v_map_id->push_back(target(edge, graph));
            v_map_name->push_back(graph[target(edge, graph)].name);
        }

        VertexMapID   *v_map_id;
        VertexMapName *v_map_name;
    };

    template <class Tag, typename VertexMapID, typename VertexMapName>
    visitor_graph_bfs<Tag, VertexMapID, VertexMapName>
        record_bfs(VertexMapID &v_map_id, VertexMapName &v_map_name, Tag)
    {
        return visitor_graph_bfs<Tag, VertexMapID, VertexMapName>(v_map_id, v_map_name);
    }

    // Filter vertex in NameMap
    template <typename Graph, typename NameMap>
    struct filter_vexter {
        filter_vexter() { } // has to have a default constructor!
        // https://stackoverflow.com/questions/13829077/passing-a-boostfunction-to-a-template-what-class-is-boostfunction
        filter_vexter(const Graph& g, NameMap vName) : g(&g), vName(vName)
        {
            cout << "filter_vexter: ";
            for (int i = 0; i < vName.size(); i++)
                cout << vName[i] << ", ";
            cout << endl;
        }

        bool operator() (typename boost::graph_traits<Graph>::vertex_descriptor v) const
        {
            bool isKeep = false;
            string name = (*g)[v].name;
            for (int i = 0; i < vName.size(); i++)
                if (vName[i].compare(name) == 0)
                {
                    isKeep = true;
                    break;
                }
            return isKeep;
        }
        const Graph* g;
        NameMap vName;
    };

    template <typename Graph>
    STATUS copy_graph_to_undirected(Graph &graph, Graph_ALU &graph_undirected)
    {
        BGL_FORALL_VERTICES_T(v_i, graph, Graph) {
            auto v = boost::add_vertex(graph_undirected);
            graph_undirected[v].name = graph[v_i].name;
        }
        std::regex reg_name("([a-zA-Z0-9_.-]+)( - )([a-zA-Z0-9_.-]+)");
        BGL_FORALL_EDGES_T(e_i, graph, Graph) {
            // 1 Extract edge endpoints' information
            smatch matches;
            regex_search(graph[e_i].name, matches, reg_name);
            string vname_source = matches.str(1),
                vname_target = matches.str(3);
            // cout << graph[e_i].name << ", " << vname_source << ", " << vname_target << endl;
            // 2 Verify endpoint order
            auto v_source = source(e_i, graph);
            auto v_target = target(e_i, graph);
            if (graph_undirected[v_source].name != vname_source) {
                auto v_temp = v_source;
                v_source = v_target;
                v_target = v_temp;
            }
            // 3 Add edge to new graph
            auto edge = boost::add_edge(v_source, v_target, graph_undirected);
            graph_undirected[edge.first].index  = graph[e_i].index;
            graph_undirected[edge.first].name   = graph[e_i].name;
            graph_undirected[edge.first].weight = graph[e_i].weight;
        }
        return SUCCESS;
    }

    template <typename Graph>
    STATUS get_neighbour_vertex(Graph &graph, string v_name, vector<string> &listVerticesNeighbour_Name, Graph_ALU &graph_undirected)
    {
        listVerticesNeighbour_Name.clear();
        // 1. Tìm trên đồ thị các đỉnh là hàng xóm của đỉnh đang xét
        BGL_FORALL_EDGES_T(edge_i, graph, Graph) 
        {
            auto src_name  = graph[source(edge_i, graph)].name;
            auto dest_name = graph[target(edge_i, graph)].name;
            if (v_name.compare(dest_name) == 0) // nếu điểm đang xét là điểm cuối thì lấy điểm đầu của cạnh
                listVerticesNeighbour_Name.push_back(src_name);
            else if (v_name.compare(src_name) == 0) // nếu điểm đang xét là điểm đầu thì lấy điểm cuối của cạnh
                listVerticesNeighbour_Name.push_back(dest_name);
        }

        if (listVerticesNeighbour_Name.size() > 0)
        {
            // 2. Clone đồ thị thành đồ thị vô hướng
            copy_graph_to_undirected(graph, graph_undirected);
            // lấy id vertex trên đồ thị vô hướng
            typename boost::graph_traits<Graph>::vertex_descriptor vertex_i;
            get_vertex_from_name(vertex_i, graph_undirected, v_name);

            // 3. Xóa điểm cut trên đồ thị vô hướng
            boost::clear_vertex(vertex_i, graph_undirected);
            boost::remove_vertex(vertex_i, graph_undirected);
        }
        return SUCCESS;
    }

    template <typename Graph>
    STATUS get_neighbour_edge(Graph &graph, string v_name_from, string v_name_to, 
        vector<string> &listVerticesNeighbour_Name, Graph_ALU &graph_undirected)
    {
        listVerticesNeighbour_Name.clear();
        // 1. Tìm trên đồ thị các đỉnh vào ra cạnh đang xét
        std::pair<
            typename boost::graph_traits<Graph>::edge_descriptor,
            bool> edge1, edge2;
        string name_edge1 = v_name_from + " - " + v_name_to;
        string name_edge2 = v_name_to   + " - " + v_name_from;
        // Nếu cạnh không tồn tại thì return FAIL
        if (get_edge_from_name(&edge1, graph, name_edge1) == FAIL &&
            get_edge_from_name(&edge2, graph, name_edge2) == FAIL)
        {
            return FAIL;
        }

        // 2. Clone đồ thị thành đồ thị vô hướng
        copy_graph_to_undirected(graph, graph_undirected);

        std::pair<
            typename boost::graph_traits<Graph_ALU>::edge_descriptor,
            bool> edge;
        // 3. Xóa cạnh cut trên đồ thị vô hướng
        if (edge1.second)
        {
            get_edge_from_name(&edge, graph_undirected, name_edge1);
            boost::remove_edge(edge.first, graph_undirected);
        }
        if (edge2.second)
        {
            get_edge_from_name(&edge, graph_undirected, name_edge2);
            boost::remove_edge(edge.first, graph_undirected);
        }

        // 4. Add các đỉnh giữa cạnh vào list
        listVerticesNeighbour_Name.push_back(v_name_from);
        listVerticesNeighbour_Name.push_back(v_name_to);
        return SUCCESS;
    }

    template <typename Graph>
    STATUS sub_graph(Graph &graph, Graph_ALU &graph_undirected, vector<string> listVerticesNeighbour_Name, vector<string> &list_subgraph_file)
    {
        if (listVerticesNeighbour_Name.size() == 0)
            return SUCCESS;

        vector<int> listVerticesHistory_ID;
        // 3. Duyệt các đỉnh vào ra đỉnh cut
        for (int i = 0; i < listVerticesNeighbour_Name.size(); i++)
        {
            typename boost::graph_traits<Graph>::vertex_descriptor vertex_neightbour;
            // 3.1. Lấy ra id vertex của đỉnh đang xét trên đồ thị vô hướng
            get_vertex_from_name(vertex_neightbour, graph_undirected, listVerticesNeighbour_Name[i]);
            vector<int> listVerticesBFS_ID;
            vector<string> listVerticesBFS_Name;

            listVerticesBFS_Name.push_back(listVerticesNeighbour_Name[i]); // phải thêm điểm đang xét vào listVerticesBFS_Name
            listVerticesBFS_ID.push_back(vertex_neightbour);      // thêm điểm vertex đang xét vào mảng để lần sau không duyệt nữa
            listVerticesHistory_ID.push_back(vertex_neightbour); // thêm điểm vertex đang xét vào mảng để lần sau không duyệt nữa

            // 3.2. Tìm BFS điểm đang xét trên đồ thị vô hướng
            boost::breadth_first_search(graph_undirected, vertex_neightbour,
                visitor(make_bfs_visitor(
                    record_bfs(
                        listVerticesBFS_ID,
                        listVerticesBFS_Name,
                        on_tree_edge{}
                    )
                ))
            );
            
            // 3.3. Sort 2 mảng listVerticesHistory_ID và listVerticesBFS_ID
            sort(listVerticesHistory_ID.begin(), listVerticesHistory_ID.end());
            sort(listVerticesBFS_ID.begin(), listVerticesBFS_ID.end());
            vector<int> vCheckOverlap;
            set_intersection(
                listVerticesHistory_ID.begin(), listVerticesHistory_ID.end(),
                listVerticesBFS_ID.begin(), listVerticesBFS_ID.end(),
                back_inserter(vCheckOverlap));            

            // 3.4. Nếu 2 mảng listVerticesHistory_ID và listVerticesBFS_ID có điểm trùng nhau thì không làm gì, nếu có thì sẽ lấy đồ thị chứa các đỉnh BFS trên đồ thị gốc và lưu vào file
            if (vCheckOverlap.size() == 1)
            {
                // 3.5. copy graph gốc ban đầu sang subgraph, subgraph chứa kết quả duyệt BFS ở trên
                Graph g_subgraph; // Graph is Graph_AL or Graph_ALU
                copy_graph(
                    // https://www.boost.org/doc/libs/1_68_0/boost/graph/filtered_graph.hpp
                    make_filtered_graph(
                        graph,         // const Graph& g,
                        keep_all(), // EdgePredicate ep, 
                        filter_vexter<Graph, vector<string>>(graph, listVerticesBFS_Name) // VertexPredicate vp
                        ),
                    g_subgraph
                );
                // 3.6. Lưu subgraph ra file
                std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
                string filename = to_string(ms.count()) + "_subgraph_" + listVerticesNeighbour_Name[i] + EXT_GRAPHVIZ;
                list_subgraph_file.push_back(filename);

                // export graph to file (not encrypt)
                export_graph(g_subgraph, filename, false, false);
                g_subgraph.clear();
            }
        }
        return SUCCESS;
    }
    // --------------------------------------------------------------------
}

namespace mGraphHelper {
    template <typename Graph>
    STATUS export_image(Graph graph, string clustering_edge_list = "")
    {
        if (clustering_edge_list.length() > 0) {
            stringstream tokens(clustering_edge_list);
            string edgename = "";
            std::pair<
                typename boost::graph_traits<Graph>::edge_descriptor,
                bool
            > edge;
            while (getline(tokens, edgename, '\n')) {
                get_edge_from_name(&edge, graph, edgename);
                graph[edge.first].color = "red";
                graph[source(edge.first, graph)].color = "red";
                graph[source(edge.first, graph)].color = "red";
            }
        }
        mGraph::export_graph(graph, "graph_tmp.gv", false, false);
        auto end = std::chrono::system_clock::now();
        int n_vertex = boost::num_vertices(graph);
        int n_edge = boost::num_edges(graph);
        int status = system("dot -Tpng graph_tmp.gv -o graph_tmp.png");
                status = system("rm graph_tmp.gv");
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        tm local_tm = *localtime(&end_time);
        std::ofstream outfile;
        outfile.open("graph_log.txt", std::ios_base::app); // append instead of overwrite
        outfile << local_tm.tm_year + 1900 << "-"
            << local_tm.tm_mon + 1 << "-"
            << local_tm.tm_mday << " "
            << local_tm.tm_hour << ":"
            << local_tm.tm_min << ":"
            << local_tm.tm_sec << " "
            << "Exported graph to graph_tmp.png." << " "
            << "Graph info " << n_vertex << "," << n_edge << "\n";
        BGL_FORALL_VERTICES_T(vertex, graph, Graph)
            graph[vertex].color = "black";
        BGL_FORALL_EDGES_T(edge, graph, Graph)
            graph[edge].color = "black";
        return SUCCESS;
    }
}