/* version 3 */
#include "../include/socket.h"

namespace mGraph {
    /*
        Name: quit_backend
        Function: Closes all active sockets.
        Input: none
        Output: SUCCESS
        Return: none
    */
    STATUS quit_backend(pthread_arg &data)
    {
        close_socket(data.sent.socket_operation_server);
        close_socket(data.sent.socket_operation_client);
        return SUCCESS;
    }
    /*
        Name: reset
        Function: If the server is unavailable then closes all active sockets
            and sets the server state to AVAILABLE.
        Input: none
        Output: SUCCESS if all active sockets are closed; FAIL otherwise
        Return: none
    */
    STATUS reset(pthread_arg &data)
    {
        pthread_cancel(data.sent.thread_id);

        close_socket(data.sent.socket_operation_server);
        close_socket(data.sent.socket_operation_client);
        return SUCCESS;
    }
    /*
        Name: get_status
        Function: Returns the command prior to the CMD_GET_STATUS command.
        Input: none
        Output: SUCCESS
        Return: Previously called command.
    */
    STATUS get_status(pthread_arg &data)
    {
        data.processed.command = data.sent.latest_command;
        return SUCCESS;
    }
}

/*
    Name: get_address
    Function: Returns an address struct required by the sockets.
    Input: Host IP address and port.
*/
STATUS get_address(struct sockaddr_in &address, string host, int port)
{
    try {
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(host.c_str());
        address.sin_port = htons(port);
    } catch (...) { return FAIL; }
    return SUCCESS;
}
/*
    Name: create_socket_server
    Function: Creates a server-side socket, binds the new socket to an
        address and sets the socket to listen mode.
    Input: Host IP address and port, type of socket.
*/
STATUS create_socket_server(int &new_socket, struct sockaddr_in &address,
    string host, int port, bool type_operation)
{
    get_address(address, host, port);

    // Create the socket
    new_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (new_socket == 0) {
        PRINT_STATUS("Failed to create socket.");
        return FAIL;
    }
    PRINT_UI("Created socket.");

    // Set socket options
    int yes = 1,
        num_conection = SOMAXCONN;
    if (type_operation) {
        if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
            &yes, sizeof(yes)) < SUCCESS) {
            PRINT_STATUS("Failed to set options for socket.");
            close_socket(new_socket);
            return FAIL;
        }
        num_conection = 1;
    }
    else if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR,
        &yes, sizeof(yes)) < SUCCESS) {
        PRINT_STATUS("Failed to set options for socket.");
        close_socket(new_socket);
        return FAIL;
    }

    // Bind an ip address and port to the server socket
    PRINT_UI("Binding socket...");
    if (bind(new_socket, (sockaddr*)&address, sizeof(address)) != SUCCESS) {
        PRINT_STATUS("Failed to bind socket.");
        close_socket(new_socket);
        return FAIL;
    }
    PRINT_UI("Binded socket.");

    // The server socket is listening for a connection
    PRINT_UI("Listening for clients...");
    if (listen(new_socket, num_conection) != SUCCESS) {
        PRINT_STATUS("Failed to listen for connections.");
        close_socket(new_socket);
        return FAIL;
    }

    return SUCCESS;
}
/*
    Name: create_socket_client
    Function: Creates a server-side socket that lets the server act as a
        client and connects the socket to another server.
    Input: Host IP address and port of the other server.
*/
STATUS create_socket_client(int &new_socket, struct sockaddr_in &address,
    string host, int port)
{
    get_address(address, host, port);

    // Create the socket
    new_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (new_socket == 0)
        return FAIL;
    PRINT_UI("Created socket.");

    // Connect the client socket
    PRINT_UI("Connecting to server...");
    if (connect(new_socket, (sockaddr*)&address, sizeof(address)) < SUCCESS) {
        PRINT_STATUS("Failed to connect to server.");
        close_socket(new_socket);
        return FAIL;
    }
    PRINT_UI("Connected socket.");

    return SUCCESS;

}
/*
    Name: accept_socket
    Function: Accept a connected client socket.
    Input: Client socket descriptor, address of sockets.
*/
STATUS accept_socket(int &server_socket, int &client_socket,
    struct sockaddr_in &address)
{
    // Accept the client socket connection
    int addrlen = sizeof(address);
    client_socket = accept(server_socket,
        (sockaddr*)&address, (socklen_t*)&addrlen
    );
    if (client_socket < 0) {
        PRINT_STATUS("Failed to accept socket.");
        return FAIL;
    }
    PRINT_UI("Client socket accepted.");
    return SUCCESS;
}
/*
    Name: close_socket
    Function: Shuts down the socket and closes the descriptor for that socket.
    Input: Socket to be closed.
*/
STATUS close_socket(int &socket)
{
    shutdown(socket, SHUT_RDWR);
    if (close(socket) < SUCCESS)
        return FAIL;
    return SUCCESS;
}

