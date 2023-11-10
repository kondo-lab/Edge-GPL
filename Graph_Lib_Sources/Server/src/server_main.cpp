/* version 3 */
#include "server_thread.h"

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

	// 2. Start thread to read commands
	pthread_t tidCmd;
	if (pthread_create(&tidCmd, NULL, t_command_server, &socket_information) < 0) {
		PRINT_STATUS("Failed to create command thread.");
		return FAIL;
	}
	pthread_join(tidCmd, NULL);

	PRINT_UI("Disconnected.");
	return SUCCESS;
}