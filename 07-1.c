#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main()
{
    int *array;
    int shmid;
    char pathname[] = "07-1.c";
    key_t key;
    char *source_code;
    
    int file_size = 0;
    int *file_size_ptr;
    
    if ((key = ftok(pathname,0)) < 0) {
        printf("memory can't be created\n");
        exit(-1);
    }
    
    if ((shmid = shmget(key, sizeof(int) + file_size * sizeof(char), 0)) < 0) {
        printf("memory can't be created\n");
        exit(-1);
    }
    if ((file_size_ptr = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)) {
        printf("memory can't be created\n");
        exit(-1);
    }
    file_size = *file_size_ptr;
    source_code = (char*)(file_size_ptr + 1);
    
    for (int i = 0; i < file_size; i++)
        putchar(source_code[i]);
    
    if (shmdt(file_size_ptr) < 0) {
        printf("memory can't be created\n");
        exit(-1);
    }
    
    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        printf("memory can't be created\n");
        exit(-1);
    }
    return 0;
}
