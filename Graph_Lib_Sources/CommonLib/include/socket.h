/* version 3 */
#pragma once

#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include "graph.h"
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

using namespace std;

const int MAX_BUFFER_SIZE = 1024;
const int SOCKET_NULL = -1;

#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE 32

const unsigned char _KEY_AES_SERVER[AES_KEY_SIZE] = {
    0xE7,0x1B,0xF2,0xF8,
    0x7B,0x74,0xB1,0x9E,
    0x15,0x7E,0x41,0xEE,
    0x9E,0x9B,0xCF,0x2B,
    0x34,0xE1,0xC0,0x15,
    0xD7,0xFD,0x69,0xEF,
    0x6F,0x6F,0x86,0xDC,
    0xEE,0x0A,0x1B,0xDC
};
const unsigned char _IV_AES_SERVER[AES_KEY_SIZE] = {
    0x68,0xA9,0x5C,0x33,
    0xA0,0xA6,0xD8,0x63,
    0xEB,0xA3,0x58,0x0F,
    0x32,0xA1,0x73,0x99,
    0x11,0x3C,0xDE,0x05,
    0x4D,0xDD,0x1E,0x0C,
    0x2C,0x54,0x85,0x29,
    0xCB,0x52,0xC8,0xD8
};

#define DEBUG_UI
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

// #define DEBUG_TIME
#ifdef DEBUG_TIME
    #include <bits/stdc++.h>
    using namespace std::chrono; // for timing
#endif

#define ENCRYPTION_ONLINE
#define ENCRYPTION_OFFLINE
#define SHOW_LOG_GRAPH_PARTITION false

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

struct data_sent {
    GraphPtr graph = false,
             graph_copy = false;
    string name_vertex_from = "",
           name_vertex_to = "",
           filename = "",
           global_path = "",
           local_path = "",
           sg_fname = "";
    int edge_weight = 0;
    int num_parts = 0; // for graph partitioning
    int cut_type = 0;  // for graph partitioning
    int socket_operation_server = SOCKET_NULL,
        socket_operation_client = SOCKET_NULL;
    int command = CMD_NULL,
        latest_command = CMD_NULL;
    int new_graph_format = ADJACENCY_LIST;
    int *current_graph_format = NULL;
    double clustering_threshold = 0;
    bool is_encryption_file = false;
    pthread_t thread_id = -1;
};
struct data_processed {
    int command = CMD_NULL;
};
struct socket_info {
    string host = "0.0.0.0";
    string host_backup = "";
    int command_port = 2021;
    int operation_port = 2022;
    string client_ip_addr = "";
};
struct pthread_arg {
    data_sent sent;
    data_processed processed;
    socket_info socket;
};

STATUS get_address(struct sockaddr_in &address, string host, int port);
STATUS create_socket_server(int &new_socket, struct sockaddr_in &address,
    string host, int port, bool type_operation = false);
STATUS create_socket_client(int &new_socket, struct sockaddr_in &address,
    string host, int port);
STATUS accept_socket(int &server_socket, int &client_socket,
    struct sockaddr_in &address);
STATUS close_socket(int &socket);
string print_command(COMMAND cmd);
STATUS socket_send(int &socket, string data);
STATUS socket_recv(int &socket, string &data);
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
void encrypt_file(
    string filename_in, 
    string filename_out);
void decrypt_file(
    string filename_in, 
    string filename_out);
STATUS init_openssl();
STATUS free_openssl();

namespace mGraph {
    STATUS quit_backend(pthread_arg &arg);
    STATUS reset(pthread_arg &arg);
    STATUS get_status(pthread_arg &arg);
}