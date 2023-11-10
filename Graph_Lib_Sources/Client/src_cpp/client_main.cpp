/* version 3 */
#include "module.h"
using namespace Client;
/*
    Name: main
    Function: Main client module. Gets user input and calls the respective
        module for the user command.
    Input: Host IP address socket port, operation socket port (all
        are optional).
*/

int main(int argc, char* argv[]) {
    // 1. Extract command-line arguments
    socket_info socket_information;
    int i = 1;
    while(i < argc) {
        if (string(argv[i]).compare("--host") == SUCCESS
                && i + 1 < argc) {
            socket_information.host = string(argv[i + 1]);
            i += 2;
        } else if (string(argv[i]).compare("--command_port") == SUCCESS
                && i + 1 < argc) {
            socket_information.command_port = atoi(argv[i + 1]);
            i += 2;
        } else if (string(argv[i]).compare("--operation_port") == SUCCESS
                && i + 1 < argc ) {
            socket_information.operation_port = atoi(argv[i + 1]);
            i += 2;
        } else {
            PRINT_STATUS("Invalid argument(s).");
            return FAIL;
        }
    }

    // 2. Declare variables to be used
    string vertex_from, vertex_to, filename;
    int edge_weight = 0,
        graph_format = 0;
    int num_parts = 0;
    int cut_type = 0;
    string global_path, local_path, sg_fname;
    int socket_command = -1,
        socket_operation = -1;
    bool is_encryption = false;
    COMMAND command = CMD_NULL;
    double clustering_threshold = 0;
    STATUS status_command = FAIL;
    string str_graph = "";
    init_openssl(); // init openssl

    // 3. Get the command
    get_command(command);
    if (command == CMD_EXIT)
        return SUCCESS;
    if (command != CMD_QUIT_BACKEND && command != CMD_GET_STATUS && command != CMD_RESET)
        get_hostname(socket_information.host_backup);

    // 4. Get the necessary data (some modules may not require any input)
    // 5. Run the appropriate module
    //      Each module takes in the command and (optionally) the entered
    //      data in the order they were entered.
    switch (command) {
    case CMD_EXIT:
        status_command = SUCCESS;
        break;
    case CMD_ADD_VERTEX:
        get_vertex(vertex_from);
        status_command = add_vertex(&socket_information, vertex_from);
        break;
    case CMD_REMOVE_VERTEX:
        get_vertex(vertex_from);
        status_command = remove_vertex(&socket_information, vertex_from);
        break;
    case CMD_NUM_VERTEX:
        status_command = get_num_vertex(&socket_information);
        break;
    case CMD_ADD_EDGE:
        get_vertex(vertex_from);
        get_vertex(vertex_to);
        get_edge_weight(edge_weight);
        status_command = add_edge(&socket_information, vertex_from, vertex_to, edge_weight);
        break;
    case CMD_REMOVE_EDGE:
        get_vertex(vertex_from);
        get_vertex(vertex_to);
        status_command = remove_edge(&socket_information, vertex_from, vertex_to);
        break;
    case CMD_NUM_EDGE:
        status_command = get_num_edge(&socket_information);
        break;
    case CMD_EDGE_IN_VERTEX:
        get_vertex(vertex_from);
        status_command = get_edge_in_vertex(&socket_information, vertex_from);
        break;
    case CMD_EDGE_OUT_VERTEX:
        get_vertex(vertex_from);
        status_command = get_edge_out_vertex(&socket_information, vertex_from);
        break;
    case CMD_CLEAR_ALL_GRAPH:
        status_command = clear_graph(&socket_information);
        break;
    case CMD_IMPORT_GRAPH:
        get_filename(filename);
        get_encryption(is_encryption);
        status_command = import_graph(&socket_information, filename, is_encryption);
        break;
    case CMD_EXPORT_GRAPH:
        get_filename(filename);
        get_encryption(is_encryption);
        status_command = export_graph(&socket_information, filename, is_encryption);
        break;
    case CMD_PRINT_GRAPH:
        status_command = print_graph(&socket_information);
        break;
    case CMD_LIST_GRAPH_FORMATS:
        status_command = list_graph_formats(&socket_information);
        break;
    case CMD_CURRENT_FORMAT:
        status_command = get_current_format(&socket_information);
        break;
    case CMD_CHANGE_GRAPH_FORMAT:
        get_graph_format(graph_format);
        status_command = change_graph_format(&socket_information, graph_format);
        break;
    case CMD_BFS:
        get_vertex(vertex_from);
        status_command = run_bfs(&socket_information, vertex_from);
        break;
    case CMD_DFS:
        get_vertex(vertex_from);
        status_command = run_dfs(&socket_information, vertex_from);
        break;
    case CMD_SPS:
        get_vertex(vertex_from);
        get_vertex(vertex_to);
        status_command = run_sps(&socket_information, vertex_from, vertex_to);
        break;
    case CMD_CENTRALITY_ANALYSIS:
        status_command = centrality_analysis(&socket_information);
        break;
    case CMD_CLUSTERING:
        get_clustering_threshold(clustering_threshold);
        status_command = clustering(&socket_information, clustering_threshold);
        break;
    case CMD_DATA_CONCAT:
        get_vertex(vertex_from);
        status_command = run_data_concat(&socket_information, vertex_from);
        break;
    case CMD_QUIT_BACKEND:
        status_command = quit_backend(&socket_information);
        break;
    case CMD_GET_STATUS:
        status_command = get_status(&socket_information);
        break;
    case CMD_RESET:
        status_command = reset(&socket_information);
        break;
    case CMD_LOAD_SUB_GRAPH:
        get_filename(filename);
        get_encryption(is_encryption);
        status_command = load_sub_graph(&socket_information, filename, str_graph, is_encryption);
        break;
    case CMD_SPS_ALL_PATH:
        get_vertex(vertex_from);
        get_vertex(vertex_to);
        status_command = run_sps_all_path(&socket_information, vertex_from, vertex_to);
        break;
    case CMD_GRAPH_PARTITION:
        get_num_parts(num_parts);
        get_cut_type(cut_type);
        status_command = run_graph_partition(&socket_information, num_parts, cut_type);
        break;
    case CMD_SUBGRAPH_INFO:
        get_global_path(global_path);
        status_command = get_subgraph_info(&socket_information, global_path);
        break;
    case CMD_EVALUATE_PATH:
        get_local_path(local_path);
        get_sg_fname(sg_fname);
        status_command = evaluate_path(&socket_information, local_path, sg_fname);
        break;
    case CMD_REGISTER_PATH:
        get_local_path(local_path);
        get_sg_fname(sg_fname);
        status_command = register_path(&socket_information, local_path, sg_fname);
        break;
    case CMD_REMOVE_PATH:
        status_command = remove_path(&socket_information);
        break;
    }
    free_openssl(); // free openssl

    PRINT_UI("status = " + to_string(status_command));
    cout << "The memory roughly occupied by this graph is: " << endl;
    system("ps aux | grep /bin/server | grep -wv grep | grep -wv sh | awk '{print $6 \" KB\"}'");

    return SUCCESS;
}