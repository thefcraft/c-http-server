#include <stdio.h>
#include "str.h"
// -L. -lstr

int main(){
    str header_raw = String();
    header_raw.append(&header_raw, "GET / HTTP/1.1\r\n");
    header_raw.append(&header_raw, "Host: 127.0.0.1:");
    header_raw.append(&header_raw, "8080\r\n\r\n");
    header_raw.print(&header_raw);

    printf("%s", header_raw.head->data);
    return 0;
}