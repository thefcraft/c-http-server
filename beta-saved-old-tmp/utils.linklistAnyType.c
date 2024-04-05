#include <stdio.h>
#include <stdlib.h>

struct _list_node{
    int data;
    struct _list_node *next;
};

typedef struct _list{
    struct _list_node *head;
    struct _list_node *tail;
    int length;
    void (*print)(struct _list*); // print method
    void (*append)(struct _list*, int); // Append object to the end of the list.
    void (*free)(struct _list*); // Free the list.
} list;

// Function of List
void _list_print(struct _list *self) {
    // print method
	struct _list_node *node = self->head;
    printf("[");
    if (node!=NULL) {
        while (node->next!=NULL){
            printf("%d, ", node->data);
            node = node->next;
        }
        printf("%d]", node->data);
    }
    else printf("]\n");
}
void _list_append(struct _list *self, int data){
    // Append object to the end of the list.
	struct _list_node *node = (struct _list_node*)malloc(sizeof(struct _list_node));
    node->data = data;
    node->next = NULL;
    if (self->head == NULL){
        self->head = node;
        self->tail = self->head;
    }else{
        self->tail->next = node;
        self->tail = node;
    }
    self->length++;

    // printf("DATA: %d ", self->head->data);
    // printf("LEN: %d\n", self->length);
}
void _list_remove(struct _list *self, int data){
    // Remove first occurrence of value.
    // Raises ValueError if the value is not present.
}
void _list_index(struct _list *self, int data){
    // Return first index of value. -1 if the value is not present.
}
void _list_free(struct _list *self){
    struct _list_node *node = self->head;
    if (node == NULL) return;
    while (node->next!=NULL){
        struct _list_node *next = node->next;
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
    return l;
}

int main() {
    list headers = List();
    headers.append(&headers, 1);
    headers.append(&headers, 2);
    headers.append(&headers, 3);
    headers.append(&headers, 4);
    headers.print(&headers);
    headers.print(&headers);

    // Allocate memory for an integer
    size_t size = sizeof(int);
    void *ptr_int = malloc(size);
    if (ptr_int == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    // Use the allocated memory
    *(int*)ptr_int = 42;
    // *(float*)ptr_float = 3.14f;
    printf("Integer value: %d\n", *(int*)ptr_int);

    free(ptr_int);

    headers.free(&headers);
    return 0;
}
