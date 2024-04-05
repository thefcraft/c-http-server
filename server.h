#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>

// gcc -c server.c -o server.o -lws2_32 && gcc -shared -o server.dll server.o -lws2_32

#define BUFFER_SIZE 4096

typedef struct _server server;
typedef struct _str str;
typedef struct _list list;
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

// String()
struct _str_chunk{
    char *data;
    struct _str_chunk *next;
};

struct _str{
    struct _str_chunk *head;
    struct _str_chunk *tail;
    int length;
    void (*print)(struct _str*); // print method
    void (*append)(struct _str*, char*); // Append object to the end of the string.
    void (*free)(struct _str*); // Free the string.
    void (*clear)(struct _str*); // Clear the string
    void (*raw)(struct _str*, char*); // copy the string to the buffer
    void (*get_chunk)(struct _str*, int, char*, int);
};

void _str_data__repr__(char* );
void _str_print(struct _str *);
void _str_append(struct _str *, char *);
void _str_raw(struct _str *, char *);
void _str_clear(struct _str *);
void _str_get_chunk(struct _str *, int , char *, int );
struct _str String();

// List()
typedef struct _list_data{
    char *key;
    char *value;
} Header;

struct _list_node{
    struct _list_data *data;
    struct _list_node *next;
};

struct _list{
    struct _list_node *head;
    struct _list_node *tail;
    int length;
    void (*print)(struct _list*); // print method
    void (*append)(struct _list*, struct _list_data*); // Append object to the end of the list.
    void (*free)(struct _list*); // Free the list.
    char* (*get)(struct _list*, char*);
};

// custom functions
char* _list_get_header(struct _list*, char*);

// utility functions
void _list_data__repr__(struct _list_data*);
struct _list_data * _list_data_copy(struct _list_data *);
void _list_data_free(struct _list_data *);

void _list_print(struct _list*);
void _list_append(struct _list*, struct _list_data*);
void _list_remove(struct _list*, struct _list_data*);
void _list_index(struct _list*, struct _list_data*);
void _list_free(struct _list*);
struct _list List();

#endif
