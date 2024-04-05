#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_file_size(FILE *fptr){
    fseek(fptr, 0L, SEEK_END);
    int file_size = ftell(fptr);
    rewind(fptr); // fseek(fptr, 0L, SEEK_SET);
    return file_size;
}

int main() {
    
    FILE *fptr;
    // Open a file in read mode
    // fptr = fopen("templates\\index.html", "r"); // may raise size error (\r\n for Windows, \n for Unix-like systems) so use rb mode
    fptr = fopen("templates\\index.html", "rb");
    // Print some text if the file does not exist
    if (fptr == NULL) {
        printf("Not able to open the file.\n");
        fclose(fptr);
        exit(EXIT_FAILURE);
    }
    
    int file_size = get_file_size(fptr);
    printf("File size: %d bytes\n", file_size);
    
    // Allocate memory to store file data
    unsigned char *file_data = (unsigned char *)malloc(file_size);
    if (file_data == NULL) {
        printf("Memory allocation failed.\n");
        fclose(fptr);
        free(file_data);
        exit(EXIT_FAILURE);
    }
    // Read the entire file into memory
    size_t bytes_read = fread(file_data, 1, file_size, fptr);
    if (bytes_read != file_size) {
        printf("Error reading file.\nSize: %d\tRead: %d\n", file_size, bytes_read);
        fclose(fptr);
        free(file_data);
        exit(EXIT_FAILURE);
    }
    // file_data[file_size] = '\0'; // Null-terminate the string // file_size + 1
    // printf("File data:\n%s\nLen: %d\n", file_data, strlen(file_data));
    for(int i = 0; i<file_size; i++) printf("%u ", file_data[i]); // prints a series of bytes
    
    // Close the file and free memory
    fclose(fptr);
    free(file_data);
    return 0;
}