const char *strCmdName[] = {
    "CMD_EXIT",
    "CMD_ADD_VERTEX",
    "CMD_REMOVE_VERTEX",
    "CMD_NUM_VERTEX",
    "CMD_ADD_EDGE",
    "CMD_REMOVE_EDGE",
    "CMD_NUM_EDGE",
    "CMD_EDGE_IN_VERTEX",
    "CMD_EDGE_OUT_VERTEX",
    "CMD_CLEAR_ALL_GRAPH",
    "CMD_IMPORT_GRAPH",
    "CMD_EXPORT_GRAPH",
    "CMD_PRINT_GRAPH",
    "CMD_LIST_GRAPH_FORMATS",
    "CMD_CURRENT_FORMAT",
    "CMD_CHANGE_GRAPH_FORMAT",
    "CMD_BFS",
    "CMD_DFS",
    "CMD_SPS",
    "CMD_CENTRALITY_ANALYSIS",
    "CMD_CLUSTERING",
    "CMD_DATA_CONCAT",
    "CMD_QUIT_BACKEND",
    "CMD_GET_STATUS",
    "CMD_RESET",
    "CMD_GRAPH_PARTITION",
    "CMD_LOAD_SUB_GRAPH",
    "CMD_SPS_ALL_PATH",
    "CMD_SUBGRAPH_INFO",
    "CMD_EVALUATE_PATH",
    "CMD_REGISTER_PATH",
    "CMD_REMOVE_PATH"
};

/*
    Name: print_command
    Function: Returns the string equivalent of a command.
    Input: Command.
*/
string print_command(COMMAND cmd) {
    if (cmd == CMD_NULL)
        return "CMD_NULL";
    return strCmdName[(int)cmd];
}

