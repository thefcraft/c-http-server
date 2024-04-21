#include <string.h>
#include <stdlib.h>
#include "server.h"

#define HOST "localhost"
#define PORT 5000
#define DEBUG 0
// In linux
// gcc -c server.c && ar rcs libserver.a server.o
// gcc demoApp.c -L. -lserver && ./a.out

// In windows
// rm -f libserver.a && gcc -c server.c -o server.o -lws2_32 && gcc -shared -o server.dll server.o -lws2_32
// gcc demoApp.c -L. -lserver && ./a.exe 


struct resistorcolor
{
    int first;
    int second;
    int multiplier;
    int tolerance;
};
struct resistorcolor getResistorcolorcode(int);

void home(server *self, str *response, list *headers, str *content_buffer){
    send_file(response, "templates/index.html");
}
void Resistorcolorcode(server *self, str *response, list *headers, str *content_buffer){
    send_file(response, "templates/Resistorcolorcode.html");
}
void ResistorcolorcodePOST(server *self, str *response, list *headers, str *content_buffer){
    char body[content_buffer->length];
    content_buffer->raw(content_buffer, body);
    int value;

    // Search for the "value" key and extract the integer following it
    if (sscanf(body, "{\"value\":%d}", &value) != 1) {
        // Error handling if sscanf fails
        printf("Error: Unable to extract value from JSON string.\n");
        response->append(response, "HTTP/1.1 400 ERROR\r\n\r\n");
        return;
    }
    struct resistorcolor result = getResistorcolorcode(value);

    printf("RESISTOR (ohm) : %d\n", value);

    response->append(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
    response->append_format(response, "{\"colors\": [%d, %d, %d]}", result.first, result.second, result.multiplier);
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
void stop_server(server *self, str *response, list *headers, str *content_buffer){
    self->stop(self);
    response->append(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
    response->append(response, "{\"stop\": true}");
}
int main(){
    server app = http_server();
    app.route(&app, "GET", "/", home);
    app.route(&app, "GET", "/stop", stop_server);
    app.route(&app, "GET", "/Resistorcolorcode", Resistorcolorcode);
    app.route(&app, "POST", "/Resistorcolorcode", ResistorcolorcodePOST);
    app.route(&app, "GET", "/favicon.ico", send_logo);
    app.route(&app, "GET", "/static/*", send_static);
    app.route(&app, "POST", "/api/", api);
    app.run(&app, HOST, PORT, DEBUG);
    return 0;
}


struct resistorcolor getResistorcolorcode(int R){   
    struct resistorcolor result;
    
    // getting the 1st digit
    int digit1 = R;
    while (digit1 >= 10)
    {
        digit1 /= 10;
    }
    int digit2 = (R / 10) % 10;
    int multiplier = 0;
    while (R >= 100)
    {
        R /= 10;
        multiplier++;
    }
    result.first = digit1;
    result.second = digit2;
    result.multiplier = multiplier;
    result.tolerance = 0;
    return result;
}