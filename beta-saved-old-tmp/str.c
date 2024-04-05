#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "str.h"

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
    char* copied_data = (char*)malloc(sizeof(char)*(strlen(data) + 1)); // +1 for the null terminator
    if (copied_data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    strcpy(copied_data, data);
    node->data = copied_data;
    node->next = NULL;
    if (self->head == NULL){
        self->head = node;
        self->tail = node;
    }else{
        self->tail->next = node;
        self->tail = node;
    }
    // printf("Size of `%s` is %d\n", data, strlen(data));
    self->length+=strlen(data);
}
void _str_append_char(struct _str *self, char ch){

}
void _str_raw(struct _str *self, char *output){
	struct _str_chunk *node = self->head;
    output[0] = '\0';
    int idx = 0;
    if (node!=NULL) {
        while (node->next!=NULL){
            int i=0;
            while (node->data[i]!='\0'){
                output[idx] = node->data[i];
                i++;
                idx++;
            }
            // strcat(output, node->data);
            node = node->next;
        }
        int i=0;
        while (node->data[i]!='\0'){
            output[idx] = node->data[i];
            i++;
            idx++;
        }
        output[idx] = '\0';
        // strcat(output, node->data);
    }
    // strcat(output, '\0');
}
void _str_clear(struct _str *self){
    struct _str_chunk *node = self->head;
    self->length = 0;
    if (node == NULL) return;
    while (node->next!=NULL){
        struct _str_chunk *next = node->next;
        free(node->data);
        free(node);
        node = next;
    }
    free(node->data);
    free(node);
    self->head = NULL;
    self->tail = NULL;
}
void _str_get_chunk(struct _str *self, int index, char *output, int length){
    struct _str_chunk *node = self->head;
    
    output[0] = '\0';

    if (node == NULL){
        printf("ERROR: _str_get_chunk node is NULL maybe size is smaller than index");
        return;
    }
    for (int i = 0; i < index; i++){
        node = node->next;
        if (node == NULL){
            printf("ERROR: _str_get_chunk node is NULL maybe size is smaller than index");
            return;
        }
    }
    
    for (int i = 0; i < length; i++) output[i] = node->data[i];
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
    s.get_chunk = _str_get_chunk;
    return s;
}

// int main(){
//     str Name = String();
//     Name.append(&Name, "This is a test ");
//     Name.append(&Name, "and i know how to fuck you");
//     printf("Size: %d\n", Name.length);
//     Name.print(&Name);
//     char raw[Name.length];
//     Name.raw(&Name, raw);
//     printf("%s\n", raw);
//     Name.clear(&Name);
//     Name.append(&Name, "This is a test ");
//     Name.append(&Name, "and i know how to fuck you");
//     printf("Size: %d\n", Name.length);
//     Name.print(&Name);
//     Name.raw(&Name, raw);
//     printf("%s\n", raw);

//     Name.free(&Name);
//     return 0;
// }