/*
    Name: socket_send
    Function: Sends a data (string type) to the client.
    Input: Data to be sent.
*/
STATUS socket_send(int &socket, string data)
{
    const char *dataSent = data.c_str();
    unsigned long bytesWritten = 0,    // to sum how many data sent
        length = strlen(dataSent); // unsigned long for big string
    int bw = 0; // to count how many data sent
    
    unsigned char *msg_origin = (unsigned char*)data.c_str();
    int len_msg_encrypt = 0;
#ifdef ENCRYPTION_ONLINE
    // encrypt data
    unsigned char *msg_encrypt = encrypt(_KEY_AES_SERVER, _IV_AES_SERVER, msg_origin, length, &len_msg_encrypt);
#else
    unsigned char *msg_encrypt = msg_origin;
    len_msg_encrypt = length;
#endif
    int countSent = len_msg_encrypt / MAX_BUFFER_SIZE,
        lastSent = len_msg_encrypt % MAX_BUFFER_SIZE;

    STATUS ack = SUCCESS; // marks client has received full data
    if (send(socket, &length, sizeof(ULONG), 0) < 0)
        return FAIL;
    // sent len_msg_encrypt of string
    if (send(socket, &len_msg_encrypt, sizeof(ULONG), 0) < 0)
        return FAIL;
    if (len_msg_encrypt == 0)
        return SUCCESS;
    // send string
    for (int i = 0; i < countSent; i++) {
        do {
            ack = SUCCESS;
            bw = send(socket, &msg_encrypt[bytesWritten], MAX_BUFFER_SIZE, 0);
            if (bw < 0)
                return FAIL;
            if (recv(socket, &ack, sizeof(ack), 0) < 0)
                return FAIL;
        } while (ack == FAIL);
        bytesWritten += bw;
    }
    // send last string
    if (lastSent > 0) {
        do {
            ack = SUCCESS;
            bw = send(socket, &msg_encrypt[bytesWritten],
                lastSent, 0);
            if (bw < 0)
                return FAIL;
            if (recv(socket, &ack, sizeof(ack), 0) < 0)
                return FAIL;
        } while (ack == FAIL);
    }
#ifdef ENCRYPTION_ONLINE
    free(msg_encrypt);
#endif
    return SUCCESS;
}
/*
    Name: socket_recv
    Function: Receives a data (string type) from another server.
    Input: Buffer for data to be received.
*/
STATUS socket_recv(int &socket, string &data) {
    data = ""; // Clear data buffer
    unsigned long bytesWritten = 0, // to sum how many data sent
              len_msg_encrypt = 0;  // unsigned long for big string
    int bw = 0; // to count how many data sent
    STATUS ack = SUCCESS; // marks client has received full data
    
    ULONG length_origin = 0;
    // recv len_msg_encrypt origin
    if (recv(socket, &length_origin, sizeof(ULONG), 0) < 0)
        return FAIL;
    // receive len_msg_encrypt of string
    if (recv(socket, &len_msg_encrypt, sizeof(ULONG), 0) < 0)
        return FAIL;
    if (len_msg_encrypt == 0)
        return SUCCESS;
    int countSent = len_msg_encrypt / MAX_BUFFER_SIZE,
        lastSent = len_msg_encrypt % MAX_BUFFER_SIZE;
    unsigned char* msg_encrypt = (unsigned char*)malloc(len_msg_encrypt * sizeof(unsigned char));
    // receive string
    for (int i = 0; i < countSent; i++) {
        char dataval[MAX_BUFFER_SIZE];
        do {
            ack = SUCCESS; // marks client has received full data
            bw = recv(socket, dataval, MAX_BUFFER_SIZE, 0);
            if (bw < 0)
                return FAIL;
            if (bw < MAX_BUFFER_SIZE)
                ack = FAIL;
            if (send(socket, &ack, sizeof(ack), 0) < 0)
                return FAIL;
        } while (ack == FAIL);
        memcpy(msg_encrypt + i * MAX_BUFFER_SIZE, dataval, MAX_BUFFER_SIZE);
    }
    // receive last string
    if (lastSent > 0) {
        char dataval[lastSent];
        do {
            ack = SUCCESS;
            bw = recv(socket, dataval, lastSent, 0);
            if (bw < 0)
                return FAIL;
            if (bw < lastSent)
                ack = FAIL;
            if (send(socket, &ack, sizeof(ack), 0) < 0)
                return FAIL;
        } while (ack == FAIL);
        memcpy(msg_encrypt + countSent * MAX_BUFFER_SIZE, dataval, lastSent);
    }
#ifdef ENCRYPTION_ONLINE
    int len_msg_decrypt = 0;
    unsigned char *msg_decrypt = decrypt(_KEY_AES_SERVER, _IV_AES_SERVER, msg_encrypt, len_msg_encrypt, &len_msg_decrypt);
    msg_decrypt[length_origin] = '\0';
    data = (char *)msg_decrypt;
    free(msg_encrypt);
#else
    msg_encrypt[length_origin] = '\0';
    data = (char *)msg_encrypt;
#endif
    return SUCCESS;
}

STATUS init_openssl()
{
    PRINT_UI("INIT OPENSSL");
    // Initialize openSSL
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);
}

STATUS free_openssl()
{
    PRINT_UI("FREE OPENSSL");
    //clean up ssl;
    EVP_cleanup(); 
    CRYPTO_cleanup_all_ex_data(); //Stop data leaks
    ERR_free_strings();
}

unsigned char* encrypt(
    const unsigned char _KEY_AES[], 
    const unsigned char _IV_AES[], 
    unsigned char *msg_origin, 
    int len_msg_origin,
    int *len_msg_encrypt)
{
    EVP_CIPHER_CTX *enc_ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit(enc_ctx, EVP_aes_256_cbc(), _KEY_AES, _IV_AES);

    int enc_length = len_msg_origin + (AES_BLOCK_SIZE - len_msg_origin % AES_BLOCK_SIZE);
    unsigned char *msg_encrypt = (unsigned char*)malloc(sizeof(unsigned char) * enc_length);
    //encrypt all the bytes up to but not including the last block
    if(!EVP_EncryptUpdate(enc_ctx, msg_encrypt, &enc_length, msg_origin, len_msg_origin)) {
        EVP_CIPHER_CTX_cleanup(enc_ctx);
        free(msg_encrypt);
        printf("EVP Error: couldn't update encryption with plain text!\n");
        return NULL;
    }
    *len_msg_encrypt = enc_length;
    //EncryptFinal will cipher the last block + Padding
    if(!EVP_EncryptFinal_ex(enc_ctx, msg_encrypt + enc_length, &enc_length)) {
        EVP_CIPHER_CTX_cleanup(enc_ctx);
        free(msg_encrypt);
        printf("EVP Error: couldn't finalize encryption!\n");
        return NULL;
    }
    *len_msg_encrypt += enc_length;
    //free context and return decrypted message
    EVP_CIPHER_CTX_cleanup(enc_ctx);
    return msg_encrypt;
}

