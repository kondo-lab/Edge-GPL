/* version 3 */
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

using namespace std;

#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE 32

const unsigned char _KEY_AES_CLIENT[AES_KEY_SIZE] = {
    0xE7,0x1B,0xF2,0xF8,
    0x7B,0x74,0xB1,0x9E,
    0x15,0x7E,0x41,0xEE,
    0x9E,0x9B,0xCF,0x2B,
    0x34,0xE1,0xC0,0x15,
    0xD7,0xFD,0x69,0xEF,
    0x6F,0x6F,0x86,0xDC,
    0xEE,0x0A,0x1B,0xDC
};

const unsigned char _IV_AES_CLIENT[AES_KEY_SIZE] = {
    0x68,0xA9,0x5C,0x33,
    0xA0,0xA6,0xD8,0x63,
    0xEB,0xA3,0x58,0x0F,
    0x32,0xA1,0x73,0x99,
    0x11,0x3C,0xDE,0x05,
    0x4D,0xDD,0x1E,0x0C,
    0x2C,0x54,0x85,0x29,
    0xCB,0x52,0xC8,0xD8
};

// #define DEBUG_UI
#ifdef DEBUG_UI
    #define PRINT_UI(message) { cout << "[INFO] " << message << endl; }
#else
    #define PRINT_UI(message) { }
#endif

#define DEBUG_STATUS
#ifdef DEBUG_STATUS
    #define PRINT_STATUS(message) { cout << "[STATUS] " << message << endl; }
#else
    #define PRINT_STATUS(message) { }
#endif

#define ULONG unsigned int

#define ENCRYPTION_ONLINE

#ifndef SERVER
    enum STATUS {
        SUCCESS,
        FAIL
    };
    enum COMMAND {
        CMD_NULL = -1,
        CMD_EXIT,
        CMD_ADD_VERTEX,
        CMD_REMOVE_VERTEX,
        CMD_NUM_VERTEX,
        CMD_ADD_EDGE,
        CMD_REMOVE_EDGE,
        CMD_NUM_EDGE,
        CMD_EDGE_IN_VERTEX,
        CMD_EDGE_OUT_VERTEX,
        CMD_CLEAR_ALL_GRAPH,
        CMD_IMPORT_GRAPH,
        CMD_EXPORT_GRAPH,
        CMD_PRINT_GRAPH,
        CMD_LIST_GRAPH_FORMATS,
        CMD_CURRENT_FORMAT,
        CMD_CHANGE_GRAPH_FORMAT,
        CMD_BFS,
        CMD_DFS,
        CMD_SPS,
        CMD_CENTRALITY_ANALYSIS,
        CMD_CLUSTERING,
        CMD_DATA_CONCAT,
        CMD_QUIT_BACKEND,
        CMD_GET_STATUS,
        CMD_RESET,
        CMD_GRAPH_PARTITION,
        CMD_LOAD_SUB_GRAPH,
        CMD_SPS_ALL_PATH,
        CMD_SUBGRAPH_INFO,
        CMD_EVALUATE_PATH,
        CMD_REGISTER_PATH,
        CMD_REMOVE_PATH
    };
    struct socket_info {
        string host = "127.0.0.1";
        string host_backup = "";
        int command_port = 2021;
        int operation_port = 2022;
    };
#endif

