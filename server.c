#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h> // Include ws2tcpip.h for inet_ntop
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

int find(char *s, char *value) {
    int i = 0;
    int pos = -1;
    while (s[i] != '\0' && pos == -1) {
        if (s[i] == value[0]) {
            int j = 0;
            while (value[j] != '\0') {
                if (value[j] != s[i + j]) break;
                j++;
            }
            if (value[j] == '\0') pos = i;
        }
        i++;
    }
    return pos;
}
int findchar(char *s, char value) {
    int i = 0;
    int pos = -1;
    while (s[i] != '\0' && pos == -1) {
        if (s[i] == value) pos = i;
        i++;
    }
    return pos;
}

// Function to convert a request string to an integer representation
int _server_request_to_int(const char *str) {
    // Example: Convert "GET" to 1, "POST" to 2, etc.
    if (strcmp(str, "GET") == 0) return 1;
    else if (strcmp(str, "POST") == 0) return 2;
    return 0; // Default case
}

void route_post(str *response, list *headers, str *data){
    int pos = findchar(headers->head->data->value, ' ');
    char url[pos+1];
    for (int i = 0; i < pos; i++) url[i] = headers->head->data->value[i];
    url[pos] = '\0';
    printf("URL: %s\n", url);
    char data_raw[data->length];
    data->raw(data, data_raw);

    if (strcmp(url, "/") == 0) {
        response->append(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
        response->append(response, "Hello world! POST<br>");
        response->append(response, data_raw);
        response->append(response, "<br>");
        response->append(response, headers->head->data->value);
    }
    else {
        response->append(response, "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n");
        response->append(response, "Not Fount");
    }
}

void _server_route(struct _server *self, char *method, char *path, Callback callback){
    struct _server_node *node = (struct _server_node*)malloc(sizeof(struct _server_node));
    // char* copied_method = (char*)malloc(sizeof(char)*(strlen(method) + 1)); // +1 for the null terminator
    // strcpy(copied_method, method);
    // char* copied_path = (char*)malloc(sizeof(char)*(strlen(path) + 1)); // +1 for the null terminator
    // strcpy(copied_path, path);
    node->callback = callback;
    node->method = method;
    node->path = path;
    if (self->head == NULL){
        self->head = node;
        self->tail = node;
    }else{
        self->tail->next = node;
        self->tail = node;
    }
    // printf("%s\n", node->path);
    self->length++;
}
int _server_run(struct _server *self, char *host, int port, int debug){
    WSADATA wsa;
    SOCKET server_sock, client_sock;
    struct sockaddr_in server, client;
    int client_len;
    char client_ip[INET_ADDRSTRLEN]; // Buffer to store client IP address
    char buffer[BUFFER_SIZE+1]; // Buffer to store received data
    
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Create socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        return 1;
    }
    
    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    
    // Bind
    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        return 1;
    }
    printf("Server listening on port %d...\n", port);

    // Listen
    listen(server_sock, 3);

    // Accept and handle connections
    while (1) {
        client_len = sizeof(client);
        client_sock = accept(server_sock, (struct sockaddr *)&client, &client_len);
        if (client_sock == INVALID_SOCKET) {
            printf("Accept failed\n");
            return 1;
        }

        // Print client address
        inet_ntop(AF_INET, &client.sin_addr, client_ip, INET_ADDRSTRLEN);
        // printf("Client connected from: %s:%d\n", client_ip, ntohs(client.sin_port));
        
        str header_buffer = String();
        str content_buffer = String();
        // Receive request header from client
        while (1) {
            int bytes_received = recv(client_sock, buffer, sizeof(buffer)-1, 0);
            if (bytes_received <= 0) break;
            buffer[bytes_received] = '\0'; // Null-terminate the received data
            // int nlpos = findchar(buffer, '\n');
            // while (nlpos != -1){
                // buffer[nlpos] = '\0';
                // printf("FIND: %s\n", buffer);
                // buffer[nlpos] = '\n';
                // break;
            // }
            // printf("`%s`\n", buffer);
            
            header_buffer.append(&header_buffer, buffer);
            // printf("LEN: %d\n", header_line_buffer.length);
        
            // char raw_header[16+1];
            // header_line_buffer.get_chunk(&header_line_buffer, 0, raw_header,16);
            // raw_header[16] = '\0';
            // printf("DATA 1: `%s`\n", raw_header);
            // Check if we've received the end of the header (double CRLF)
            char raw_header[header_buffer.length];
            header_buffer.raw(&header_buffer, raw_header);
            int pos = find(raw_header, "\r\n\r\n");
            if (pos!=-1) {
                // printf("We Found the end of the header %d\n", pos);
                // printf("Total length: %d\n", header_buffer.length);

                char content_tmp[BUFFER_SIZE+1];
                int idx=0;
                for(int i=pos+4; i<header_buffer.length; i++) {
                    content_tmp[idx] = raw_header[i];
                    idx++;
                }
                content_tmp[idx] = '\0';
                content_buffer.append(&content_buffer, content_tmp);

                
                raw_header[pos+4] = '\0';
                header_buffer.clear(&header_buffer);
                header_buffer.append(&header_buffer, raw_header);
                
                // printf("LEN: %d but total len: %d\n", strlen(raw_header), header_line_buffer.length);
                // header_line_buffer.print(&header_line_buffer);
                break;
            }
        }
        
        char raw_header[header_buffer.length];
        header_buffer.raw(&header_buffer, raw_header);

        list headers = List();
        // printf("\n------------------------------------\n%c", raw_header[0]);
        char tmp[header_buffer.length];
        int idx = 0;

        for (int i = 1; i < header_buffer.length-2; i++){
            tmp[idx] = raw_header[i-1];
            if (raw_header[i-1] == '\r' && raw_header[i] == '\n'){
                tmp[idx+1] = '\0';

                int pos = findchar(tmp, ' ');
                tmp[pos] = '\0';
                
                char value[idx-pos+1];
                for (int i = 0; i < idx-pos+1; i++) value[i] = tmp[pos+i+1];
                value[idx-pos-1]='\0'; // changing \r to \0

                char key[strlen(tmp)+1];
                strcpy(key, tmp);
                
                Header header;
                header.key = key;
                header.value = value;
                // printf("{%s : %s}\n", key, value);
                // printf("KEY: `%s`LEN: %d\n\n", key, strlen(key));
                // printf("VALUE: `%s`LEN: %d\n\n", value, strlen(value));

                headers.append(&headers, &header);
                
                tmp[pos] = ' ';
                
                idx = -1;
                i++;
            }
            idx++;
        }

        // printf("\n\nRequest header:\n");
        // headers.print(&headers);
        // header_buffer.print(&header_buffer);
        // printf("Request body:\n");
        // header_buffer.print(&content_buffer);
        
        str response = String();
        printf("%s - - %s \"%s\"\n", client_ip, headers.head->data->key, headers.head->data->value);
        int flag_not_found = 1;
        struct _server_node *node = self->head;
        
        int pos = findchar(headers.head->data->value, ' '); 
        // TODO split by ? and many more characters
        char url[pos+1];
        for (int i = 0; i < pos; i++) url[i] = headers.head->data->value[i];
        url[pos] = '\0';

        while (node!=NULL){
            if (strcmp(headers.head->data->key, node->method) == 0){
                if (strcmp(url, node->path) == 0){
                    if(strcmp(headers.head->data->key, "POST")==0){
                        int content_length = atoi(headers.get(&headers, "Content-Length:"));
                        while (content_length > content_buffer.length){
                            int bytes_received = recv(client_sock, buffer, min(sizeof(buffer)-1, (content_length - content_buffer.length)), 0);
                            if (bytes_received <= 0) break;
                            buffer[bytes_received] = '\0';
                            content_buffer.append(&content_buffer, buffer);
                        }
                    }
                    node->callback(self, &response, &headers, &content_buffer);
                    flag_not_found = 0;
                    break;
                }
            }
            node = node->next;
        }
        if(flag_not_found == 1) printf("\n\nERROR: NOT FOUND\n\n");
    
        // char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nHello world!";
        char response_raw[response.length];
        response.raw(&response, response_raw);
        // Send response
        send(client_sock, response_raw, strlen(response_raw), 0);
        
        // Close client socket
        closesocket(client_sock);
        headers.free(&headers);
        // line_buffer.free(&line_buffer);
        header_buffer.free(&header_buffer);
        content_buffer.free(&content_buffer);
    }
    
    // Close server socket
    closesocket(server_sock);
    WSACleanup();
    return 0;

}

