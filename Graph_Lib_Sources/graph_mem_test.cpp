/* version 3 */
/*
    Filename: graph_mem_test.cpp
    This program imports a graph from .gv file (e.i: tmp_graph.gv) and evaluates memory usage of the graph
    Input: type of graph e.g. adjacency list, coo or csr

    How to compile: $ make graphmem
    How to run: $ ./bin/graphmem -t <type> (e.i: 0, 1, 2, 3, 4, 5) -f <filename> (e.i: tmp_graph.gv)
*/
#include <bits/stdc++.h>

#include <boost/variant.hpp>
#include <boost/variant/variant.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/edge_list.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/graphviz.hpp>

using namespace std;
using namespace boost;

#define PRINT_LOG

#ifdef PRINT_LOG
#define PRINT_INFO(message) { cout << "Info: " << message << endl; }
#define PRINT_GRAPH_FILE(filename) { \
    ifstream gf(filename);           \
    gf.seekg(0, ios::beg);           \
    char ch;                         \
    while (gf.get(ch))               \
        cout << ch;                  \
    cout << endl;                    \
    gf.close();                      \
}
#else
#define PRINT_INFO(message) { }
#define PRINT_GRAPH_FILE(filename) { }
#endif

#define PRINT_ERROR(message) { cerr << "Error: " << message << endl; }

#define DYNAMIC_PROPERTY(graph, dp) {                                   \
    dp.property("node_id", get(&Node::name, graph));                    \
    dp.property("index", get(&Edge::index, graph));                     \
    dp.property("name", get(&Edge::name, graph));                       \
    dp.property("weight", get(&Edge::weight, graph)); }

#ifdef PRINT_LOG
#define PRINT_GRAPH(graph) {                                            \
    boost::dynamic_properties dp(ignore_other_properties);              \
    DYNAMIC_PROPERTY(graph, dp);                                        \
    write_graphviz_dp(std::cout, graph, dp, "node_id"); }
#else
#define PRINT_GRAPH(graph) { }
#endif

#define LOAD_GRAPH(graph, filename) {                                   \
    try {                                                               \
        boost::dynamic_properties dp(ignore_other_properties);          \
        ifstream graph_file(filename);                                  \
        DYNAMIC_PROPERTY(graph, dp);                                    \
        read_graphviz(graph_file, graph, dp, "node_id");                \
        graph_file.close();                                             \
    } catch (...) {                                                     \
        PRINT_GRAPH_FILE(filename);                                     \
        PRINT_ERROR("Failed to import graph from '" + filename + "'."); \
        return FAIL;                                                    \
    }                                                                   \
}

enum GRAPH_FORMAT {
    ADJACENCY_LIST              = 0,
    ADJACENCY_MATRIX            = 1,
    COMPRESSED_SPARSE_ROW       = 2,
    ADJACENCY_LIST_UNDIRECTED   = 3,
    ADJACENCY_MATRIX_UNDIRECTED = 4,
    EDGE_LIST                   = 5
};

enum STATUS {
    SUCCESS,
    FAIL
};

struct Node {
    string name = "";
};

struct Edge {
    int index = 0;
    string name = "";
    int weight = 0;
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

// Overload: Conversion to non-CSR graph types
template <typename CurrentGraph, typename NewGraph>
STATUS change_graph_format(CurrentGraph &current_graph, NewGraph &new_graph, int new_format)
{
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
        new_graph[edge.first].index  = current_graph[edge_i].index;
        new_graph[edge.first].name   = current_graph[edge_i].name;
        new_graph[edge.first].weight = current_graph[edge_i].weight;
    }
    return SUCCESS;
}
// Overload: Convertion to CSR
template <typename CurrentGraph, typename NewGraph>
STATUS change_graph_format(CurrentGraph &current_graph, NewGraph &new_graph)
{
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
        new_graph[edge.first].index  = current_graph[edge_i].index;
        new_graph[edge.first].name   = current_graph[edge_i].name;
        new_graph[edge.first].weight = current_graph[edge_i].weight;
    }
    return SUCCESS;
}

