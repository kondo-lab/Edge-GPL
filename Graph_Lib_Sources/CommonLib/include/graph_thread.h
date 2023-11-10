/* version 3 */
#include "graph_structure.h"
#include "graph_algorithms.h"
#include "graph_edge_list.h"
#include "socket.h"

namespace mGraphThreads {
	void* add_vertex(void* args);
	void* remove_vertex(void* args);
	void* get_num_vertex(void *arg);
	void* add_edge(void* args);
	void* remove_edge(void* args);
	void* get_num_edge(void *arg);
	void* get_edge_in_vertex(void* args);
	void* get_edge_out_vertex(void* args);
	void* clear_graph(void* args);
	void* import_graph(void* args);
	void* export_graph(void* args);
	void* print_graph(void* args);
	void* list_graph_formats(void* args);
	void* get_current_format(void* args);
	void* change_graph_format(void* args);
	void* run_bfs(void *arg);
	void* run_dfs(void *arg);
	void* run_sps(void *arg);
	void* clustering(void *arg);
	void* centrality_analysis(void *arg);
	void* run_data_concat(void *arg);
	void* quit_backend(void *arg);
	void* reset(void *arg);
	void* get_status(void *arg);
	void* send_sub_graph(void *arg);
    void* save_result_from_edge_device(void* args);
    void* run_sps_all_paths(void* args);
	void* run_graph_partition(void* args);
	void* get_subgraph_info(void* args);
	void* evaluate_path(void* args);
	void* register_path(void* args);
	void* remove_path(void* args);
}