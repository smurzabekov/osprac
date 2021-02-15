#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{

//void reverse(char* str, size_t sz);


    int pipe_parent[2], pipe_child[2], result;
    size_t size;
    char resstring[14];

    if (pipe(pipe_parent) < 0) {
        printf("Error during creation parent pipe\n");
        exit(-1);
    }

    if (pipe(pipe_child) < 0) {
        printf("Error during creation child pipe\n");
        exit(-1);
    }

    result = fork();
    if (result < 0) {
        printf("Error during creation child fork\n");
        exit(-1);
    } else if (result > 0) {
        close(pipe_parent[0]);
        close(pipe_child[1]);
        size = write(pipe_parent[1], "Hello, world!", 14);
        if (size != 14) {
            printf("Error during writing all string\n");
            exit(-1);
        }
        close(pipe_parent[1]);

        size = read(pipe_child[0], resstring, 14);
        if (size != 14) {
            printf("Error during reading from child\n\n");
            exit(-1);
        }
        printf("Parent took from child: %s\n", resstring);
        printf("Parent off\n");
    } else {
        close(pipe_parent[1]);
        close(pipe_child[0]);
        size = read(pipe_parent[0], resstring, 14);
        if (size < 0) {
            printf("Error during getting from parent\n");
            exit(-1);
        }
        printf("Got from parent: %s\n", resstring);
        reverse(resstring, 13);
        size = write(pipe_child[1], resstring, 14);
        close(pipe_parent[0]);
        printf("Child exits\n");
    }
}

void reverse(char* str, size_t sz){
    for (int i = 0; i < sz / 2; i++) {
        char t = str[i];
        str[i] = str[sz - i - 1];
        str[sz - i - 1] = t;
    }}

