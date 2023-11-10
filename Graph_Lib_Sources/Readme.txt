User Manual

I. Program overview

- Graph processing progam that runs daemon on compact devices with limited processing capability such as the Raspberry Pi.
- Graphs are based on the Boost Graph Library (BGL)
- Device connection established using sockets
- Available in C++ and Python (optional)

II. Installation

- Installing Boost
   Run the run_pi_install.sh file from the terminal: $ sudo ./run_pi_install.sh
- Build
   Compile the source code using make command
       - server: $ make server
       - client: $ make client
       - all: $ make
- Run
     - server: $ ./server
     - client: $ ./client
               $ python3 client_main.py

III. Available commands in the progam
        CMD_EXIT:
            - Function: Close the client.
            - Input: none
            - Output: none
            - Note: This does not affect the server.
        CMD_ADD_VERTEX:
            - Function: Add a new vertex to the graph.
            - Input: Vertex name.
            - Output: Status of the requested operation. (success/fail)
            - Note: Does not add a new vertex if the suplied vertex anme already exists.
        CMD_REMOVE_VERTEX:
            - Function: Remove a vertex fromt the graph.
            - Input: Vertex name.
            - Output: Status of the requested operation. (success/fail)
            - Note: Operation fails if the vertex does not exist.
        CMD_NUM_VERTEX:
            - Function: Returns the number of vertices in the current graph.
            - Input: none
            - Output: Status of the requested operation. (success/fail)
            - Note: none
        CMD_ADD_EDGE: 
            - Function: Add a new edge to the graph.
            - Input: Names of the source and target vertices, the edge weight.
            - Output: Status of the requested operation. (success/fail)
            - Note: Does not add a new edge if either vertex does not exist. If the edge already exists, the weight of that edge is updated.
        CMD_REMOVE_EDGE:
            - Function: Remove the edge from the graph.
            - Input: Names of the source and target.
            - Output: Status of the requested operation. (success/fail)
            - Note: Operation fails if either vertices does not exist.
        CMD_NUM_EDGE:
            - Function: Returns the number of edges in the current graph.
            - Input: none
            - Output: Status of the requested operation. (success/fail)
            - Note: none
        CMD_EDGE_IN_VERTEX:
            - Function: Returns a list of source vertices to the supplied vertex and edge weights.
            - Input: Vertex name.
            - Output: Status of the requested operation. (success/fail)
            - Note: Operation fails if vertex does not exist.
        CMD_EDGE_OUT_VERTEX:
            - Function: Returns a list of target vertices to the supplied vertex and edge weights.
            - Input: Vertex name.
            - Output: Status of the requested operation. (success/fail)
            - Note: Operation fails if vertex does not exist.
        CMD_CLEAR_ALL_GRAPH:
            - Function: Removes all vertices and edges in the current graph.
            - Input: none
            - Output: Status of the requested operation (success/fail)
            - Note: none
        CMD_IMPORT_GRAPH:
            - Function: Imports a graph from a file.
            - Input: File name.
            - Output: Status of the requested operation (success/fail)
            - Note: Operation fails if the file does not exist.
        CMD_EXPORT_GRAPH:
            - Function: Export the graph to a file.
            - Input: File name.
            - Output: Status of the requested operation (success/fail)
            - Note: Operation fails if the file does not exist.
        CMD_PRINT_GRAPH:
            - Function: Print the graph to terminal.
            - Input: none
            - Output: none
            - Note: none
        CMD_LIST_GRAPH_FORMATS:
            - Function: Print the available graph formats and their codes to terminal.
            - Input: none
            - Output: none
            - Note: none
        CMD_CURRENT_FORMAT:
            - Function: Print the current graph format to terminal.
            - Input: none
            - Output: none
            - Note: none
        CMD_CHANGE_GRAPH_FORMAT:
            - Function: Switch to another graph format.
            - Input: Code of new graph format.
            - Output: Status of the requested operation (success/fail)
            - Note: none
        CMD_BFS:
            - Function: Run Breadth-First Search on the graph with the supplied vertex as the root.
            - Input: Vertex name.
            - Output: Status of the requested operation (success/fail). List of names of traversed vertices.
            - Note: The algorithm does not run if the supplied vertex does not exist.
        CMD_DFS:
            - Function: Run Depth-First Search on the graph with the supplied vertex as the root.
            - Input: Vertex name.
            - Output: Status of the requested operation (success/fail). List of names of traversed vertices.
            - Note: The algorithm does not run if the supplied vertex does not exist.
        CMD_SPS:
            - Function: Run Dijkstra's Shortest Path (Bellman-Ford for edge list) on the graph with the supplied source vertex as the root.
            - Input: Names of the source and target vertices.
            - Output: Status of the requested operation (success/fail). List of names of traversed vertices in the path
                        from the source vertex to the target.
            - Note: The algorithm does not run if the either supplied vertices does not exist.
        CMD_CENTRALITY_ANALYSIS:
            - Function: Run Brandes Betweenness Centrality on the graph with the supplied vertex as the root.
            - Input: none
            - Output: Status of the requested operation (success/fail). List of names of highest centrality vertices.
            - Note: none
        CMD_CLUSTERING:
            - Function: Run Edge Betweenness Clustering on the graph with the supplied vertex as the root.
            - Input: none
            - Output: Status of the requested operation (success/fail). List of remaining edges.
            - Note: none
        CMD_DATA_CONCAT:
            - Function: Calls Depth-First Search to be run on another daemon with the supplied vertex
                        as the root vertex. Receives list of traversed vertices and edges that are added to the current graph
                        (if they are not already in the graph)
            - Input: Vertex name and DFS host ip address.
            - Output: Status of the requested operation (success/fail). Number of added edges.
            - Note: The algorithm does not run if the supplied vertex does not exist in the DFS daemon or if the host ip
                    does not exist.
        CMD_QUIT_BACKEND:
            - Function: Close both the client and the server.
            - Input: none
            - Output: none
            - Note: none
        CMD_GET_STATUS:
            - Function: Returns the latest command received by the server prior to CMD_GET_STATUS.
            - Input: none
            - Output: Latest command.
            - Note: none
        CMD_RESET:
            - Function: Closes any open threads, frees allocated memory (if any) and sets server status to available.
            - Input: none
            - Output: none
            - Note: Operation runs regardless of server status.
			
		CMD_LOAD_SUB_GRAPH:
			- Load subgraph into memory on edge device
			
		CMD_SPS_ALL_PATH:
			- List all shortest paths among two nodes of the graph

        CMD_GRAPH_PARTITION
            - Perform graph partition, then save subgraphs to file
        
        CMD_SUBGRAPH_INFO
            - Get subgraph information

        CMD_EVALUATE_PATH
            - Evaluate shortest path

        CMD_REGISTER_PATH
            - Add shortest path to "determined paths"

        CMD_REMOVE_PATH
            - Remove shortest path from "determined paths"
            