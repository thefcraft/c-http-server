## Run

### in windows

To complile the server.h

`rm -f libserver.a && gcc -c server.c -o server.o -lws2_32 && gcc -shared -o server.dll server.o -lws2_32`

To run the server

`gcc demo.c -L. -lserver && ./a.exe`

### in linux

To complile the server.h

`gcc -c server.c && ar rcs libserver.a server.o`

To run the server

`gcc demo.c -L. -lserver && ./a.out`