struct _server http_server(){
    struct _server s;
    s.head = NULL;
    s.tail = NULL;
    s.length = 0;
    s.route = _server_route;
    s.run = _server_run;
    return s;
}

size_t get_file_size(FILE *fptr){
    fseek(fptr, 0L, SEEK_END);
    int file_size = ftell(fptr);
    rewind(fptr); // fseek(fptr, 0L, SEEK_SET);
    return file_size;
}
void send_file(str *response, char *filename){
    FILE *fptr;
    fptr = fopen(filename, "rb");
    if (fptr == NULL) {
        printf("Not able to open the file.\n");
        fclose(fptr);
        exit(EXIT_FAILURE);
    }
    size_t file_size = get_file_size(fptr);
    // Allocate memory to store file data
    char *file_data = (char *)malloc(file_size+1);
    if (file_data == NULL) {
        printf("Memory allocation failed.\n");
        fclose(fptr);
        free(file_data);
        exit(EXIT_FAILURE);
    }
    // Read the entire file into memory
    size_t bytes_read = fread(file_data, 1, file_size, fptr);
    if (bytes_read != file_size) {
        printf("Error reading file.\nSize: %d\tRead: %d\n", file_size, bytes_read);
        fclose(fptr);
        free(file_data);
        exit(EXIT_FAILURE);
    }
    file_data[file_size] = '\0'; // Null-terminate the string // file_size + 1
    response->append(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
    response->append(response, file_data);
}


// String()
void _str_data__repr__(char* data){
    printf("%s", data);
}

// Function of List
void _str_print(struct _str *self){
    // print method
	struct _str_chunk *node = self->head;
    if (node!=NULL) {
        while (node->next!=NULL){
            _str_data__repr__(node->data);
            node = node->next;
        }
        _str_data__repr__(node->data);
    }
    printf("\n");
}
void _str_append(struct _str *self, char *data){
    // Append object to the end of the list.
	struct _str_chunk *node = (struct _str_chunk*)malloc(sizeof(struct _str_chunk));
    char* copied_data = (char*)malloc(sizeof(char)*(strlen(data) + 1)); // +1 for the null terminator
    if (copied_data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    strcpy(copied_data, data);
    node->data = copied_data;
    node->next = NULL;
    if (self->head == NULL){
        self->head = node;
        self->tail = node;
    }else{
        self->tail->next = node;
        self->tail = node;
    }
    // printf("Size of `%s` is %d\n", data, strlen(data));
    self->length+=strlen(data);
}
void _str_append_char(struct _str *self, char ch){

}
void _str_raw(struct _str *self, char *output){
	struct _str_chunk *node = self->head;
    output[0] = '\0';
    int idx = 0;
    if (node!=NULL) {
        while (node->next!=NULL){
            int i=0;
            while (node->data[i]!='\0'){
                output[idx] = node->data[i];
                i++;
                idx++;
            }
            // strcat(output, node->data);
            node = node->next;
        }
        int i=0;
        while (node->data[i]!='\0'){
            output[idx] = node->data[i];
            i++;
            idx++;
        }
        output[idx] = '\0';
        // strcat(output, node->data);
    }
    // strcat(output, '\0');
}
void _str_clear(struct _str *self){
    struct _str_chunk *node = self->head;
    self->length = 0;
    if (node == NULL) return;
    while (node->next!=NULL){
        struct _str_chunk *next = node->next;
        free(node->data);
        free(node);
        node = next;
    }
    free(node->data);
    free(node);
    self->head = NULL;
    self->tail = NULL;
}
void _str_get_chunk(struct _str *self, int index, char *output, int length){
    struct _str_chunk *node = self->head;
    
    output[0] = '\0';

    if (node == NULL){
        printf("ERROR: _str_get_chunk node is NULL maybe size is smaller than index");
        return;
    }
    for (int i = 0; i < index; i++){
        node = node->next;
        if (node == NULL){
            printf("ERROR: _str_get_chunk node is NULL maybe size is smaller than index");
            return;
        }
    }
    
    for (int i = 0; i < length; i++) output[i] = node->data[i];
}

// Function to initialize a test structure with default values
struct _str String() {
    struct _str s;
    s.head = NULL;
    s.tail = NULL;
    s.length = 0;
    s.print = _str_print;
    s.append = _str_append;
    s.free = _str_clear;
    s.clear = _str_clear;
    s.raw = _str_raw;
    s.get_chunk = _str_get_chunk;
    return s;
}

// List()

void _list_data__repr__(struct _list_data *data){
    printf("{ %s : %s }", data->key, data->value);
}

struct _list_data * _list_data_copy(struct _list_data *data){
    struct _list_data * copied_data = (struct _list_data *)malloc(sizeof(struct _list_data));
    char* copied_data_key = (char*)malloc(sizeof(char)*(strlen(data->key) + 1)); // +1 for the null terminator
    char* copied_data_value = (char*)malloc(sizeof(char)*(strlen(data->value) + 1)); // +1 for the null terminator
    strcpy(copied_data_key, data->key);
    strcpy(copied_data_value, data->value);
    copied_data->key = copied_data_key;
    copied_data->value = copied_data_value;
    return copied_data;
}
void _list_data_free(struct _list_data *data){
    free(data->value);
    free(data->key);
};

char* _list_get_header(struct _list* self, char* key){
	struct _list_node *node = self->head;
    if (node!=NULL) {
        while (strcmp(node->data->key, key) && node->next!=NULL) node = node->next;
    }
    else printf("ERROR: Invalid get key\n");
    return node->data->value;
}


// Function of List
void _list_print(struct _list *self) {
    // print method
	struct _list_node *node = self->head;
    printf("[\n\t");
    if (node!=NULL) {
        while (node->next!=NULL){
            _list_data__repr__(node->data);
            printf(", \n\t");
            // printf("%d, ", node->data);
            node = node->next;
        }
        _list_data__repr__(node->data);
        printf("\n]\n");
        // printf("%d]\n", node->data);
    }
    else printf("]\n");
}
void _list_append(struct _list *self, struct _list_data *data){
    // Append object to the end of the list.
	struct _list_node *node = (struct _list_node*)malloc(sizeof(struct _list_node));
    node->data = _list_data_copy(data);
    node->next = NULL;
    if (self->head == NULL){
        self->head = node;
        self->tail = node;// TODO;
    }else{
        self->tail->next = node;
        self->tail = node;
    }
    self->length++;

    // printf("DATA: %d ", self->head->data);
    // printf("LEN: %d\n", self->length);
}
void _list_remove(struct _list *self, struct _list_data *data){
    // Remove first occurrence of value.
    // Raises ValueError if the value is not present.
}
void _list_index(struct _list *self, struct _list_data *data){
    // Return first index of value. -1 if the value is not present.
}
void _list_free(struct _list *self){
    struct _list_node *node = self->head;
    self->length = 0;
    if (node == NULL) return;
    while (node->next!=NULL){
        struct _list_node *next = node->next;
        _list_data_free(node->data);
        free(node->data);
        free(node);
        node = next;
    }
    free(node);
    self->head = NULL;
    self->tail = NULL;
}

// Function to initialize a test structure with default values
struct _list List() {
    struct _list l;
    l.head = NULL;
    l.tail = NULL;
    l.length = 0;
    l.print = _list_print;
    l.append = _list_append;
    l.free = _list_free;
    l.get = _list_get_header;
    return l;
}