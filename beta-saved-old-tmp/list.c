#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void _list_data__repr__(struct _list_data *data){
    printf("{ %s : %s }", data->key, data->value);
}

struct _list_data * _list_data_copy(struct _list_data *data){
    struct _list_data * copied_data = (struct _list_data *)malloc(sizeof(struct _list_data));
    char* copied_data_key = (char*)malloc(sizeof(char)*(strlen(data->key) + 1)); // +1 for the null terminator
    char* copied_data_value = (char*)malloc(sizeof(char)*(strlen(data->value) + 1)); // +1 for the null terminator
    strcpy(copied_data_key, data->key);
    strcpy(copied_data_value, data->value);
    copied_data->key = copied_data_key;
    copied_data->value = copied_data_value;
    return copied_data;
}
void _list_data_free(struct _list_data *data){
    free(data->value);
    free(data->key);
};

char* _list_get_header(struct _list* self, char* key){
	struct _list_node *node = self->head;
    if (node!=NULL) {
        while (strcmp(node->data->key, key) && node->next!=NULL) node = node->next;
    }
    else printf("ERROR: Invalid get key\n");
    return node->data->value;
}


// Function of List
void _list_print(struct _list *self) {
    // print method
	struct _list_node *node = self->head;
    printf("[\n\t");
    if (node!=NULL) {
        while (node->next!=NULL){
            _list_data__repr__(node->data);
            printf(", \n\t");
            // printf("%d, ", node->data);
            node = node->next;
        }
        _list_data__repr__(node->data);
        printf("\n]\n");
        // printf("%d]\n", node->data);
    }
    else printf("]\n");
}
void _list_append(struct _list *self, struct _list_data *data){
    // Append object to the end of the list.
	struct _list_node *node = (struct _list_node*)malloc(sizeof(struct _list_node));
    node->data = _list_data_copy(data);
    node->next = NULL;
    if (self->head == NULL){
        self->head = node;
        self->tail = node;// TODO;
    }else{
        self->tail->next = node;
        self->tail = node;
    }
    self->length++;

    // printf("DATA: %d ", self->head->data);
    // printf("LEN: %d\n", self->length);
}
void _list_remove(struct _list *self, struct _list_data *data){
    // Remove first occurrence of value.
    // Raises ValueError if the value is not present.
}
void _list_index(struct _list *self, struct _list_data *data){
    // Return first index of value. -1 if the value is not present.
}
void _list_free(struct _list *self){
    struct _list_node *node = self->head;
    self->length = 0;
    if (node == NULL) return;
    while (node->next!=NULL){
        struct _list_node *next = node->next;
        _list_data_free(node->data);
        free(node->data);
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
    l.get = _list_get_header;
    return l;
}