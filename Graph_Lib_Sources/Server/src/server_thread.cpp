/* version 3 */
#define G_THREAD
#define G_DATA
#include "server_thread.h"

void* t_command_server(void* arg)
{
    // 1. Extract socket information
    socket_info *socket_information = (socket_info*) arg;
    string host = socket_information->host;
    int command_port = socket_information->command_port,
        operation_port = socket_information->operation_port;

    // 2. Define used variables;
    int socket_command_server = SOCKET_NULL,   // Server command socket
        socket_command_client = SOCKET_NULL;   // Client command socket
    COMMAND cmd = CMD_NULL;        // Current command
    STATUS ack = SUCCESS;
    double clustering_threshold = 0; // Threshold for stopping clustering algorithm
    struct sockaddr_in address_command,   // Address config for sockets
               address_operation;
    pthread_t thread_operation_id, thread_reset_id; // Thread IDs
    pthread_arg data; // Operation thread argument
    data.sent.graph = new Graph_AL();

    // For testing graph partition
    //Graph_AL g_test;
    //mGraph::import_graph(g_test, "graph.gv", false);
    //data.sent.graph = &g_test;
    //int g_format = ADJACENCY_LIST;
    //data.sent.current_graph_format = &g_format;
    
    data.sent.current_graph_format = &g_graph_format;
    data.socket = *socket_information;

    // 3. Create server command socket
    if (create_socket_server(socket_command_server, address_command,
            host, command_port) == FAIL)
        return (void*)FAIL;

    while (true) {
        // 4. Accept incoming client command connection
        if (accept_socket(socket_command_server, socket_command_client,
                address_command) == FAIL)
            continue;
        data.socket.client_ip_addr = inet_ntoa(address_command.sin_addr);

        // 5. Receive command from client
        PRINT_UI("Listening for command...");
        if (recv(socket_command_client, &cmd, sizeof(cmd), 0) <= 0) {
            PRINT_STATUS("Could not receive command.");
            close_socket(socket_command_client);
            continue;
        }
        PRINT_UI("Command " + print_command(cmd) + " received.");
        data.sent.command = cmd;
        if (cmd != CMD_GET_STATUS) {
            // Track latest command != CMD_GET_STATUS
            data.sent.latest_command = cmd;
        }
        if (cmd == CMD_RESET) {
            data.sent.thread_id = thread_operation_id;
            pthread_create(&thread_reset_id, NULL,
                mGraphThreads::reset, &data);
            pthread_join(thread_reset_id, NULL);
            continue;
        } else if (cmd == CMD_QUIT_BACKEND) {
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::quit_backend, &data);
            pthread_join(thread_operation_id, NULL);
            break;
        }

        // 6. Create a new graph if necessary
        if (data.sent.graph.which() == 0) {
            data.sent.graph = new Graph_AL();
        }

        // 7. Create new server operation socket
        if (create_socket_server(
            data.sent.socket_operation_server,
            address_operation,
            host, operation_port, true
        ) == FAIL)
            return (void*)FAIL;

        // 8. Return ACK to client
        PRINT_UI("Returning ACK...");
        while (send(socket_command_client, &ack, sizeof(ack), 0) <= 0) {
            continue;
        }
        PRINT_UI("Returned ACK.");
        close_socket(socket_command_client);

        // 10. Accept an incoming client operation connection
        if (accept_socket(
            data.sent.socket_operation_server,
            data.sent.socket_operation_client,
            address_operation
        ) == FAIL)
            continue;
        close_socket(data.sent.socket_operation_server);

        if (cmd == CMD_GET_STATUS) {
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::get_status, &data);
            continue;
        } 

        // 10. Receive data from client and execute the appropriate function
        //     in a separate thread.
        //     Some operations may not require any client-supplied data
        PRINT_UI("Waiting for data...");
        socket_recv(data.sent.socket_operation_client, data.socket.host);
        switch (cmd) {
        case CMD_ADD_VERTEX:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_from) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::add_vertex, &data);
            break;
        case CMD_REMOVE_VERTEX:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_from) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::remove_vertex, &data);
            break;
        case CMD_NUM_VERTEX:
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::get_num_vertex, &data);
            break;
        case CMD_ADD_EDGE:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_from) == FAIL
            || socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_to) == FAIL
            || recv(
                data.sent.socket_operation_client,
                &data.sent.edge_weight,
                sizeof(data.sent.edge_weight), 0) <= 0)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::add_edge, &data);
            break;
        case CMD_REMOVE_EDGE:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_from) == FAIL
            || socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_to) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::remove_edge, &data);
            break;
        case CMD_NUM_EDGE:
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::get_num_edge, &data);
            break;
        case CMD_EDGE_IN_VERTEX:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_from) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::get_edge_in_vertex, &data);
            break;
        case CMD_EDGE_OUT_VERTEX:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_from) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::get_edge_out_vertex, &data);
            break;
        case CMD_CLEAR_ALL_GRAPH:
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::clear_graph, &data);
            pthread_join(thread_operation_id, NULL);
            data.sent.graph = false;
            break;
        case CMD_IMPORT_GRAPH:
            // 10.1 Receive data
            if (recv(
                data.sent.socket_operation_client,
                &data.sent.is_encryption_file,
                sizeof(data.sent.is_encryption_file), 0) <= 0)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.filename) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL, 
                mGraphThreads::import_graph, &data);
            pthread_join(thread_operation_id, NULL);
            break;
        case CMD_EXPORT_GRAPH:
            // 10.1 Receive data
            if (recv(
                data.sent.socket_operation_client,
                &data.sent.is_encryption_file,
                sizeof(data.sent.is_encryption_file), 0) <= 0)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.filename) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::export_graph, &data);
            break;
        case CMD_PRINT_GRAPH:
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::print_graph, &data);
            break;
        case CMD_LIST_GRAPH_FORMATS:
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::list_graph_formats, &data);
            break;
        case CMD_CURRENT_FORMAT:
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::get_current_format, &data);
            break;
        case CMD_CHANGE_GRAPH_FORMAT:
            // 10.1 Receive data
            if (recv(
                data.sent.socket_operation_client,
                &data.sent.new_graph_format,
                sizeof(data.sent.new_graph_format), 0) <= 0)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            // 10.2.1 Initialize new graph
            data.sent.graph_copy = data.sent.graph;
            if (data.sent.new_graph_format == EDGE_LIST) {
                switch (g_graph_format) {
                case ADJACENCY_LIST:
                    mGraphHelperEL::get_el_info(
                        *(boost::get<Graph_AL*>(data.sent.graph_copy)),
                        edges_el, vertices_el, index_el, names_el, weights_el,
                        n_vertices_el
                    );
                    break;
                case ADJACENCY_MATRIX:
                    mGraphHelperEL::get_el_info(
                        *(boost::get<Graph_AM*>(data.sent.graph_copy)),
                        edges_el, vertices_el, index_el, names_el, weights_el,
                        n_vertices_el
                    );
                    break;
                case COMPRESSED_SPARSE_ROW:
                    mGraphHelperEL::get_el_info(
                        *(boost::get<Graph_CSR*>(data.sent.graph_copy)),
                        edges_el, vertices_el, index_el, names_el, weights_el,
                        n_vertices_el
                    );
                    break;
                case ADJACENCY_LIST_UNDIRECTED:
                    mGraphHelperEL::get_el_info(
                        *(boost::get<Graph_ALU*>(data.sent.graph_copy)),
                        edges_el, vertices_el, index_el, names_el, weights_el,
                        n_vertices_el
                    );
                    break;
                case ADJACENCY_MATRIX_UNDIRECTED:
                    mGraphHelperEL::get_el_info(
                        *(boost::get<Graph_AMU*>(data.sent.graph_copy)),
                        edges_el, vertices_el, index_el, names_el, weights_el,
                        n_vertices_el
                    );
                    break;
                }
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
                // create graph
                data.sent.graph = new Full_Graph_EL(std::make_pair(graph, meta));
            } else {
                switch (data.sent.new_graph_format) {
                case ADJACENCY_LIST:
                    if (g_graph_format != ADJACENCY_LIST) {
                        data.sent.graph = new Graph_AL();
                    }
                    break;
                case ADJACENCY_MATRIX:
                    switch (g_graph_format) {
                    case ADJACENCY_LIST:
                        data.sent.graph = new Graph_AM(boost::num_vertices(
                            *(boost::get<Graph_AL*>(data.sent.graph_copy))
                        ));
                        break;
                    case COMPRESSED_SPARSE_ROW:
                        data.sent.graph = new Graph_AM(boost::num_vertices(
                            *(boost::get<Graph_CSR*>(data.sent.graph_copy))
                        ));
                        break;
                    case ADJACENCY_LIST_UNDIRECTED:
                        data.sent.graph = new Graph_AM(boost::num_vertices(
                            *(boost::get<Graph_ALU*>(data.sent.graph_copy))
                        ));
                        break;
                    case ADJACENCY_MATRIX_UNDIRECTED:
                        data.sent.graph = new Graph_AM(boost::num_vertices(
                            *(boost::get<Graph_AMU*>(data.sent.graph_copy))
                        ));
                        break;
                    case EDGE_LIST:
                        data.sent.graph = new Graph_AM(n_vertices_el);
                        break;
                    }
                    break;
                case COMPRESSED_SPARSE_ROW:
                    switch (g_graph_format) {
                    case ADJACENCY_LIST:
                        data.sent.graph = new Graph_CSR(
                            *(boost::get<Graph_AL*>(data.sent.graph_copy))
                        );
                        break;
                    case ADJACENCY_MATRIX:
                        data.sent.graph = new Graph_CSR(
                            *(boost::get<Graph_AM*>(data.sent.graph_copy))
                        );
                        break;
                    default:
                        break;
                    }
                    break;
                case ADJACENCY_LIST_UNDIRECTED:
                    if (g_graph_format != ADJACENCY_LIST_UNDIRECTED) {
                        data.sent.graph = new Graph_ALU();
                    }
                    break;
                case ADJACENCY_MATRIX_UNDIRECTED:
                    switch (g_graph_format) {
                    case ADJACENCY_LIST:
                        data.sent.graph = new Graph_AMU(boost::num_vertices(
                            *(boost::get<Graph_AL*>(data.sent.graph_copy))
                        ));
                        break;
                    case ADJACENCY_MATRIX:
                        data.sent.graph = new Graph_AMU(boost::num_vertices(
                            *(boost::get<Graph_AM*>(data.sent.graph_copy))
                        ));
                        break;
                    case COMPRESSED_SPARSE_ROW:
                        data.sent.graph = new Graph_AMU(boost::num_vertices(
                            *(boost::get<Graph_CSR*>(data.sent.graph_copy))
                        ));
                        break;
                    case ADJACENCY_LIST_UNDIRECTED:
                        data.sent.graph = new Graph_AMU(boost::num_vertices(
                            *(boost::get<Graph_ALU*>(data.sent.graph_copy))
                        ));
                        break;
                    case EDGE_LIST:
                        data.sent.graph = new Graph_AMU(n_vertices_el);
                        break;
                    }
                    break;
                }
                // Clear edge list data
                if (g_graph_format == EDGE_LIST) {
                    n_vertices_el = 0;
                    edges_el.clear();
                    vertices_el.clear();
                    index_el.clear();
                    names_el.clear();
                    weights_el.clear();
                }
            }
            // 10.2.2 Populate new graph
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::change_graph_format, &data);
            pthread_join(thread_operation_id, NULL);
            data.sent.graph_copy = false;
            break;
        case CMD_BFS:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_from) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::run_bfs, &data);
            break;
        case CMD_DFS:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_from) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                    mGraphThreads::run_dfs, &data);
            break;
        case CMD_SPS:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_from) == FAIL
            || socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_to) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::run_sps, &data);
            break;
        case CMD_CENTRALITY_ANALYSIS:
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::centrality_analysis, &data);
            break;
        case CMD_CLUSTERING:
            // 10.1 Receive data
            if (recv(data.sent.socket_operation_client,
                &data.sent.clustering_threshold,
                sizeof(data.sent.clustering_threshold), 0) <= 0)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::clustering, &data);
            break;
        case CMD_DATA_CONCAT:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_from))
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::run_data_concat, &data);
            break;
        case CMD_LOAD_SUB_GRAPH:
            // 10.1 Receive data
            if (recv(
                data.sent.socket_operation_client,
                &data.sent.is_encryption_file,
                sizeof(data.sent.is_encryption_file), 0) <= 0)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.filename) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::send_sub_graph, &data);
            break;
        case CMD_SPS_ALL_PATH:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_from) == FAIL
            || socket_recv(
                data.sent.socket_operation_client,
                data.sent.name_vertex_to) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::run_sps_all_paths, &data);
            break;

        case CMD_GRAPH_PARTITION:
            // 10.1 Receive data
            if (recv(
                data.sent.socket_operation_client,
                &data.sent.num_parts,
                sizeof(data.sent.num_parts), 0) <= 0
            || recv(
                    data.sent.socket_operation_client,
                    &data.sent.cut_type,
                    sizeof(data.sent.cut_type), 0) <= 0)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::run_graph_partition, &data);
            break;

        case CMD_SUBGRAPH_INFO:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.global_path) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::get_subgraph_info, &data);
            break;

        case CMD_EVALUATE_PATH:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.local_path) == FAIL
            ||  socket_recv(
                data.sent.socket_operation_client,
                data.sent.sg_fname) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::evaluate_path, &data);
            break;

        case CMD_REGISTER_PATH:
            // 10.1 Receive data
            if (socket_recv(
                data.sent.socket_operation_client,
                data.sent.local_path) == FAIL
            ||  socket_recv(
                data.sent.socket_operation_client,
                data.sent.sg_fname) == FAIL)
            {
                PRINT_STATUS("Failed to receive data.");
                break;
            }
            PRINT_UI("Received data.");
            // 10.2 Execute operation thread
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::register_path, &data);
            break;

        case CMD_REMOVE_PATH:
            PRINT_UI("Processing...");
            pthread_create(&thread_operation_id, NULL,
                mGraphThreads::remove_path, &data);
            break;
        }

        if (cmd != CMD_IMPORT_GRAPH
            && cmd != CMD_CHANGE_GRAPH_FORMAT
            && cmd != CMD_CLEAR_ALL_GRAPH)
        {
            pthread_detach(thread_operation_id);
        }
    }

    // 11. Close command socket
    close_socket(socket_command_server);
    PRINT_UI("Closed command socket.");

    return (void*)SUCCESS;
}