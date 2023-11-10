/* version 3 */
#include "module.h"

/*
	Name: quit_backend
	Function: Sends a request to the server to close itself.
	Input: none
*/
namespace Client {
STATUS quit_backend(socket_info *arg)
{
	// 1. Extract socket information
	socket_info *socket_information = (socket_info*) arg;
	string host = socket_information->host;
	int command_port = socket_information->command_port;

	// 2. Declare local variables
	struct sockaddr_in address_command;
	int socket_command = -1;
	COMMAND command = CMD_QUIT_BACKEND;

	// 3. Create command socket
	PRINT_UI("Creating command socket...");
	if (create_socket_client(socket_command, address_command,
			host, command_port) == FAIL) {
		PRINT_STATUS("Failed to create command socket.");
		return FAIL;
	}

	// 4. Send command to server
	PRINT_UI("Sending command " + print_command(command) + "...");
	if (send(socket_command, &command, sizeof(command), 0) <= 0) {
		PRINT_STATUS("Failed to send command.");
		return FAIL;
	}
	PRINT_UI("Sent command " + print_command(command) + ".");

	// 5. Close command socket
	close_socket(socket_command);
	PRINT_UI("Closed command socket.");

	return SUCCESS;
}
}