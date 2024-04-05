## Run

To complile the server.h

`gcc -c server.c -o server.o -lws2_32 && gcc -shared -o server.dll server.o -lws2_32`

To run the server

`gcc demo.c -L. -lserver && ./a.exe`
