/* version 3 */
#include "module.h"

/*
	Name: evaluate_path
	Function: Sends a request to evaluate a local shortest path.
	Input: Local shortest path.
	Output: Evaluation result and overlap value.
*/
namespace Client {
	STATUS evaluate_path(socket_info* arg, string local_path, string sg_fname, STATUS* eval, int* overlap)
	{
		// 1. Extract socket information
		socket_info *socket_information = (socket_info*) arg;
		string host = socket_information->host;
		int command_port = socket_information->command_port,
			operation_port = socket_information->operation_port;

		// 2. Declare local variables
		struct sockaddr_in address_command,   // address command
				           address_operation; // address operation
		int socket_command = -1,           // socket command
			socket_operation = -1;         // socket operation
		STATUS status = SUCCESS, // Marks whether the requested operation was successful or not
			   ack = FAIL;
		COMMAND command = CMD_EVALUATE_PATH;

		// 3. Create command socket
		PRINT_UI("Creating command socket...");
		if (create_socket_client(socket_command, address_command,
				host, command_port) == FAIL) {
			PRINT_STATUS("Failed to create command socket.");
			return FAIL;
		}

		// This loop only serves to easily break out when an action fails
		// Jump direcly to end of code where command socket is closed
		while (true) {
			// 4. Send command to server
			PRINT_UI("Sending command " + print_command(command) + "...");
			if (send(socket_command, &command, sizeof(command), 0) <= 0) {
				PRINT_STATUS("Failed to send command.");
				status = FAIL;
				break;
			}
			PRINT_UI("Sent command " + print_command(command) + ".");

			// 5. Receive the server status
			PRINT_UI("Waiting for server...");
			while (recv(socket_command, &ack, sizeof(ack), 0) <= SUCCESS)
				continue;
			PRINT_UI("Received ACK.");
			if (ack == FAIL) {
				PRINT_STATUS("Server unavailable.");
				status = FAIL;
				break;
			}

			// 6. Create operation socket
			PRINT_UI("Creating operation socket...");
			if (create_socket_client(socket_operation, address_operation,
					host, operation_port) == FAIL) {
				PRINT_STATUS("Failed to create operation socket.");
				return FAIL;
			}

			// 7. Send data to server
			PRINT_UI("Sending data...");
			socket_send(socket_operation, socket_information->host_backup);
			socket_send(socket_operation, local_path);
			socket_send(socket_operation, sg_fname);
			PRINT_UI("Sent data.");

			// 8. Receiving data
			PRINT_UI("Receiving data from server...");
			if (recv(socket_operation, &status,
				sizeof(status), 0) <= SUCCESS) {
				PRINT_STATUS("Failed to receive processed data.");
				status = FAIL;
			}
			else if (status == SUCCESS) {
				STATUS status_data = SUCCESS;
				STATUS ev;
				int ovl;
				if (recv(socket_operation, &ev, sizeof(ev), 0) <= SUCCESS ||
					recv(socket_operation, &ovl, sizeof(ovl), 0) <= SUCCESS)
					status_data = FAIL;
				if (status_data == SUCCESS) {
					string s = (ev == SUCCESS ? "Confirmation" : "Alarm");
					PRINT_STATUS("Evaluation: " + s);
					PRINT_STATUS("Overlap:    " + to_string(ovl));
					if (eval != NULL)
						*eval = ev;
					if (overlap != NULL)
						*overlap = ovl;
				}
				else {
					PRINT_STATUS("Failed to receive processed data.");
				}
			}
			else {
				PRINT_STATUS("Failed to evaluate local shortest path.")
			}

			// 9. Close operation socket
			close_socket(socket_operation);
			PRINT_UI("Closed operation socket.");

			break;
		}

		// 10. Close command socket
		close_socket(socket_command);
		PRINT_UI("Closed command socket.");

		// 11. Exit the module
		// If an action has failed at any point, return FAIL.
		return status;
	}
} // End of namespace