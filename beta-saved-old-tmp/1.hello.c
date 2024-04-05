#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h> // Include ws2tcpip.h for inet_ntop
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib
// gcc 1.hello.c -lws2_32 && ./a.exe

#define PORT 8080

int main() {
    WSADATA wsa;
    SOCKET server_sock, client_sock;
    struct sockaddr_in server, client;
    int client_len;
    char client_ip[INET_ADDRSTRLEN]; // Buffer to store client IP address
    char buffer[4096]; // Buffer to store received data

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
    server.sin_port = htons(PORT);
    
    // Bind
    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        return 1;
    }
    printf("Server listening on port %d...\n", PORT);

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
    
        // Receive request header from client
        while (1) {
            int bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0'; // Null-terminate the received data
                printf("Request header:\n%s\n", buffer);
            }
            else break;
            // Check if we've received the end of the header (double CRLF)
            if (strstr(buffer, "\r\n\r\n") != NULL) {
                break;
            }
        }

        FILE *fptr;
        // Open a file in read mode
        fptr = fopen("templates\\index.html", "r");
        // Print some text if the file does not exist
        if(fptr == NULL) {
            printf("Not able to open the file.");
            exit(-1);
        }
        fseek(fptr, 0L, SEEK_END);
        int file_size = ftell(fptr);
        fseek(fptr, 0L, SEEK_SET); //rewind(fp);

        char file_data[file_size];

        fgets(file_data, file_size, fptr);

        printf("\n\n\nFILE DATA\n%s\n", file_data);

        char header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

        char response[file_size+strlen(header)];
        strcpy(response, header);
        strcat(response, file_data);  // concatenation is done here
        for (int i = 0; i < sizeof(response)/sizeof(response[0]); i++){
            printf("%c", response[i]);
        }
        

        // Send response
        send(client_sock, response, strlen(response), 0);

        // Close the file
        fclose(fptr);

        // Close client socket
        closesocket(client_sock);
    }

    // Close server socket
    closesocket(server_sock);
    WSACleanup();
    return 0;
}