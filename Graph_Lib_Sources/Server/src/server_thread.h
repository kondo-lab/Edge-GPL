/* version 3 */
#pragma once

#include "../../CommonLib/include/graph_thread.h"

#ifdef G_THREAD
	int g_graph_format = ADJACENCY_LIST;
	int n_vertices_el = 0;
	vector<Edge_EL> edges_el;
	vector<string> vertices_el, names_el;
	vector<int> index_el, weights_el;
#endif

void* t_command_server(void* arg);