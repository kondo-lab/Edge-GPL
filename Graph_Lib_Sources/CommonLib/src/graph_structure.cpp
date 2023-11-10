/* version 3 */
#include "../include/graph_structure.h"

namespace mGraph {
    // Overload: Read the graph from file into a string
    STATUS import_graph(string &graph, string name, int format, bool is_encryption)
    {
        string tmp_decrypt_name = "tmp_decrypt_file.gv";
        if (is_encryption)
            decrypt_file(name, tmp_decrypt_name);
        else
            tmp_decrypt_name = name;

        boost::dynamic_properties dp(ignore_other_properties);
        ifstream graph_file(tmp_decrypt_name);
        stringstream stream("");

        if (format == ADJACENCY_LIST)
        {
            try {
                Graph_AL graph;
                dp.property("node_id", get(&Node::name, graph));
                dp.property("index", get(&Edge::index, graph));
                dp.property("name", get(&Edge::name, graph));
                dp.property("weight", get(&Edge::weight, graph));
                read_graphviz(graph_file, graph, dp, "node_id");
                write_graphviz_dp(stream, graph, dp, "node_id");
            } catch (...) {
                PRINT_STATUS("Failed to read graph from '" + name + "'.");
                graph_file.close();
                return FAIL;
            }
        }
        else if (format == ADJACENCY_LIST_UNDIRECTED)
        {
            try {
                Graph_ALU graph;
                dp.property("node_id", get(&Node::name, graph));
                dp.property("index", get(&Edge::index, graph));
                dp.property("name", get(&Edge::name, graph));
                dp.property("weight", get(&Edge::weight, graph));
                read_graphviz(graph_file, graph, dp, "node_id");
                write_graphviz_dp(stream, graph, dp, "node_id");
            } catch (...) {
                PRINT_STATUS("Failed to read graph from '" + name + "'.");
                graph_file.close();
                return FAIL;
            }
        }
        graph_file.close();
        graph = stream.str();
        if (is_encryption)
            std::remove(tmp_decrypt_name.c_str());
        PRINT_STATUS("Read graph from '" + name + "'.");
        return SUCCESS;
    }
    // Overload: Write the string containing a graph to file
    STATUS export_graph(string &graph, string name, bool is_encryption)
    {
        try {
            std::ofstream out(name);
            out << graph;
            out.close();
            if (is_encryption)
                encrypt_file(name, name);
            PRINT_STATUS("Exported graph to '" + name + "'.");
            return SUCCESS;
        } catch (...) {
            PRINT_STATUS("Failed to export graph to '" + name + "'.");
            return FAIL;
        }
    }
    /*
        Name: list_graph_formats
        Function: Stores the possible graph formats ina list.
        Input: List container
        Output: SUCCESS
        Return: List of available formats
    */
    STATUS list_graph_formats(string &formats) {
        for (int i = ADJACENCY_LIST; i <= EDGE_LIST; i++) {
            switch (i) {
            case ADJACENCY_LIST:
                formats += "(" + to_string(i) + ") Adjacency list" + "\n";
                break;
            case ADJACENCY_MATRIX:
                formats += "(" + to_string(i) + ") Adjacency matrix" + "\n";
                break;
            case COMPRESSED_SPARSE_ROW:
                formats += "(" + to_string(i) + ") Compressed sparse row" + "\n";
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                formats += "(" + to_string(i) + ") Undirected adjacency list" + "\n";
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                formats += "(" + to_string(i) + ") Undirected adjacency matrix" + "\n";
                break;
            case EDGE_LIST:
                formats += "(" + to_string(i) + ") Edge list" + "\n";
                break;
            }
        }
        return SUCCESS;
    }
    /*
        Name: get_current_format
        Function: Converts the current format int value into string.
        Input: Current format in int from
        Output: SUCCESS
        Return: Current format in string form
    */
    STATUS get_current_format(int &format, string &strformat)
    {
        switch (format) {
        case ADJACENCY_LIST:
            strformat = "ADJACENCY_LIST";
            break;
        case ADJACENCY_MATRIX:
            strformat = "ADJACENCY_MATRIX";
            break;
        case COMPRESSED_SPARSE_ROW:
            strformat = "COMPRESSED_SPARSE_ROW";
            break;
        case ADJACENCY_LIST_UNDIRECTED:
            strformat = "ADJACENCY_LIST_UNDIRECTED";
            break;
        case ADJACENCY_MATRIX_UNDIRECTED:
            strformat = "ADJACENCY_MATRIX_UNDIRECTED";
            break;
        case EDGE_LIST:
            strformat = "EDGE_LIST";
            break;
        }
        return SUCCESS;
    }
}