namespace Client {
    // Structure functions
    STATUS add_vertex(socket_info *arg, string name);
    STATUS remove_vertex(socket_info *arg, string name);
    STATUS get_num_vertex(socket_info *arg, int *num_vertex = NULL);
    STATUS add_edge(socket_info *arg, string name_from, string name_to, int weight);
    STATUS remove_edge(socket_info *arg, string name_from, string name_to);
    STATUS get_num_edge(socket_info *arg, int *num_edge = NULL);
    STATUS get_edge_in_vertex(socket_info *arg, string name,
        vector<string> *sources = NULL, vector<int> *weights = NULL);
    STATUS get_edge_out_vertex(socket_info *arg, string name,
        vector<string> *targets = NULL, vector<int> *weights = NULL);
    STATUS clear_graph(socket_info *arg);
    STATUS import_graph(socket_info *arg, string filename, bool is_encryption);
    STATUS export_graph(socket_info *arg, string filename, bool is_encryption);
    STATUS print_graph(socket_info *arg, string *graph = NULL, bool is_print_graph = true);
    STATUS list_graph_formats(socket_info *arg, string *formats = NULL);
    STATUS get_current_format(socket_info *arg, string *format = NULL);
    STATUS change_graph_format(socket_info *arg, int format);
    STATUS load_sub_graph(socket_info *arg, string filename, string &str_graph, bool is_encryption);
    STATUS run_sps_all_path(socket_info *arg, string name_from, string name_to, string *list = NULL);
    STATUS run_graph_partition(socket_info* arg, int num_parts, int cut_type);
    STATUS get_subgraph_info(socket_info* arg, string global_path, string* subgraph_fname = NULL,
        string* source_vname = NULL, string* local_dest_vname = NULL);
    STATUS evaluate_path(socket_info* arg, string local_path, string sg_fname, STATUS* eval = NULL, int* overlap = NULL);
    STATUS register_path(socket_info* arg, string local_path, string sg_fname);
    STATUS remove_path(socket_info* arg);

    // Algorithms
    STATUS run_bfs(socket_info *arg, string name,
        string *vertexlist = NULL, string *edgelist = NULL);
    STATUS run_dfs(socket_info *arg, string name,
        string *vertexlist = NULL, string *edgelist = NULL);
    STATUS run_sps(socket_info *arg, string name_from, string name_to, string *list = NULL);
    STATUS clustering(socket_info *arg, double threshold, string *list = NULL);
    STATUS centrality_analysis(socket_info *arg, string *list = NULL);
    STATUS run_data_concat(socket_info *arg, string name);

    // System functions
    STATUS quit_backend(socket_info *arg);
    STATUS reset(socket_info *arg);
    STATUS get_status(socket_info *arg, int *cmd = NULL);
    STATUS exit_client(socket_info *arg);

    // Socket
    STATUS create_socket_client(int &new_socket, struct sockaddr_in &address,
        string host, int port);
    STATUS close_socket(int &socket);
    STATUS get_address(struct sockaddr_in &address, int ip_address, int port);
    string print_command(COMMAND cmd);
    STATUS socket_send(int &socket, string data);
    STATUS socket_recv(int &socket, string &data);
    STATUS send_result(socket_info *arg, int cmd, string result);
    unsigned char* encrypt(
        const unsigned char _KEY_SERVER[], 
        const unsigned char _IV_SERVER[], 
        unsigned char *msg_origin, 
        int len_msg_origin,
        int *len_msg_encrypt);
    unsigned char* decrypt(
        const unsigned char _KEY_SERVER[], 
        const unsigned char _IV_SERVER[], 
        unsigned char *msg_encrypt, 
        int len_msg_encrypt,
        int *len_msg_decrypt);
    STATUS init_openssl();
    STATUS free_openssl();
    #define AES_BLOCK_SIZE 16
    #define AES_KEY_SIZE 32
}

// Helpers
STATUS get_command(COMMAND &cmd);
STATUS get_vertex(string &vertex);
STATUS get_filename(string &filename);
STATUS get_hostname(string &hostname);
STATUS get_graph_format(int &format);
STATUS get_encryption(bool &is_encryption);
STATUS get_edge_weight(int &weight);
STATUS get_clustering_threshold(double &threshold);
STATUS get_num_parts(int& num_parts);
STATUS get_cut_type(int& cut_type);
STATUS get_global_path(string& global_path);
STATUS get_local_path(string& local_path);
STATUS get_sg_fname(string& sg_fname);