#include "../include/cloud_server.h"
#include "../include/graph.h"
#include "../include/graph_structure.h"
#include <string>
#include <vector>
#include <sstream>

using namespace std;

namespace mCloudServer
{
    vector<string> sg_fnames;
    std::unordered_map<string, path_info> dpath;
    int ovl_threshold = 5;
    
    // Split a path string into a list of vertex names
    // E.g., "A B C D" --> {"A", "B", "C", "D"}
    vector<string> get_vertices(const string& path)
    {
        vector<string> vertices;
        stringstream ss(path);
        string vertex;

        while (getline(ss, vertex, ' '))
        {
            if (vertex != "")
                vertices.push_back(vertex);
        }

        return vertices;
    }

    int compute_ovl(string path1, string path2)
    {
        vector<string> vertices1 = get_vertices(path1);
        vector<string> vertices2 = get_vertices(path2);
        int ovl = 0;

        // Count the number of vertices occurring in both paths
        for (auto& v1 : vertices1)
        {
            for (auto& v2 : vertices2)
                if (v1 == v2)
                    ovl++;
        }

        return ovl;
    }

    int compute_ovl_max(string path, string sg_fname)
    {
        int max = 0;
        for (auto iter = dpath.begin(); iter != dpath.end(); ++iter)
        {
            if (iter->second.sg_fname == sg_fname)
            {
                int ovl = compute_ovl(path, iter->second.path);
                if (ovl > max)
                    max = ovl;
            }
        }

        return max;
    }

    template <typename Graph>
    void numv_in_subgraph(
        Graph subgraph,
        string path,
        int& numv,            // Number of vertices inside the subgraph
        string& source_v,     // Source vertex
        string& local_dest_v) // Local destination vertex
    {
        vector<string> vertices = get_vertices(path);
        
        numv = 0;
        for (string vname : vertices)
            if (vertex_exist(subgraph, vname))
                numv++;
            else
                break;

        source_v = " ";
        local_dest_v = " ";
        if (numv > 0)
        {
            source_v = vertices[0];
            local_dest_v = vertices[numv - 1];
        }
    }

    STATUS get_subgraph_info(
        string path,
        int graph_format,     // graph format
        string& sg_fname,     // Subgraph filename
        string& source_v,     // Source vertex
        string& local_dest_v) // Local destination vertex
    {
        STATUS stt = FAIL;
        
        sg_fname = " ";
        source_v = " ";
        local_dest_v = " ";

        int nv_max = 0;

        for (string fn : mCloudServer::sg_fnames)
        {
            int nv = 0;
            string sv, ldv;
            if (graph_format == ADJACENCY_LIST)
            {
                Graph_AL sg;
                stt = mGraph::import_graph(sg, fn, false);
                numv_in_subgraph(sg, path, nv, sv, ldv);
            }
            else if (graph_format == ADJACENCY_LIST_UNDIRECTED)
            {
                Graph_ALU sg;
                stt = mGraph::import_graph(sg, fn, false);
                numv_in_subgraph(sg, path, nv, sv, ldv);
            }
            if (nv > nv_max)
            {
                nv_max = nv;
                sg_fname = fn;
                source_v = sv;
                local_dest_v = ldv;
            }
        }

        return stt;
    }

    STATUS evaluate_path(string path, string sg_fname, STATUS& eval, int& overlap)
    {
        overlap = compute_ovl_max(path, sg_fname);
        eval = (overlap > ovl_threshold ? FAIL : SUCCESS);

        PRINT_UI("Determined Paths: ");
        for (auto iter = dpath.begin(); iter != dpath.end(); ++iter)
        {
            PRINT_UI(iter->first << ", " << iter->second.path << ", " << iter->second.sg_fname);
        }

        return SUCCESS;
    }

    STATUS register_path(string ip_addr, string path, string sg_fname)
    {
        path_info pi;
        pi.path = path;
        pi.sg_fname = sg_fname;
        dpath[ip_addr] = pi;

        PRINT_UI("Determined Paths: ");
        for (auto iter = dpath.begin(); iter != dpath.end(); ++iter)
        {
            PRINT_UI(iter->first << ", " << iter->second.path << ", " << iter->second.sg_fname);
        }

        return SUCCESS;
    }

    STATUS remove_path(string ip_addr)
    {
        dpath.erase(ip_addr);

        PRINT_UI("Determined Paths: ");
        for (auto iter = dpath.begin(); iter != dpath.end(); ++iter)
        {
            PRINT_UI(iter->first << ", " << iter->second.path << ", " << iter->second.sg_fname);
        }

        return SUCCESS;
    }
}