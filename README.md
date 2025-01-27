# Basic HTTP Server in C

A lightweight and efficient HTTP server implementation in C that supports basic routing, static file serving, and both GET and POST requests. This server is designed for development and learning purposes.

## Features

- Simple and clean API inspired by modern web frameworks
- Support for both GET and POST requests
- Static file serving capability
- Route patterns with wildcard support (`/path/*`)
- Cross-platform compatibility (Windows and Linux)
- Built-in debugging mode
- Memory-efficient string and list implementations
- Support for serving various content types (HTML, JSON, images, etc.)
- Basic request and response handling

## Prerequisites

### Windows
- GCC compiler (MinGW)
- ws2_32 library (Windows Sockets 2)

### Linux
- GCC compiler
- Standard C libraries

## Building the Library

### Windows
```bash
rm -f libserver.a && gcc -c server.c -o server.o -lws2_32 && gcc -shared -o server.dll server.o -lws2_32
```

### Linux
```bash
gcc -c server.c && ar rcs libserver.a server.o
```
## Compilation and Running

### Windows
```bash
gcc demo.c -L. -lserver && ./a.exe
```

### Linux
```bash
gcc demo.c -L. -lserver && ./a.out
```

## Usage Example

Here's a basic example showing how to create a simple web server:

```c
#include <string.h>
#include <stdlib.h>
#include "server.h"

#define HOST "0.0.0.0"
#define PORT 8080
#define DEBUG 1

// Route handler for home page
void home(server *self, str *response, list *headers, str *content_buffer) {
    send_file(response, "templates/demo.html");
}

// Route handler for API endpoint
void api(server *self, str *response, list *headers, str *content_buffer) {
    response->append(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
    response->append(response, "{\"status\": \"success\"}");
}

int main() {
    server app = http_server();
    
    // Define routes
    app.route(&app, "GET", "/", home);
    app.route(&app, "POST", "/api", api);
    
    // Start the server
    app.run(&app, HOST, PORT, DEBUG);
    return 0;
}
```

## API Reference

### Server Functions

- `http_server()`: Creates a new server instance
- `server.route(server*, method, path, callback)`: Registers a new route
- `server.run(server*, host, port, debug)`: Starts the server
- `server.stop(server*)`: Stops the server gracefully

### Utility Functions

- `send_file(str *response, char *filename)`: Sends a file as response
- `send_file_with_header(str *response, char *filename, char *header)`: Sends a file with custom headers

### String and List Operations

The server includes custom implementations for string and list operations:
- `String()`: Creates a new string buffer
- `List()`: Creates a new list for header storage

## Route Patterns

The server supports two types of route patterns:
1. Exact matches: `/path`
2. Wildcard patterns: `/path/*`

## Debug Mode

When debug mode is enabled (`DEBUG=1`), the server will output:
- Client connection details
- Request information
- Response status codes
- Error messages

## Limitations

- Development server only - not recommended for production use
- Basic HTTP/1.1 implementation
- Limited security features
- No SSL/TLS support
- Single-threaded operation

## Contributing

Feel free to submit issues, fork the repository, and create pull requests for any improvements.

## Acknowledgments

This server implementation is designed for educational purposes and as a learning tool for understanding HTTP servers and network programming in C.

