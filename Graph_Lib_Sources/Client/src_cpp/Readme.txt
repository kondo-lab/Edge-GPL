Client source code

- Contains the C++ program from which the client is built.
- The main file (client_main.cpp) links to all other modules.
- Modules are the respective funtions to be called for each command. A module sends the command and receives the returned data.
    add_edge.cpp: sends the respective command, the name of source and target vertices and receives operation status.
    add_vertex.cpp: sends the respective command, the name of the vertex to be added and receives operation status.
    centrality_analysis.cpp: sends the respective command and receives operation status, vertex list.
    change_graph_format.cpp: sends the respective command, the format to change to and receives operation status.
    clear_graph.cpp: sends the respective command and receives operation status.
    client_main.cpp: main module that receives user input (command and data) and calls the appropriate module with user input as arguments.
    clustering.cpp: sends the respective command and receives operation status, edge list.
    exit_client.cpp: sends the respective command and closes the client.
    export_graph.cpp: sends the respective command, the name of the file to export to and receives operation status.
    get_current_format.cpp: sends the respective command and receives the current graph format.
    get_edge_in_vertex.cpp: sends the respective command and receives a list of source vertices and edge weights.
    get_edge_out_vertex.cpp: sends the respective command and receives a list of target vertices and edge weights.
    get_num_edge.cpp: sends the respective command and receives the number of vertices in the current graph.
    get_num_vertex.cpp: sends the respective command and receives the number of edges in the current graph.
    get_status.cpp: sends the respective command and receives the latest command.
    import_graph.cpp: sends the respective command, the name of the file to import from and receives operation status.
    list_graph_formats.cpp: sends the respective command and receives format list.
    module.cpp: contains helper functions for other modules.
    module.h: contains all function and struct declarations.
    print_graph.cpp: sends the respective command and reads the graph from the temp file.
    quit_backend.cpp: sends the respective command and closes the client.
    remove_edge.cpp: sends the respective command,  the name of source and target vertices and receives operation status.
    remove_vertex.cpp: sends the respective command, the name of the vertex to be removed and receives operation status.
    reset.cpp: sends the respective command.
    run_bfs.cpp: sends the respective command, the name of the root vertex and receives operation status, vertex list.
    run_data_concat.cpp: sends the respective command, the name of the root vertex, host ip and receives operation status, number of added edges.
    run_dfs.cpp: sends the respective command, the name of the root vertex and receives operation status, vertex list.
    run_sps.cpp: sends the respective command, the name of the source vertex, target vertex and receives operation status, vertex list.
    load_sub_graph.cpp: sends the respective command, the name of the file subgraph to import from and receives operation status.
    send_result.cpp: sends the respective command, the text of results will save into server side.
    sub_graph.cpp: sends the respective command, the name of the vertex or the edge and receives the subgraph filename.