#ifndef STR_H
#define STR_H
// gcc -c str.c -o str.o && gcc -shared -o str.dll str.o
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
    void (*get_chunk)(struct _str*, int, char*, int);
} str;

void _str_data__repr__(char* );
void _str_print(struct _str *);
void _str_append(struct _str *, char *);
void _str_raw(struct _str *, char *);
void _str_clear(struct _str *);
void _str_get_chunk(struct _str *, int , char *, int );
struct _str String();

#endif
