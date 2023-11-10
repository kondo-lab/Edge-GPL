/* version 3 */
#define SERVER
#include "../include/graph_thread.h"
#include "../../Client/src_cpp/module.h"
#include "../include/cloud_server.h"

namespace mGraphThreads {
    /*
        All of below are thread functions that are to be called from the
        server command thread.
        All of them calls their respective graph functions and return the
        proccessed data to the client.
    */

    void* add_vertex(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        STATUS status_operation = FAIL;

        if (data.socket.host.size() > 0) {
            status_operation = Client::add_vertex(
                &data.socket,
                data.sent.name_vertex_from
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::add_vertex(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    data.sent.name_vertex_from
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::add_vertex(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    data.sent.name_vertex_from
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            default:
                PRINT_STATUS("Cannot add vertex to the current graph format.");
                break;
            }
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        PRINT_UI("Returned data.");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);

        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* remove_vertex(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        STATUS status_operation = FAIL;

        if (data.socket.host.size() > 0) {
            status_operation = Client::remove_vertex(
                &data.socket,
                data.sent.name_vertex_from
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::remove_vertex(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    data.sent.name_vertex_from
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::remove_vertex(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    data.sent.name_vertex_from
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            default:
                PRINT_STATUS("Cannot remove vertex from the current graph format.");
                break;
            }
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        PRINT_UI("Returned data.");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* get_num_vertex(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        STATUS status_operation = FAIL;

        int num_vertex = 0;
        if (data.socket.host.size() > 0) {
            status_operation = Client::get_num_vertex(
                &data.socket,
                &num_vertex
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::get_num_vertex(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    num_vertex
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::get_num_vertex(
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    num_vertex
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AM*>(data.sent.graph)));
                #endif
                break;
            case COMPRESSED_SPARSE_ROW:
                status_operation = mGraph::get_num_vertex(
                    *(boost::get<Graph_CSR*>(data.sent.graph)),
                    num_vertex
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_CSR*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::get_num_vertex(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    num_vertex
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::get_num_vertex(
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    num_vertex
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AMU*>(data.sent.graph)));
                #endif
                break;
            case EDGE_LIST:
                status_operation = mGraphEL::get_num_vertex(
                    *(boost::get<Full_Graph_EL*>(data.sent.graph)),
                    num_vertex
                );
                break;
            default:
                PRINT_STATUS("Cannot get the number of vertices from the current graph format.");
                break;
            }
        }

        if (send(socket, &num_vertex, sizeof(num_vertex), 0) < 0) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        PRINT_UI("Returned data.");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* add_edge(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        STATUS status_operation = FAIL;

        if (data.socket.host.size() > 0) {
            status_operation = Client::add_edge(
                &data.socket,
                data.sent.name_vertex_from,
                data.sent.name_vertex_to,
                data.sent.edge_weight
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::add_edge(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    data.sent.edge_weight
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::add_edge(
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    data.sent.edge_weight
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AM*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::add_edge(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    data.sent.edge_weight
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::add_edge(
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    data.sent.edge_weight
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AMU*>(data.sent.graph)));
                #endif
                break;
            default:
                PRINT_STATUS("Cannot add edge to the current graph format.");
                break;
            }
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        PRINT_UI("Returned data.");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* remove_edge(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        STATUS status_operation = FAIL;

        if (data.socket.host.size() > 0) {
            status_operation = Client::remove_edge(
                &data.socket,
                data.sent.name_vertex_from,
                data.sent.name_vertex_to
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::remove_edge(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::remove_edge(
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AM*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::remove_edge(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::remove_edge(
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AMU*>(data.sent.graph)));
                #endif
                break;
            default:
                PRINT_STATUS("Cannot remove edge from the current graph format.");
                break;
            }
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        PRINT_UI("Returned data.");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* get_num_edge(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        STATUS status_operation = FAIL;

        int num_edge = 0;
        if (data.socket.host.size() > 0) {
            status_operation = Client::get_num_edge(
                &data.socket,
                &num_edge
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::get_num_edge(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    num_edge
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::get_num_edge(
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    num_edge
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AM*>(data.sent.graph)));
                #endif
                break;
            case COMPRESSED_SPARSE_ROW:
                status_operation = mGraph::get_num_edge(
                    *(boost::get<Graph_CSR*>(data.sent.graph)),
                    num_edge
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_CSR*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::get_num_edge(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    num_edge
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::get_num_edge(
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    num_edge
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AMU*>(data.sent.graph)));
                #endif
                break;
            case EDGE_LIST:
                status_operation = mGraph::get_num_edge(
                    (boost::get<Full_Graph_EL*>(data.sent.graph))->first,
                    num_edge
                );
                break;
            default:
                PRINT_STATUS("Cannot get the number of edges from the current graph format.");
                break;
            }
        }

        if (send(socket, &num_edge, sizeof(num_edge), 0) < 0) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        PRINT_UI("Returned data.");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* get_edge_in_vertex(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        STATUS status_operation = FAIL;
        bool sent = true;

        vector<string> source_names;
        vector<int> edge_weights;
        if (data.socket.host.size() > 0) {
            status_operation = Client::get_edge_in_vertex(
                &data.socket,
                data.sent.name_vertex_from,
                &source_names, &edge_weights
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::get_edge_in_vertex(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    source_names, edge_weights
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::get_edge_in_vertex(
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    source_names, edge_weights
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AM*>(data.sent.graph)));
                #endif
                break;
            case COMPRESSED_SPARSE_ROW:
                status_operation = mGraph::get_edge_in_vertex(
                    *(boost::get<Graph_CSR*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    source_names, edge_weights
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_CSR*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::get_edge_in_vertex(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    source_names, edge_weights, true
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::get_edge_in_vertex(
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    source_names, edge_weights, true
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AMU*>(data.sent.graph)));
                #endif
                break;
            case EDGE_LIST:
                status_operation = mGraphEL::get_edge_in_vertex(
                    *(boost::get<Full_Graph_EL*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    source_names, edge_weights
                );
                break;
            default:
                PRINT_STATUS("Cannot get in-edges of the vertex from the current graph format.");
                break;
            }
        }
        int datasize = source_names.size();

        // Send STATUS of algorithm (success/fail)
        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;
        // Send size of vertex list
        if (send(socket, &datasize, sizeof(datasize), 0) < 0)
            sent = false;
        // Send vertex names and edge weights one pair ata time
        for (int i = 0; i < datasize; i++) {
            if (socket_send(socket, source_names[i]) == FAIL)
                sent = false;
            if (send(socket, &edge_weights[i], sizeof(edge_weights[i]), 0) < 0)
                sent = false;
        }
        if (sent) {
            PRINT_UI("Returned all data packets.");
        } else {
            PRINT_UI("One or more data packets have been lost.");
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* get_edge_out_vertex(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        STATUS status_operation = FAIL;
        bool sent = true;

        vector<string> target_names;
        vector<int> edge_weights;
        if (data.socket.host.size() > 0) {
            status_operation = Client::get_edge_out_vertex(
                &data.socket,
                data.sent.name_vertex_from,
                &target_names, &edge_weights
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::get_edge_out_vertex(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    target_names, edge_weights
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::get_edge_out_vertex(
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    target_names, edge_weights
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AM*>(data.sent.graph)));
                #endif
                break;
            case COMPRESSED_SPARSE_ROW:
                status_operation = mGraph::get_edge_out_vertex(
                    *(boost::get<Graph_CSR*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    target_names, edge_weights
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_CSR*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::get_edge_out_vertex(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    target_names, edge_weights
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::get_edge_out_vertex(
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    target_names, edge_weights
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AMU*>(data.sent.graph)));
                #endif
                break;
            case EDGE_LIST:
                status_operation = mGraphEL::get_edge_out_vertex(
                    *(boost::get<Full_Graph_EL*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    target_names, edge_weights
                );
                break;
            default:
                PRINT_STATUS("Cannot get out-edges of the vertex from the current graph format.");
                break;
            }
        }
        int datasize = target_names.size();

        // Send STATUS of algorithm (success/fail)
        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;
        // Send size of vertex list
        if (send(socket, &datasize, sizeof(datasize), 0) < 0)
            sent = false;
        // Send vertex names and edge weights one pair ata time
        for (int i = 0; i < datasize; i++) {
            if (socket_send(socket, target_names[i]) == FAIL)
                sent = false;
            if (send(socket, &edge_weights[i], sizeof(edge_weights[i]), 0) < 0)
                sent = false;
        }
        if (sent) {
            PRINT_UI("Returned all data packets.");
        } else {
            PRINT_UI("One or more data packets have been lost.");
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* clear_graph(void *args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        bool sent = true;
        STATUS status_operation = FAIL;

        if (data.socket.host.size() > 0) {
            status_operation = Client::clear_graph(
                &data.socket
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::clear_graph(
                    boost::get<Graph_AL*>(data.sent.graph)
                );
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::clear_graph(
                    boost::get<Graph_AM*>(data.sent.graph)
                );
                break;
            case COMPRESSED_SPARSE_ROW:
                status_operation = mGraph::clear_graph(
                    boost::get<Graph_CSR*>(data.sent.graph)
                );
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::clear_graph(
                    boost::get<Graph_ALU*>(data.sent.graph)
                );
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::clear_graph(
                    boost::get<Graph_AMU*>(data.sent.graph)
                );
                break;
            case EDGE_LIST:
                status_operation = mGraph::clear_graph(
                    boost::get<Full_Graph_EL*>(data.sent.graph)
                );
                break;
            }
            *(data.sent.current_graph_format) = ADJACENCY_LIST;
            data.sent.graph = new Graph_AL();
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        PRINT_UI("Returned data.");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* import_graph(void *args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        bool is_encryption_file = data.sent.is_encryption_file;
        bool sent = true;
        STATUS status_operation = FAIL;
        if (data.socket.host.size() > 0) {
            string graph = "";
            status_operation = mGraph::import_graph(
                graph,
                data.sent.filename + EXT_GRAPHVIZ,
                graph_format,
                is_encryption_file
            );
            status_operation = Client::import_graph(
                &data.socket, graph, is_encryption_file
            );
        } else {
            string filename = data.sent.filename + EXT_GRAPHVIZ;
            struct stat buffer;
            bool is_file = (stat(filename.c_str(), &buffer) == 0);
            switch (graph_format) {
            case ADJACENCY_LIST:
                if (!is_file) {
                    // If the data is not a file
                    status_operation = mGraph::import_graph(
                        *(boost::get<Graph_AL*>(data.sent.graph)),
                        &data.sent.filename
                    );
                } else {
                    // If the data is a file
                    status_operation = mGraph::import_graph(
                        *(boost::get<Graph_AL*>(data.sent.graph)),
                        filename,
                        is_encryption_file
                    );
                }
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                if (!is_file) {
                    // If the data is not a file
                    status_operation = mGraph::import_graph(
                        *(boost::get<Graph_ALU*>(data.sent.graph)),
                        &data.sent.filename
                    );
                } else {
                    // If the data is a file
                    status_operation = mGraph::import_graph(
                        *(boost::get<Graph_ALU*>(data.sent.graph)),
                        filename,
                        is_encryption_file
                    );
                }
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            default:
                PRINT_STATUS("Cannot import graph to the current graph format.");
                break;
            }
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;
        if (sent) {
            PRINT_UI("Returned data.");
        } else {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* export_graph(void *args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        bool is_encryption_file = data.sent.is_encryption_file;
        STATUS status_operation = FAIL;

        if (data.socket.host.size() > 0) {
            string graph = "";
            bool is_print_graph = false;
            #ifdef DEBUG_UI
            is_print_graph = true;
            #endif
            status_operation = Client::print_graph(
                &data.socket,
                &graph,
                is_print_graph
            );
            status_operation = mGraph::export_graph(
                graph,
                data.sent.filename + EXT_GRAPHVIZ,
                is_encryption_file
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::export_graph(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    data.sent.filename + EXT_GRAPHVIZ,
                    true,
                    is_encryption_file
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::export_graph(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    data.sent.filename + EXT_GRAPHVIZ,
                    true,
                    is_encryption_file
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            default:
                PRINT_STATUS("Cannot export the current graph format.");
                break;
            }
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        PRINT_UI("Returned data.");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* print_graph(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client,
            &graph_format = *(data.sent.current_graph_format);
        bool sent = true;
        STATUS status_operation = FAIL;

        string strgraph = "";
        if (data.socket.host.size() > 0) {
            bool is_print_graph = false;
            #ifdef DEBUG_UI
            is_print_graph = true;
            #endif
            status_operation = Client::print_graph(
                &data.socket,
                &strgraph,
                is_print_graph
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::print_graph(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    strgraph
                );
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::print_graph(
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    strgraph
                );
                break;
            case COMPRESSED_SPARSE_ROW:
                status_operation = mGraph::print_graph(
                    *(boost::get<Graph_CSR*>(data.sent.graph)),
                    strgraph
                );
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::print_graph(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    strgraph
                );
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::print_graph(
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    strgraph
                );
                break;
            case EDGE_LIST:
                status_operation = mGraphEL::print_graph(
                    *(boost::get<Full_Graph_EL*>(data.sent.graph)),
                    strgraph
                );
                break;
            default:
                PRINT_STATUS("Cannot print the current graph format.");
                break;
            }
        }

        // Send status of algorithm (success/fail)
        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;
        if (socket_send(socket, strgraph) == FAIL)
            sent = false;
        if (sent) {
            PRINT_UI("Returned all data packets.");
        } else {
            PRINT_UI("One or more data packets have been lost.");
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* list_graph_formats(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        bool sent = true;
        STATUS status_operation = FAIL;

        string formats = "";
        if (data.socket.host.size() > 0) {
            status_operation = Client::list_graph_formats(
                &data.socket,
                &formats
            );
        } else {
            status_operation = mGraph::list_graph_formats(formats);
        }

        // Send STATUS of algorithm (success/fail)
        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;
        // Send format names
        if (socket_send(socket, formats) == FAIL)
            sent = false;
        if (sent) {
            PRINT_UI("Returned all data packets.");
        } else {
            PRINT_UI("One or more data packets have been lost.");
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* get_current_format(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client,
            &graph_format = *(data.sent.current_graph_format);
        STATUS status_operation = FAIL;

        string format = "";
        if (data.socket.host.size() > 0) {
            status_operation = Client::get_current_format(
                &data.socket,
                &format
            );
        } else {
            status_operation = mGraph::get_current_format(
                graph_format,
                format
            );
        }

        if (socket_send(socket, format) == FAIL) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        PRINT_UI("Returned data.");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* change_graph_format(void*args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client,
            &current_format = *data.sent.current_graph_format,
            &new_format = data.sent.new_graph_format;
        STATUS status_operation = FAIL;
        int is_change_current_host = 1;

        if (data.socket.host.size() > 0) {
            is_change_current_host = 0;
            status_operation = Client::change_graph_format(
                &data.socket,
                new_format
            );
        } else if (current_format != EDGE_LIST) {
            switch (new_format) {
            case ADJACENCY_LIST:
                switch (current_format) {
                case ADJACENCY_MATRIX:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_AM*>(data.sent.graph_copy)),
                        *(boost::get<Graph_AL*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_AM*>(data.sent.graph_copy));
                    break;
                case COMPRESSED_SPARSE_ROW:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_CSR*>(data.sent.graph_copy)),
                        *(boost::get<Graph_AL*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_CSR*>(data.sent.graph_copy));
                    break;
                case ADJACENCY_LIST_UNDIRECTED:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_ALU*>(data.sent.graph_copy)),
                        *(boost::get<Graph_AL*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_ALU*>(data.sent.graph_copy));
                    break;
                case ADJACENCY_MATRIX_UNDIRECTED:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_AMU*>(data.sent.graph_copy)),
                        *(boost::get<Graph_AL*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_AMU*>(data.sent.graph_copy));
                    break;
                }
                break;
            case ADJACENCY_MATRIX:
                switch (current_format) {
                case ADJACENCY_LIST:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_AL*>(data.sent.graph_copy)),
                        *(boost::get<Graph_AM*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_AL*>(data.sent.graph_copy));
                    break;
                case COMPRESSED_SPARSE_ROW:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_CSR*>(data.sent.graph_copy)),
                        *(boost::get<Graph_AM*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_CSR*>(data.sent.graph_copy));
                    break;
                case ADJACENCY_LIST_UNDIRECTED:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_ALU*>(data.sent.graph_copy)),
                        *(boost::get<Graph_AM*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_ALU*>(data.sent.graph_copy));
                    break;
                case ADJACENCY_MATRIX_UNDIRECTED:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_AMU*>(data.sent.graph_copy)),
                        *(boost::get<Graph_AM*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_AMU*>(data.sent.graph_copy));
                    break;
                }
                break;
            case COMPRESSED_SPARSE_ROW:
                switch (current_format) {
                case ADJACENCY_LIST:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_AL*>(data.sent.graph_copy)),
                        *(boost::get<Graph_CSR*>(data.sent.graph))
                    );
                    delete (boost::get<Graph_AL*>(data.sent.graph_copy));
                    break;
                case ADJACENCY_MATRIX:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_AM*>(data.sent.graph_copy)),
                        *(boost::get<Graph_CSR*>(data.sent.graph))
                    );
                    delete (boost::get<Graph_AM*>(data.sent.graph_copy));
                    break;
                default:
                    status_operation = FAIL;
                    break;
                }
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                switch (current_format) {
                case ADJACENCY_LIST:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_AL*>(data.sent.graph_copy)),
                        *(boost::get<Graph_ALU*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_AL*>(data.sent.graph_copy));
                    break;
                case ADJACENCY_MATRIX:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_AM*>(data.sent.graph_copy)),
                        *(boost::get<Graph_ALU*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_AM*>(data.sent.graph_copy));
                    break;
                case COMPRESSED_SPARSE_ROW:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_CSR*>(data.sent.graph_copy)),
                        *(boost::get<Graph_ALU*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_CSR*>(data.sent.graph_copy));
                    break;
                case ADJACENCY_MATRIX_UNDIRECTED:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_AMU*>(data.sent.graph_copy)),
                        *(boost::get<Graph_ALU*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_AMU*>(data.sent.graph_copy));
                    break;
                }
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                switch (current_format) {
                case ADJACENCY_LIST:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_AL*>(data.sent.graph_copy)),
                        *(boost::get<Graph_AMU*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_AL*>(data.sent.graph_copy));
                    break;
                case ADJACENCY_MATRIX:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_AM*>(data.sent.graph_copy)),
                        *(boost::get<Graph_AMU*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_AM*>(data.sent.graph_copy));
                    break;
                case COMPRESSED_SPARSE_ROW:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_CSR*>(data.sent.graph_copy)),
                        *(boost::get<Graph_AMU*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_CSR*>(data.sent.graph_copy));
                    break;
                case ADJACENCY_LIST_UNDIRECTED:
                    status_operation = mGraph::change_graph_format(
                        *(boost::get<Graph_ALU*>(data.sent.graph_copy)),
                        *(boost::get<Graph_AMU*>(data.sent.graph)),
                        new_format
                    );
                    delete (boost::get<Graph_ALU*>(data.sent.graph_copy));
                    break;
                }
                break;
            case EDGE_LIST:
                status_operation = SUCCESS;
                break;
            }
        } else {
            switch (new_format) {
            case ADJACENCY_LIST:
                status_operation = mGraphEL::change_graph_format(
                    *(boost::get<Full_Graph_EL*>(data.sent.graph_copy)),
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    new_format
                );
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraphEL::change_graph_format(
                    *(boost::get<Full_Graph_EL*>(data.sent.graph_copy)),
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    new_format
                );
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraphEL::change_graph_format(
                    *(boost::get<Full_Graph_EL*>(data.sent.graph_copy)),
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    new_format
                );
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraphEL::change_graph_format(
                    *(boost::get<Full_Graph_EL*>(data.sent.graph_copy)),
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    new_format
                );
                break;
            default:
                status_operation = FAIL;
                break;
            }
            if (status_operation == SUCCESS)
                delete (boost::get<Full_Graph_EL*>(data.sent.graph_copy));
        }

        if (is_change_current_host == 1 && status_operation == SUCCESS)
            *(data.sent.current_graph_format) = data.sent.new_graph_format;

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        else if (status_operation == SUCCESS){
            PRINT_UI("Changed graph successfully.");
        }
        else {
            PRINT_STATUS("Failed to change graph.");
        }
        PRINT_UI("Returned data.");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* run_bfs(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        bool sent = true;
        STATUS status_operation = FAIL;

        string vertexlist = "",
               edgelist = "";
        if (data.socket.host.size() > 0) {
            status_operation = Client::run_bfs(
                &data.socket,
                data.sent.name_vertex_from,
                &vertexlist, &edgelist
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::run_bfs(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    data.sent.name_vertex_from, graph_format,
                    vertexlist, edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::run_bfs(
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    data.sent.name_vertex_from, graph_format,
                    vertexlist, edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AM*>(data.sent.graph)));
                #endif
                break;
            case COMPRESSED_SPARSE_ROW:
                status_operation = mGraph::run_bfs(
                    *(boost::get<Graph_CSR*>(data.sent.graph)),
                    data.sent.name_vertex_from, graph_format,
                    vertexlist, edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_CSR*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::run_bfs(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    data.sent.name_vertex_from, graph_format,
                    vertexlist, edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::run_bfs(
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    data.sent.name_vertex_from, graph_format,
                    vertexlist, edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AMU*>(data.sent.graph)));
                #endif
                break;
            default:
                PRINT_STATUS("Cannot run operation on this graph format.");
                status_operation = FAIL;
                break;
            }
        }

        // Send STATUS of algorithm (success/fail)
        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;
        // Send vertex names one by one
        if (socket_send(socket, vertexlist) == FAIL
            || socket_send(socket, edgelist) == FAIL)
            sent = false;
        if (sent) {
            PRINT_UI("Returned all data packets.");
        } else {
            PRINT_UI("One or more data packets have been lost.");
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* run_dfs(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        bool sent = true;
        STATUS status_operation = FAIL;

        string vertexlist = "",
               edgelist = "";
        if (data.socket.host.size() > 0) {
            status_operation = Client::run_dfs(
                &data.socket,
                data.sent.name_vertex_from,
                &vertexlist, &edgelist
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::run_dfs(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    data.sent.name_vertex_from, graph_format,
                    vertexlist, edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::run_dfs(
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    data.sent.name_vertex_from, graph_format,
                    vertexlist, edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AM*>(data.sent.graph)));
                #endif
                break;
            case COMPRESSED_SPARSE_ROW:
                status_operation = mGraph::run_dfs(
                    *(boost::get<Graph_CSR*>(data.sent.graph)),
                    data.sent.name_vertex_from, graph_format,
                    vertexlist, edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_CSR*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::run_dfs(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    data.sent.name_vertex_from, graph_format,
                    vertexlist, edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::run_dfs(
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    data.sent.name_vertex_from, graph_format,
                    vertexlist, edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AMU*>(data.sent.graph)));
                #endif
                break;
            default:
                PRINT_STATUS("Cannot run operation on this graph format.");
                status_operation = FAIL;
                break;
            }
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) <= 0)
            sent = false;
        if (socket_send(socket, vertexlist) == FAIL
            || socket_send(socket, edgelist) == FAIL)
            sent = false;
        if (sent) {
            PRINT_UI("Returned all data packets.");
        } else {
            PRINT_UI("One or more data packets have been lost.");
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* run_sps(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        bool sent = true;
        STATUS status_operation = FAIL;

        string vertexlist = "";
        if (data.socket.host.size() > 0) {
            status_operation = Client::run_sps(
                &data.socket,
                data.sent.name_vertex_from,
                data.sent.name_vertex_to,
                &vertexlist
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::run_sps(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    vertexlist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::run_sps(
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    vertexlist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AM*>(data.sent.graph)));
                #endif
                break;
            case COMPRESSED_SPARSE_ROW:
                status_operation = mGraph::run_sps(
                    *(boost::get<Graph_CSR*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    vertexlist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_CSR*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::run_sps(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    vertexlist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::run_sps(
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    vertexlist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AM*>(data.sent.graph)));
                #endif
                break;
            case EDGE_LIST:
                status_operation = mGraphEL::run_sps(
                    *(boost::get<Full_Graph_EL*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    vertexlist
                );
                break;
            default:
                PRINT_STATUS("Cannot run operation on this graph format.");
                status_operation = FAIL;
                break;
            }
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;
        if (socket_send(socket, vertexlist) == FAIL)
            sent = false;
        if (sent) {
            PRINT_UI("Returned all data packets.");
        } else {
            PRINT_UI("One or more data packets have been lost.");
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* clustering(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        bool sent = true;
        STATUS status_operation;

        string edgelist = "";
        if (data.socket.host.size() > 0) {
            status_operation = Client::clustering(
                &data.socket,
                data.sent.clustering_threshold,
                &edgelist
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::clustering(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    data.sent.clustering_threshold,
                    edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)),
                        edgelist
                    );
                #endif
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::clustering(
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    data.sent.clustering_threshold,
                    edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AM*>(data.sent.graph)),
                        edgelist
                    );
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::clustering(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    data.sent.clustering_threshold,
                    edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)),
                        edgelist
                    );
                #endif
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::clustering(
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    data.sent.clustering_threshold,
                    edgelist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AMU*>(data.sent.graph)),
                        edgelist
                    );
                #endif
                break;
            default:
                PRINT_STATUS("Cannot run operation on this graph format.");
                status_operation = FAIL;
                break;
            }
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;
        if (socket_send(socket, edgelist) == FAIL)
            sent = false;
        if (sent) {
            PRINT_UI("Returned all data packets.");
        } else {
            PRINT_UI("One or more data packets have been lost.");
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* centrality_analysis(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        bool sent = true;
        STATUS status_operation = FAIL;

        string vertexlist = "";
        if (data.socket.host.size() > 0) {
            status_operation = Client::centrality_analysis(
                &data.socket,
                &vertexlist
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::centrality_analysis(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    vertexlist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AL*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::centrality_analysis(
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    vertexlist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AM*>(data.sent.graph)));
                #endif
                break;
            case COMPRESSED_SPARSE_ROW:
                status_operation = mGraph::centrality_analysis(
                    *(boost::get<Graph_CSR*>(data.sent.graph)),
                    vertexlist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_CSR*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::centrality_analysis(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    vertexlist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_ALU*>(data.sent.graph)));
                #endif
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::centrality_analysis(
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    vertexlist
                );
                #ifdef DEBUG_EXPORT_GRAPH_IMAGE
                    mGraphHelper::export_image(
                        *(boost::get<Graph_AMU*>(data.sent.graph)));
                #endif
                break;
            default:
                PRINT_STATUS("Cannot run operation on this graph format.");
                status_operation = FAIL;
                break;
            }
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;
        if (socket_send(socket, vertexlist) == FAIL)
            sent = false;
        if (sent) {
            PRINT_UI("Returned all data packets.");
        } else {
            PRINT_UI("One or more data packets have been lost.");
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* run_data_concat(void* args)
    {
        // 1. Extract data from argument
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        string &vertex = data.sent.name_vertex_from;
        bool sent = true;
        STATUS status_operation = FAIL;

        if (graph_format == ADJACENCY_LIST) {
            boost::graph_traits<Graph_AL>::vertex_descriptor v_descriptor;
            status_operation = get_vertex_from_name(v_descriptor,
                *(boost::get<Graph_AL*>(data.sent.graph)), vertex
            );
        } else if (graph_format == ADJACENCY_LIST_UNDIRECTED) {
            boost::graph_traits<Graph_ALU>::vertex_descriptor v_descriptor;
            status_operation = get_vertex_from_name(v_descriptor,
                *(boost::get<Graph_ALU*>(data.sent.graph)), vertex
            );
        } else {
            PRINT_STATUS("Cannot run operation with current graph type.");
            status_operation = FAIL;
        }

        // 2. Run algorithm
        int added_edges = 0;
        if (status_operation == SUCCESS) {
            string vertexlist = "",
            edgelist = "";
            status_operation = Client::run_dfs(&data.socket,
                data.sent.name_vertex_from,
                &vertexlist, &edgelist
            );
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::run_data_concat(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    vertexlist, edgelist, graph_format, added_edges
                );
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::run_data_concat(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    vertexlist, edgelist, graph_format, added_edges
                );
                break;
            default:
                status_operation = FAIL;
                PRINT_STATUS("Cannot run operation with current graph type.");
                break;
            }
        } else {
            PRINT_STATUS("Vertex does not exist.");
            status_operation = FAIL;
        }

        // 3. Send status of algorithm (success/fail)
        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;

        if (status_operation == SUCCESS) {
            // 4. Return number of added vertices
            if (send(socket, &added_edges, sizeof(added_edges), 0) < 0)
                sent = false;
            if (sent) {
                PRINT_UI("Returned data.");
            } else {
                PRINT_STATUS("Failed to return data.");
                return (void*)FAIL;
            }
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* quit_backend(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);

        STATUS status_operation = mGraph::quit_backend(data);

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* reset(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);

        STATUS status_operation = mGraph::reset(data);

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
    void* get_status(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;

        STATUS status_operation = mGraph::get_status(data);
        int cmd_latest = data.processed.command;

        if (send(socket, &cmd_latest, sizeof(cmd_latest), 0) < 0) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        PRINT_UI("Returned data.");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }

    void* send_sub_graph(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        STATUS status_operation = FAIL;
        string strGraph = "";
        int &graph_format = *(data.sent.current_graph_format);
        bool is_encryption_file = data.sent.is_encryption_file;
        string filename = data.sent.filename; // +EXT_GRAPHVIZ;
        switch (graph_format)
        {
        case ADJACENCY_LIST:
        if (data.socket.host.size() > 0) {
            status_operation = Client::load_sub_graph(
                &data.socket,
                filename,
                strGraph,
                is_encryption_file
            );
            mGraph::import_graph(
                *(boost::get<Graph_AL*>(data.sent.graph)),
                &strGraph
            );
        }
        else
        {
            status_operation = mGraph::import_graph(
                strGraph, filename + EXT_GRAPHVIZ, ADJACENCY_LIST, is_encryption_file
            );
        }
        break;
        case ADJACENCY_LIST_UNDIRECTED:
        if (data.socket.host.size() > 0) {
            status_operation = Client::load_sub_graph(
                &data.socket,
                filename,
                strGraph,
                is_encryption_file
            );
            mGraph::import_graph(
                *(boost::get<Graph_ALU*>(data.sent.graph)),
                &strGraph
            );
        } 
        else
        {
            status_operation = mGraph::import_graph(
                strGraph, filename + EXT_GRAPHVIZ, ADJACENCY_LIST_UNDIRECTED, is_encryption_file
            );
        }
        break;
        default:
            PRINT_STATUS("Cannot send sub_graph from the current graph format.");
            break;
        }
#ifdef PRINT_SUB_GRAPH
        PRINT_UI(strGraph);
#endif

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        PRINT_UI("Returned data.");

        if (status_operation == SUCCESS)
        {
            while (recv(socket, &status_operation, sizeof(status_operation), 0) <= SUCCESS)
                continue;
            socket_send(socket, strGraph);
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);

        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }

    void* save_result_from_edge_device(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        STATUS status_operation = FAIL;
        string result = "";

        COMMAND cmd = CMD_NULL;

        if (recv(socket, &cmd, sizeof(cmd), 0) <= 0) {
            PRINT_STATUS("Could not receive command.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }

        status_operation = socket_recv(socket, result);
        ofstream myfile;
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        string filename = to_string(ms.count()) + "_result.txt";
        myfile.open(filename);
        myfile << "COMMAND: " << print_command((COMMAND)cmd) << endl;
        myfile << "RESULT : " << result << endl;
        myfile.close();

        cout << "COMMAND: " << print_command((COMMAND)cmd) << endl;
        cout << "RESULT : " << result << endl;

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        PRINT_UI("Returned data.");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);

        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }

    void* run_sps_all_paths(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int &socket = data.sent.socket_operation_client;
        int &graph_format = *(data.sent.current_graph_format);
        bool sent = true;
        STATUS status_operation = FAIL;

        string vertexlist = "";
        if (data.socket.host.size() > 0) {
            status_operation = Client::run_sps_all_path(
                &data.socket,
                data.sent.name_vertex_from,
                data.sent.name_vertex_to,
                &vertexlist
            );
        } else {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraph::run_sps_all_paths(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    vertexlist
                );
                break;
            case ADJACENCY_MATRIX:
                status_operation = mGraph::run_sps_all_paths(
                    *(boost::get<Graph_AM*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    vertexlist
                );
                break;
            case COMPRESSED_SPARSE_ROW:
                status_operation = mGraph::run_sps_all_paths(
                    *(boost::get<Graph_CSR*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    vertexlist
                );
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraph::run_sps_all_paths(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    vertexlist
                );
                break;
            case ADJACENCY_MATRIX_UNDIRECTED:
                status_operation = mGraph::run_sps_all_paths(
                    *(boost::get<Graph_AMU*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    vertexlist
                );
                break;
            case EDGE_LIST:
                status_operation = mGraphExtend::run_sps_all_paths_el(
                    *(boost::get<Full_Graph_EL*>(data.sent.graph)),
                    data.sent.name_vertex_from,
                    data.sent.name_vertex_to,
                    vertexlist
                );
                break;
            default:
                PRINT_STATUS("Cannot run operation on this graph format.");
                status_operation = FAIL;
                break;
            }
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;
        if (socket_send(socket, vertexlist) == FAIL)
            sent = false;
        if (sent) {
            PRINT_UI("Returned all data packets.");
        } else {
            PRINT_UI("One or more data packets have been lost.");
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }

    // GRAPH_EXTEND
    void* run_graph_partition(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int& socket = data.sent.socket_operation_client;
        int& graph_format = *(data.sent.current_graph_format);
        string fn_prefix = "subgraph";
        bool sent = true;
        STATUS status_operation = FAIL;

        if (data.socket.host.size() > 0) {
            status_operation = Client::run_graph_partition(
                &data.socket,
                data.sent.num_parts,
                data.sent.cut_type
            );
        }
        else
        {
            switch (graph_format) {
            case ADJACENCY_LIST:
                status_operation = mGraphExtend::run_gpart(
                    *(boost::get<Graph_AL*>(data.sent.graph)),
                    data.sent.num_parts,
                    data.sent.cut_type,
                    fn_prefix
                );
                break;
            case ADJACENCY_LIST_UNDIRECTED:
                status_operation = mGraphExtend::run_gpart(
                    *(boost::get<Graph_ALU*>(data.sent.graph)),
                    data.sent.num_parts,
                    data.sent.cut_type,
                    fn_prefix
                );
                break;
            default:
                PRINT_STATUS("Cannot run operation on this graph format.");
                status_operation = FAIL;
                break;
            }
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;
        if (sent) {
            PRINT_UI("Returned all data packets.");
        }
        else {
            PRINT_UI("One or more data packets have been lost.");
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }

    void* get_subgraph_info(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int& socket = data.sent.socket_operation_client;
        int graph_format = *(data.sent.current_graph_format);
        bool sent = true;
        STATUS status_operation = FAIL;

        string subgraph_fname = "";
        string source_vname = "";
        string local_dest_vname = "";

        if (data.socket.host.size() > 0) {
            status_operation = Client::get_subgraph_info(
                &data.socket,
                data.sent.global_path,
                &subgraph_fname,
                &source_vname,
                &local_dest_vname
            );
        }
        else {
            status_operation = mCloudServer::get_subgraph_info(
                data.sent.global_path,
                graph_format,
                subgraph_fname,
                source_vname,
                local_dest_vname
            );
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;

        if (status_operation == SUCCESS)
        {
            if (socket_send(socket, subgraph_fname)   == FAIL ||
                socket_send(socket, source_vname)     == FAIL ||
                socket_send(socket, local_dest_vname) == FAIL)
                sent = false;
            if (sent) {
                PRINT_UI("Returned all data packets.");
            }
            else {
                PRINT_UI("One or more data packets have been lost.");
            }
        }
        else
            PRINT_UI("Failed to get subgraph information");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }

    void* evaluate_path(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int& socket = data.sent.socket_operation_client;
        bool sent = true;
        STATUS status_operation = FAIL;

        STATUS eval;
        int overlap;

        if (data.socket.host.size() > 0) {
            status_operation = Client::evaluate_path(
                &data.socket,
                data.sent.local_path,
                data.sent.sg_fname,
                &eval,
                &overlap
            );
        }
        else {
            status_operation = mCloudServer::evaluate_path(
                data.sent.local_path,
                data.sent.sg_fname,
                eval,
                overlap
            );
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;

        if (send(socket, &eval, sizeof(eval), 0) < 0 ||
            send(socket, &overlap, sizeof(overlap), 0) < 0) {
            PRINT_STATUS("Failed to return data.");
            close_socket(data.sent.socket_operation_client);
            close_socket(data.sent.socket_operation_server);
            PRINT_UI("Finished processing.");
            return (void*)FAIL;
        }
        PRINT_UI("Returned data.");

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }

    void* register_path(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int& socket = data.sent.socket_operation_client;
        bool sent = true;
        STATUS status_operation = FAIL;

        if (data.socket.host.size() > 0) {
            status_operation = Client::register_path(
                &data.socket,
                data.sent.local_path,
                data.sent.sg_fname
            );
        }
        else {
            status_operation = mCloudServer::register_path(
                data.socket.client_ip_addr,
                data.sent.local_path,
                data.sent.sg_fname
            );
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;
        if (sent) {
            PRINT_UI("Returned all data packets.");
        }
        else {
            PRINT_UI("One or more data packets have been lost.");
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }

    void* remove_path(void* args)
    {
        pthread_arg data = *((pthread_arg*)args);
        int& socket = data.sent.socket_operation_client;
        bool sent = true;
        STATUS status_operation = FAIL;

        if (data.socket.host.size() > 0) {
            status_operation = Client::remove_path(
                &data.socket
            );
        }
        else {
            status_operation = mCloudServer::remove_path(
                data.socket.client_ip_addr
            );
        }

        if (send(socket, &status_operation, sizeof(status_operation), 0) < 0)
            sent = false;
        if (sent) {
            PRINT_UI("Returned all data packets.");
        }
        else {
            PRINT_UI("One or more data packets have been lost.");
        }

        close_socket(data.sent.socket_operation_client);
        close_socket(data.sent.socket_operation_server);
        PRINT_UI("Finished processing.");
        return (void*)SUCCESS;
    }
}