// Overload: Conversion from edge list
template <typename Graph>
STATUS change_graph_format(Graph &graph, std::vector<Edge_EL> &edges_el, std::vector<string> &vertices_el,
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

void print_memory()
{
    // for (size_t i = 0; i < 1; i++)
    {
        // std::this_thread::sleep_for(1000ms);
        system("echo The memory roughly occupied by this graph is: ");
        system("ps aux | grep graphmem | grep -wv grep | grep -wv sh | awk '{print $6 \" KB\"}'");
        cout << endl;
        // std::this_thread::sleep_for(500ms);
    }
}

int main(int argc, char* argv[]) {
    int format = ADJACENCY_LIST,
        argi = 1;
    string filename = "tmp_graph.gv";

    while(argi < argc)
    {
        if (string(argv[argi]).compare("-t") == SUCCESS
                && argi + 1 < argc) {
            try {
                format = stoi(string(argv[argi + 1]));
                argi += 2;
            } catch (...) {
                PRINT_ERROR("Invalid argument(s).");
                return FAIL;
            }
        } else if (string(argv[argi]).compare("-f") == SUCCESS
                && argi + 1 < argc) {
            try {
                filename = string(argv[argi + 1]);
                argi += 2;
            } catch (...) {
                PRINT_ERROR("Invalid argument(s).");
                return FAIL;
            }
        } else {
            PRINT_ERROR("Invalid argument(s).");
            return FAIL;
        }
    }

    PRINT_INFO("Filename = " << filename << ", format = " << format);

    switch (format)
    {
    case ADJACENCY_LIST:
        {
            Graph_AL graph;
            LOAD_GRAPH(graph, filename);
            PRINT_GRAPH(graph);
            print_memory();
        }
        break;
    case ADJACENCY_LIST_UNDIRECTED:
        {
            Graph_ALU graph;
            LOAD_GRAPH(graph, filename);
            PRINT_GRAPH(graph);
            print_memory();
        }
        break;
    case ADJACENCY_MATRIX:
        {
            // init ADJACENCY_LIST graph
            Graph_AL *graphtmp = new Graph_AL();
            LOAD_GRAPH(*graphtmp, filename);
            Graph_AM graph(boost::num_vertices(*graphtmp));
            change_graph_format(*graphtmp, graph, format);
            // free ADJACENCY_LIST graph
            graphtmp->clear();
            delete graphtmp;
            graphtmp = NULL;

            PRINT_GRAPH(graph);
            print_memory();
        }
        break;
    case ADJACENCY_MATRIX_UNDIRECTED:
        {
            // init ADJACENCY_LIST graph
            Graph_ALU *graphtmp = new Graph_ALU();
            LOAD_GRAPH(*graphtmp, filename);
            Graph_AMU graph(boost::num_vertices(*graphtmp));
            change_graph_format(*graphtmp, graph, format);
            // free ADJACENCY_LIST graph
            graphtmp->clear();
            delete graphtmp;
            graphtmp = NULL;

            PRINT_GRAPH(graph);
            print_memory();
        }
        break;
    case COMPRESSED_SPARSE_ROW:
        {
            Graph_AL *graphtmp = new Graph_AL();
            // init ADJACENCY_LIST graph
            LOAD_GRAPH(*graphtmp, filename);

            graph_traits <Graph_AL>::edge_iterator e, e_end;
            vector<pair<int, int>> the_edges;
            for (boost::tie(e, e_end) = edges(*graphtmp); e != e_end; ++e)
                the_edges.push_back(make_pair(
                    source(*e, *graphtmp),
                    target(*e, *graphtmp)
                ));

            Graph_CSR graph(
                boost::edges_are_unsorted, 
                the_edges.begin(), the_edges.end(),
                boost::num_vertices(*graphtmp));
            change_graph_format(*graphtmp, graph);

            the_edges.clear();
            // free ADJACENCY_LIST graph
            graphtmp->clear();
            delete graphtmp;
            graphtmp = NULL;

            PRINT_GRAPH(graph);
            print_memory();
        }
        break;
    case EDGE_LIST:
        {
            // init var
            int n_vertices_el = 0;
            vector<Edge_EL> edges_el;
            vector<string> vertices_el, names_el;
            vector<int> index_el, weights_el;

            // init ADJACENCY_LIST graph
            Graph_AL *graphtmp = new Graph_AL();
            LOAD_GRAPH(*graphtmp, filename);

            change_graph_format(
                *graphtmp,
                edges_el, vertices_el, index_el, names_el, weights_el,
                n_vertices_el
            );

            Graph_EL graph(
                edges_el.data(),
                edges_el.data() + edges_el.size()
            );
            Meta_EL meta;
            meta.push_back(vertices_el.data());
            meta.push_back(index_el.data());
            meta.push_back(names_el.data());
            meta.push_back(weights_el.data());
            meta.push_back(n_vertices_el);

            Full_Graph_EL* fullGraph = new Full_Graph_EL(std::make_pair(graph, meta));
            // free ADJACENCY_LIST graph
            graphtmp->clear();
            delete graphtmp;
            graphtmp = NULL;

            typename boost::graph_traits<Graph_EL>::vertex_descriptor v_source, v_target;
            string strgraph = "Number of vertices = " + std::to_string(n_vertices_el) + "\n";
            BGL_FORALL_EDGES_T(edge_i, graph, Graph_EL) {
                // extract edge endpoints
                v_source = source(edge_i, graph);
                v_target = target(edge_i, graph);
                // construct edge string
                string edge = "";
                edge += vertices_el[v_source] + " - " + vertices_el[v_target];
                edge += " [index=" + std::to_string(index_el[edge_i]) + ",";
                edge += " name=\"" + names_el[edge_i] + "\",";
                edge += " weight=" + std::to_string(weights_el[edge_i]) + "]";
                strgraph += edge + "\n";
            }
            // print graph
            PRINT_INFO(strgraph);
            print_memory();
        }
        break;
    default:
        PRINT_INFO("format = " << format << " invalid");
        break;
    };
    return SUCCESS;
}