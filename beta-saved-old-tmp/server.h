#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include "list.h"
#include "str.h"

// gcc -c server.c -o server.o -L. -llist -L. -lstr -lws2_32 && gcc -shared -o server.dll server.o -L. -llist -L. -lstr -lws2_32

#define BUFFER_SIZE 4096

typedef struct _server server;
typedef void (*Callback)(server *, str *response, list *headers, str *content_buffer);

struct _server_node{
    char* method;
    char* path;
    Callback callback;
    struct _server_node* next;
};

struct _server{
    struct _server_node *head;
    struct _server_node *tail;
    int length;
    // void (*print)(struct _str*); // print method
    void (*route)(struct _server*, char*, char*, Callback callback);
    int (*run)(struct _server*, char*, int, int);
    // char* (*get)(struct _list*, char*);
};

struct _server http_server();

int find(char *s, char *value);
int findchar(char *s, char value);
size_t get_file_size(FILE *fptr);
void send_file(str *response, char *filename);

// Function to convert a request string to an integer representation
int _server_request_to_int(const char *str);

void _server_route(struct _server *self, char *method, char *path, Callback callback);
int _server_run(struct _server *self, char *host, int port, int debug);

#endif
