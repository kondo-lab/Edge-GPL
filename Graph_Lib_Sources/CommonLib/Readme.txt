Graph processing library source code

- Contains the code related to sockets and graph processing.
- Folder structure:
    - The 'include' folder contains the library headers:
        - graph_algorithms.h: contains function declarations for the respective .cpp file's functions as well as some helper
                                structs for the BGL algorithms.
        - graph_structure.h: contains function declarations for the respective .cpp file's functions.
        - graph_thread.h: contains function declarations for the respective .cpp file's functions.
        - graph.h: contains definitions for the graph types and helper types as well as helper functions.
        - socket.h: contains struct definitions for pthread arguments as well as function declarations for the
                    respective .cpp file's functions.
        - graph_edge_list.h: contains function declarations for the respective .cpp file's functions as well as a helper
                        function for the BGL algorithms.
    - The 'src' folder contains the actual function definitions
        - graph_algorithms.cpp: contains the code to graph processing algorithms such as BFS, DFS, finding shortest path etc.
        - graph_structure.cpp: contains the code to graph altering functions such as adding/removing vertices and edges etc.
        - graph_thread.cpp: contains the pthread operation functions that are to be called from the server command thread.
        - socket.cpp: contains socket-related helper functions such as creating/closing sockets.
        - graph_edge_list.cpp: ontains the code to customized edge list functions.