unsigned char* decrypt(
    const unsigned char _KEY_AES[], 
    const unsigned char _IV_AES[], 
    unsigned char *msg_encrypt, 
    int len_msg_encrypt,
    int *len_msg_decrypt)
{
    EVP_CIPHER_CTX *dec_ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit(dec_ctx, EVP_aes_256_cbc(), _KEY_AES, _IV_AES);
    unsigned char *msg_decrypt = (unsigned char *)malloc(sizeof(unsigned char) * len_msg_encrypt);
    *len_msg_decrypt = len_msg_encrypt;
    int dec_length = 0;
    //same as above
    if(!EVP_DecryptUpdate(dec_ctx, msg_decrypt, &dec_length, msg_encrypt, len_msg_encrypt)) {
        EVP_CIPHER_CTX_cleanup(dec_ctx);
        free(msg_decrypt);
        printf("EVP Error: couldn't update decrypt with text!\n");
        return NULL;
    }

    len_msg_encrypt = dec_length;
    if(!EVP_DecryptFinal_ex(dec_ctx, msg_decrypt + len_msg_encrypt, &dec_length)) {
        EVP_CIPHER_CTX_cleanup(dec_ctx);
        free(msg_decrypt);
        printf("EVP Error: couldn't finalize decryption!\n");
        return NULL;
    }
    *len_msg_decrypt += dec_length;
    // msg_decrypt[*len_msg_decrypt] = '\0'; // // potential bugs if *len_msg_decrypt > len(msg_decrypt)
    //free context and return decrypted message
    EVP_CIPHER_CTX_cleanup(dec_ctx);
    return msg_decrypt;
}

/*
    Name: encrypt_file
    Function: This function encrypt file's content.
    Input: filename.
    Output: filename.
*/
void encrypt_file(
    string filename_in, 
    string filename_out)
{
    // open file
    ifstream f_in(filename_in);
    stringstream sstr;
    while(f_in >> sstr.rdbuf());
    string strgraph = sstr.str();
    f_in.close();
    // encrypt
    unsigned char *msg_origin = (unsigned char*)strgraph.c_str();
    int len_msg_encrypt = 0;
    const char *dataSent = strgraph.c_str();
    int length = strlen(dataSent);
    unsigned char *msg_encrypt = encrypt(_KEY_AES_SERVER, _IV_AES_SERVER, msg_origin, length, &len_msg_encrypt);
    // write file
    FILE *file_output = fopen(filename_out.c_str(), "w");
    fprintf(file_output, "%d\n%d\n", length, len_msg_encrypt);
    for (int i = 0; i < len_msg_encrypt; i++)
        fprintf(file_output, "%c", msg_encrypt[i]);
    fclose(file_output);
    free(msg_encrypt);
}

/*
    Name: decrypt_file
    Function: This function decrypt file's content.
    Input: filename.
    Output: filename.
*/
void decrypt_file(
    string filename_in, 
    string filename_out)
{
    // open file
    ifstream f_in(filename_in);
    stringstream sstr;
    int len_msg_origin = 0;
    int len_msg_encrypt = 0;
    f_in >> len_msg_origin;
    f_in >> len_msg_encrypt;
    while(f_in >> sstr.rdbuf());
    string strgraph = sstr.str();
    f_in.close();

    // decrypt
    unsigned char *msg_encrypt = (unsigned char*)strgraph.c_str();
    int len_msg_decrypt = 0;
    unsigned char *msg_decrypt = decrypt(_KEY_AES_SERVER, _IV_AES_SERVER, msg_encrypt, len_msg_encrypt, &len_msg_decrypt);
    // write file
    FILE *file_output = fopen(filename_out.c_str(), "w");
    for (int i = 0; i < len_msg_origin; i++)
        fprintf(file_output, "%c", msg_decrypt[i]);
    fclose(file_output);
    free(msg_decrypt);
}