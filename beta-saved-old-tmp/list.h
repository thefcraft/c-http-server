#ifndef LIST_H
#define LIST_H
// gcc -c list.c -o list.o
// gcc -shared -o list.dll list.o
// gcc -c list.c -o list.o && gcc -shared -o list.dll list.o
// gcc server.c -L. -llist && ./a.exe 
typedef struct _list_data{
    char *key;
    char *value;
} Header;

struct _list_node{
    struct _list_data *data;
    struct _list_node *next;
};

typedef struct _list{
    struct _list_node *head;
    struct _list_node *tail;
    int length;
    void (*print)(struct _list*); // print method
    void (*append)(struct _list*, struct _list_data*); // Append object to the end of the list.
    void (*free)(struct _list*); // Free the list.
    char* (*get)(struct _list*, char*);
} list;

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
