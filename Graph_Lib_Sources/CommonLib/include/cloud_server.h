#pragma once

#include "graph.h"
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

namespace mCloudServer
{
    struct path_info
    {
        string path;
        string sg_fname; // Filename of the subgraph containing the path
    };

    // List of names of files containing the subgraphs
    extern vector<string> sg_fnames;

    // Determined paths
    // Key: IP address of an edge device (e.g., "192.168.1.2")
    // Value: The determined path (e.g., "A B C D") and the corresponding subgraph
    extern std::unordered_map<string, path_info> dpath;

    // Overlap threshold
    extern int ovl_threshold;

    // Compute overlap between two paths
    int compute_ovl(string path1, string path2);

    // Compute the maximum of overlaps between this path and others
    int compute_ovl_max(string path, string sg_fname);

    // Compute the number of consecutive vertices (from the source) on a path
    // which are contained in a subgraph.
    template <typename Graph>
    void numv_in_subgraph(
        Graph subgraph,
        string path,
        int& numv,             // Number of vertices inside the subgraph
        string& source_v,      // Source vertex
        string& local_dest_v); // Local destination vertex

    // Get the name of the subgraph containing the source vertex of a given path
    STATUS get_subgraph_info(
        string  path,
        int graph_format,      // graph format
        string& sg_fname,      // Subgraph filename
        string& source_v,      // Source vertex
        string& local_dest_v); // Local destination vertex

    STATUS evaluate_path(string path, string sg_fname, STATUS& eval, int& overlap);
    STATUS register_path(string ip_addr, string path, string sg_fname);
    STATUS remove_path(string ip_addr);
}
