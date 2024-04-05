#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "str.h"
#include "server.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h> // Include ws2tcpip.h for inet_ntop
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib
// gcc 4.requestHandler.c -L. -llist -L. -lstr -lws2_32 && ./a.exe

typedef void (*Callback)(struct _server *);

typedef struct _server{
    // struct _str_chunk *head;
    // struct _str_chunk *tail;
    // int length;
    // void (*print)(struct _str*); // print method
    // void (*append)(struct _str*, char*); // Append object to the end of the string.
    void (*route)(struct _server*, char*, char*, Callback callback);
    void (*run)(struct _server*, char*, int, int);
    // char* (*get)(struct _list*, char*);
} server;

int find(char *s, char *value);
int findchar(char *s, char value);
int _server_request_to_int(const char *str);
size_t get_file_size(FILE *fptr);
void send_file(str *response, char *filename);
void _server_route(struct _server *self, char *method, char *path, Callback callback);
void _server_run(struct _server *self, char *host, int port, int debug);
struct _server http_server();

#define BUFFER_SIZE 64 //4096

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

void route_get(str *response, list *headers){
    int pos = findchar(headers->head->data->value, ' ');
    char url[pos+1];
    for (int i = 0; i < pos; i++) url[i] = headers->head->data->value[i];
    url[pos] = '\0';
    printf("URL: %s\n", url);
    if (strcmp(url, "/") == 0) {
        // response->append(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
        // response->append(response, "Hello World");
        send_file(response, "templates\\index.html");
    }
    else {
        response->append(response, "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n");
        response->append(response, "Not Fount");
    }
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

void _server_route(struct _server *self, char *method, char *path, Callback Callback){
    
}
void _server_run(struct _server *self, char *host, int port, int debug){
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
        printf("Client connected from: %s:%d\n", client_ip, ntohs(client.sin_port));
        
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
        switch (_server_request_to_int(headers.head->data->key)){
            case 1: // GET
                printf("%s - - GET \"%s\"\n", client_ip, headers.head->data->value);
                route_get(&response, &headers);
                break;
            case 2: // POST
                printf("%s - - POST \"%s\"\n", client_ip, headers.head->data->value);

                int content_length = atoi(headers.get(&headers, "Content-Length:"));
                while (content_length > content_buffer.length){
                    int bytes_received = recv(client_sock, buffer, min(sizeof(buffer)-1, (content_length - content_buffer.length)), 0);
                    if (bytes_received <= 0) break;
                    buffer[bytes_received] = '\0';
                    content_buffer.append(&content_buffer, buffer);
                }
                
                // printf("\n\nRequest header:\n");
                // headers.print(&headers);
                // printf("Content-Length: %d\n", content_length);
                // printf("Request body:\n");
                // content_buffer.print(&content_buffer);
                // "Content-Length:"
                route_post(&response, &headers, &content_buffer);
                break;
            default:
                printf("%s - - UKN \"%s\"\n", client_ip, headers.head->data->value);
                break;
        }
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
    s.route = _server_route;
    s.run = _server_run;
    return s;
}

#define HOST "localhost"
#define PORT 8080
#define DEBUG 0

void home(server *self){

}

int main(){
    server app = http_server();
    app.route(&app, "GET", "/", home);
    app.run(&app, HOST, PORT, DEBUG);
    return 0;
}