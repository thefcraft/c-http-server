#include <string.h>
#include "server.h"

#define HOST "localhost"
#define PORT 8080
#define DEBUG 0

void home(server *self, str *response, list *headers, str *content_buffer){
    // int pos = findchar(headers->head->data->value, ' ')+1;
    // char other_data[strlen(headers->head->data->value)-pos+1];
    // for (int i = pos; i < strlen(headers->head->data->value); i++) other_data[i-pos] = headers->head->data->value[i];
    // other_data[strlen(headers->head->data->value)-pos] = '\0';
    // printf("other_data: \"%s\"\n", other_data);
    send_file(response, "templates\\index.html");
}
void login(server *self, str *response, list *headers, str *content_buffer){
    response->append(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
    response->append(response, "Hello World");
}
void api(server *self, str *response, list *headers, str *content_buffer){
    char body[content_buffer->length];
    content_buffer->raw(content_buffer, body);
    printf("%s\n", body);
    headers->print(headers);

    response->append(response, "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n");
    response->append(response, "Not Fount");
}

int main(){
    server app = http_server();
    app.route(&app, "GET", "/", home);
    app.route(&app, "POST", "/api", api);
    app.route(&app, "GET", "/login", login);
    app.run(&app, HOST, PORT, DEBUG);
    return 0;
}