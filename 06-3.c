#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>

int main() {
    int tmp[2];
    size_t size = 1;
    char* resstring = malloc(1 * sizeof(char));
    int result = 0;

    if(pipe(tmp) < 0) {
        printf("Can\'t create pipe\n");
        exit(-1);
    }

    fcntl(tmp[1], F_SETFL, fcntl(tmp[1], F_GETFL) | O_NONBLOCK);
    
    while (size == 1) {
        size = write(tmp[1], resstring, 1);
        result++;
    }
    printf("Result of task: %d bytes\n", result);

    return 0;
}
