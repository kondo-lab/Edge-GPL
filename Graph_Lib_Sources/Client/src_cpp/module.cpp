/* version 3 */
#include "module.h"

/*
    Name: get_command, get_vertex, get_filename, get_hostname
          get_edge_weight, get_clustering_threshold, get_graph_format
    Function: Gets the appropriate user input data.
    Input: Buffer to hold the user input.
*/
STATUS get_command(COMMAND &cmd)
{
    try {
        cout << "[INFO] AVAILABLE COMMANDS" << endl;
        cout << "\t" << "(" << CMD_ADD_VERTEX << ")  Add vertex" << endl;
        cout << "\t" << "(" << CMD_REMOVE_VERTEX << ")  Remove vertex" << endl;
        cout << "\t" << "(" << CMD_NUM_VERTEX << ")  Get number of vertices" << endl;
        cout << "\t" << "(" << CMD_ADD_EDGE << ")  Add edge" << endl;
        cout << "\t" << "(" << CMD_REMOVE_EDGE << ")  Remove edge" << endl;
        cout << "\t" << "(" << CMD_NUM_EDGE << ")  Get number of edges" << endl;
        cout << "\t" << "(" << CMD_EDGE_IN_VERTEX << ")  Get in-edges of vertex" << endl;
        cout << "\t" << "(" << CMD_EDGE_OUT_VERTEX << ")  Get out-edges of vertex" << endl;
        cout << "\t" << "(" << CMD_CLEAR_ALL_GRAPH << ")  Clear graph" << endl;
        cout << "\t" << "(" << CMD_IMPORT_GRAPH << ") Import graph" << endl;
        cout << "\t" << "(" << CMD_EXPORT_GRAPH << ") Export graph" << endl;
        cout << "\t" << "(" << CMD_PRINT_GRAPH << ") Print graph" << endl;
        cout << "\t" << "(" << CMD_LIST_GRAPH_FORMATS << ") List graph formats" << endl;
        cout << "\t" << "(" << CMD_CURRENT_FORMAT << ") Get current graph format" << endl;
        cout << "\t" << "(" << CMD_CHANGE_GRAPH_FORMAT << ") Change graph format" << endl;
        cout << "\t" << "(" << CMD_BFS << ") Run Breadth-First Search" << endl;
        cout << "\t" << "(" << CMD_DFS << ") Run Depth-First Search" << endl;
        cout << "\t" << "(" << CMD_SPS << ") Run Shortest Paths" << endl;
        cout << "\t" << "(" << CMD_CENTRALITY_ANALYSIS << ") Run Centrality Analysis" << endl;
        cout << "\t" << "(" << CMD_CLUSTERING << ") Run Clustering" << endl;
        cout << "\t" << "(" << CMD_DATA_CONCAT << ") Run Data Concatenation" << endl;
        cout << "\t" << "(" << CMD_QUIT_BACKEND << ") Quit Backend" << endl;
        cout << "\t" << "(" << CMD_GET_STATUS << ") Get Server Status" << endl;
        cout << "\t" << "(" << CMD_RESET << ") Reset Server" << endl;
        cout << "\t" << "(" << CMD_GRAPH_PARTITION << ") Run Graph Partition" << endl;
        cout << "\t" << "(" << CMD_LOAD_SUB_GRAPH << ") Load Sub Graph" << endl;
        cout << "\t" << "(" << CMD_SPS_ALL_PATH << ") Run Shortest Paths with All Paths" << endl;
        cout << "\t" << "(" << CMD_SUBGRAPH_INFO << ") Get Subgraph Information" << endl;
        cout << "\t" << "(" << CMD_EVALUATE_PATH << ") Evaluate Local Shortest Path" << endl;
        cout << "\t" << "(" << CMD_REGISTER_PATH << ") Register Local Shortest Path" << endl;
        cout << "\t" << "(" << CMD_REMOVE_PATH << ") Remove Local Shortest Path" << endl;
        cout << "\t" << "(" << CMD_EXIT << ")  Exit Client" << endl;

        while (true) {
            string strcmd;
            cout << "\t" << "Enter your command: ";
            getline(std::cin, strcmd);
            bool valid = true;
            for (int  i = 0; i < strcmd.length(); i++)
                if (strcmd[i] < '0' || strcmd[i] > '9') {
                    PRINT_STATUS("Invalid command. Please try again.");
                    valid = false;
                    break;
                }
            if (valid) {
                int command = stoi(strcmd);
                if (command < CMD_EXIT || command > CMD_REMOVE_PATH) {
                    PRINT_STATUS("Invalid command. Please try again.");
                    valid = false;
                } else
                    cmd = (COMMAND) command;
            }
            if (valid)
                break;
        }
    } catch (...) { return FAIL; }
    return SUCCESS;
}
STATUS get_vertex(string &vertex)
{
    try {
        cout << "\t" << "Enter vertex name: ";
        getline(std::cin, vertex);
    } catch (...) { return FAIL; }
    return SUCCESS;
}
STATUS get_filename(string &filename)
{
    try {
        cout << "\t" << "Enter file name: ";
        getline(std::cin, filename);
    } catch (...) { return FAIL; }
    return SUCCESS;
}
STATUS get_hostname(string &hostname)
{
    try {
        cout << "\t" << "Enter host ip address: ";
        getline(std::cin, hostname);
    } catch (...) { return FAIL; }
    return SUCCESS;
}
STATUS get_edge_weight(int &weight)
{
    try {
        string strweight;
        while (true) {
            cout << "\t" << "Enter edge weight: ";
            getline(std::cin, strweight);

            bool valid = true;
            for (int i = 0; i < strweight.length(); i++)
                if (strweight[i] < '0' || strweight[i] > '9') {
                    PRINT_STATUS("Invalid weight. Please try again.");
                    valid = false;
                    break;
                }
            if (valid)
                break;
        }
        weight = stoi(strweight);
    } catch (...) { return FAIL; }
    return SUCCESS;
}

