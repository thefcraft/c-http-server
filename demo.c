#include <string.h>
#include <stdlib.h>
#include "server.h"

#define HOST "0.0.0.0"
#define PORT 8080
#define DEBUG 1
// gcc demo.c -L. -lserver && ./a.exe 
void home(server *self, str *response, list *headers, str *content_buffer){
    send_file(response, "templates/index.html");
}
void api(server *self, str *response, list *headers, str *content_buffer){
    char body[content_buffer->length];
    content_buffer->raw(content_buffer, body);
    printf("%s\n", body);
    headers->print(headers);

    response->append(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
    response->append(response, "{\"files\": [0, 1, 2, 3, 4, 5]}");
}
void send_static(server *self, str *response, list *headers, str *content_buffer){
    int pos = findchar(headers->head->data->value, ' '); 
    char url[pos];
    for (int i = 1; i < pos; i++) url[i-1] = headers->head->data->value[i];
    url[pos-1] = '\0';
    send_file(response, url);   
}
void send_logo(server *self, str *response, list *headers, str *content_buffer){
    str header_buffer = String();
    header_buffer.append(&header_buffer, "HTTP/1.1 200 OK\r\n");
    header_buffer.append(&header_buffer, "Accept-Ranges: bytes\r\n");
    FILE *fptr;
    fptr = fopen("static/favicon.ico", "rb");
    if (fptr == NULL) {
        response->append(response, "HTTP/1.1 404 NOT FOUND\r\n\r\n");
        response->append(response, "ERROR : 404 NOT FOUND");
        fclose(fptr);
        return;
    }
    size_t file_size = get_file_size(fptr);
    // header_buffer.append(&header_buffer, "Content-Length: 50000\r\n");
    header_buffer.append(&header_buffer, "Content-Type: image/x-icon\r\n\r\n");
    char raw_header[header_buffer.length];
    header_buffer.raw(&header_buffer, raw_header);
    header_buffer.free(&header_buffer);
    send_file_with_header(response, "static/favicon.ico", raw_header);   
    fclose(fptr);
}
int main(){
    server app = http_server();
    app.route(&app, "GET", "/", home);
    app.route(&app, "GET", "/favicon.ico", send_logo);
    app.route(&app, "GET", "/static/*", send_static);
    app.route(&app, "POST", "/api/", api);
    app.run(&app, HOST, PORT, DEBUG);
    return 0;
}