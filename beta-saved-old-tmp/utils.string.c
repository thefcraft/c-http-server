#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct _str_chunk{
    char *data;
    struct _str_chunk *next;
};

typedef struct _str{
    struct _str_chunk *head;
    struct _str_chunk *tail;
    int length;
    void (*print)(struct _str*); // print method
    void (*append)(struct _str*, char*); // Append object to the end of the string.
    void (*free)(struct _str*); // Free the string.
    void (*clear)(struct _str*); // Clear the string
    void (*raw)(struct _str*, char*); // copy the string to the buffer
} str;

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
    node->data = data;
    node->next = NULL;
    if (self->head == NULL){
        self->head = node;
        self->tail = self->head;
    }else{
        self->tail->next = node;
        self->tail = node;
    }
    // printf("Size of `%s` is %d\n", data, strlen(data));
    self->length+=strlen(data);
}
void _str_raw(struct _str *self, char *output){
	struct _str_chunk *node = self->head;
    output[0] = '\0';
    if (node!=NULL) {
        while (node->next!=NULL){
            strcat(output, node->data);
            node = node->next;
        }
        strcat(output, node->data);
    }
    // strcat(output, '\0');
}
void _str_clear(struct _str *self){
    struct _str_chunk *node = self->head;
    self->length = 0;
    if (node == NULL) return;
    while (node->next!=NULL){
        struct _str_chunk *next = node->next;
        free(node);
        node = next;
    }
    free(node);
    self->head = NULL;
    self->tail = NULL;
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
    return s;
}

int main(){
    str header_raw = String();
    header_raw.append(&header_raw, "GET / HTTP/1.1\r\n");
    header_raw.append(&header_raw, "Host: 127.0.0.1:");
    header_raw.append(&header_raw, "8080\r\n\r\n");
    header_raw.print(&header_raw);

    printf("%s", header_raw.head->data);
    return 0;
}
int main2(){
    str Name = String();
    Name.append(&Name, "This is a test ");
    Name.append(&Name, "and i know how to fuck you");
    printf("Size: %d\n", Name.length);
    Name.print(&Name);
    char raw[Name.length];
    Name.raw(&Name, raw);
    printf("%s\n", raw);
    Name.clear(&Name);
    Name.append(&Name, "This is a test ");
    Name.append(&Name, "and i know how to fuck you");
    printf("Size: %d\n", Name.length);
    Name.print(&Name);
    Name.raw(&Name, raw);
    printf("%s\n", raw);

    Name.free(&Name);
    return 0;
}