STATUS get_num_parts(int& num_parts)
{
    try {
        string str;
        while (true) {
            cout << "\t" << "Enter a number of parts: ";
            getline(std::cin, str);

            bool valid = true;
            for (int i = 0; i < str.length(); i++)
                if (str[i] < '0' || str[i] > '9') {
                    PRINT_STATUS("Invalid number. Please try again.");
                    valid = false;
                    break;
                }
            if (valid)
                break;
        }
        num_parts = stoi(str);
    }
    catch (...) { return FAIL; }
    return SUCCESS;
}

STATUS get_cut_type(int& cut_type)
{
    try {
        string str;
        while (true) {
            cout << "\t" << "Enter a graph-cut type (1-edge cut, 2-vertex cut): ";
            getline(std::cin, str);

            bool valid = true;
            for (int i = 0; i < str.length(); i++)
                if (str[i] < '0' || str[i] > '9') {
                    PRINT_STATUS("Invalid number. Please try again.");
                    valid = false;
                    break;
                }
            if (valid)
                break;
        }
        cut_type = stoi(str);
    }
    catch (...) { return FAIL; }
    return SUCCESS;
}

STATUS get_global_path(string& global_path)
{
    try {
        cout << "\t" << "Enter global shortest path: ";
        getline(std::cin, global_path);
    }
    catch (...) { return FAIL; }
    return SUCCESS;
}

STATUS get_local_path(string& local_path)
{
    try {
        cout << "\t" << "Enter local shortest path: ";
        getline(std::cin, local_path);
    }
    catch (...) { return FAIL; }
    return SUCCESS;
}

STATUS get_sg_fname(string& sg_fname)
{
    try {
        cout << "\t" << "Enter subgraph filename: ";
        getline(std::cin, sg_fname);
    }
    catch (...) { return FAIL; }
    return SUCCESS;
}

STATUS get_clustering_threshold(double &threshold)
{
    try {
        string strthreshold;
        while (true) {
            cout << "\t" << "Enter threshold: ";
            getline(std::cin, strthreshold);

            bool valid = true;
            for (int i = 0; i < strthreshold.length(); i++)
                if ((strthreshold[i] < '0' || strthreshold[i] > '9')
                    && (strthreshold[i] != '.'))
                {
                    PRINT_STATUS("Invalid threshold. Please try again.");
                    valid = false;
                    break;
                }
            if (valid)
                break;
        }
        threshold = stod(strthreshold);
    } catch (...) { return FAIL; }
    return SUCCESS;
}
STATUS get_graph_format(int &format)
{
    try {
        string strformat;
        while (true) {
            cout << "\t" << "Enter format: ";
            getline(std::cin, strformat);

            bool valid = true;
            for (int i = 0; i < strformat.length(); i++)
                if ((strformat[i] < '0' || strformat[i] > '9')
                    && (strformat[i] != '.'))
                {
                    PRINT_STATUS("Invalid format. Please try again.");
                    valid = false;
                    break;
                }
            if (valid)
                break;
        }
        format = stoi(strformat);
    } catch (...) { return FAIL; }
    return SUCCESS;
}
STATUS get_encryption(bool &is_encryption)
{
    is_encryption = false;
    try {
        string isEncryption;
        cout << "\t" << "Is encryption(Y/N, Default = N):";
        getline(std::cin, isEncryption);
        bool valid = true;
        if (isEncryption.compare("Y") == 0 || isEncryption.compare("y") == 0)
            is_encryption = true;
        else if(isEncryption.compare("N") == 0 || isEncryption.compare("n") == 0)
            is_encryption = false;
    } catch (...) { return FAIL; }
    return SUCCESS;
}


namespace Client {
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
    Name: create_socket_client
    Function: Creates a client-side socket and connects the socket
        to the server with the given address information.
    Input: Host IP address and port.
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

const string strCmdName[] = {
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


int MAX_BUFFER_SIZE = 1024;
/*
    Name: socket_send
    Function: Sends a data (string type) to the server.
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
    // encrypt data
#ifdef ENCRYPTION_ONLINE
    // encrypt data
    unsigned char *msg_encrypt = encrypt(_KEY_AES_CLIENT, _IV_AES_CLIENT, msg_origin, length, &len_msg_encrypt);
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
    Function: Receives a data (string type) from the server.
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
    unsigned char *msg_decrypt = decrypt(_KEY_AES_CLIENT, _IV_AES_CLIENT, msg_encrypt, len_msg_encrypt, &len_msg_decrypt);
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
    // msg_decrypt[*len_msg_decrypt] = '\0'; // potential bugs if *len_msg_decrypt > len(msg_decrypt)
    //free context and return decrypted message
    EVP_CIPHER_CTX_cleanup(dec_ctx);
    return msg_decrypt;
}
}