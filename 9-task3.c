#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


void A(int s, int n) {
    struct sembuf mybuf;
    mybuf.sem_num = 0;
    mybuf.sem_op  = n;
    mybuf.sem_flg = 0;
    if (semop(s, &mybuf, 1) < 0) {
        printf("Can\'t A to semaphore\n");
        exit(-1);
    }
}

void D(int s, int n) {
    struct sembuf mybuf;
    mybuf.sem_num = 0;
    mybuf.sem_op  = -n;
    mybuf.sem_flg = 0;
    if (semop(s, &mybuf, 1) < 0) {
        printf("Can\'t D to semaphore\n");
        exit(-1);
    }
}

void Z(int s) {
    struct sembuf mybuf;
    mybuf.sem_num = 0;
    mybuf.sem_op  = 0;
    mybuf.sem_flg = 0;
    if (semop(s, &mybuf, 1) < 0) {
        printf("Can\'t Z to semaphore\n");
        exit(-1);
    }
}

int main()
{
 //изначально семафор S инициализирован 0
    printf("N: ");

    int n;
    if (scanf("%d", &n) != 1) {
        printf("Failed to read n");
        exit(-1);
    }

    if (n < 2) {
        printf("n should be >= 2");
        exit(-1);
    }



    int fd[2], result;

    size_t size;
    char  resstring[14] = "Hello, world!";

    if (pipe(fd) < 0) {
        printf("Can\'t open pipe\n");
        exit(-1);
    }


    int semid;
    // чтобы данный семафор был только у ребенка и родителя (а также при каждом запуске создавался новый),
    // используем IPC_PRIVATE
    if ((semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t create semaphore set\n");
        exit(-1);
    }
    result = fork();

    if (result < 0) {
        printf("Can\'t fork child\n");
        exit(-1);
    } else if (result > 0) {
    //родитель запускает ребенка, ребенок выполняет D(S, 1)
        // родительский процесс
        for (int i = 0; i < n; i++) {
            size = write(fd[1], resstring, 14);
            if (size != 14) {
                printf("Can\'t write all string to pipe\n");
                exit(-1);
            }
            printf("Parent write: %s\n", resstring);

    //родитель пишет в pipe данные, сообщает ребенку об этом через A(S, 2) и блокируется
    //через Z(S).
            A(semid, 2);
            Z(semid);

            size = read(fd[0], resstring, 14);
            if (size < 0) {
                printf("Parent: Can\'t read string from pipe\n");
                exit(-1);
            }
            printf("Parent read: %s\n", resstring);
        }
     //родитель запускается (из-за Z(S)), читает полученные от ребенка данные,
        //и весь процесс повторяется снова (система находится в состоянии между шагами 2 и 3)

        if (close(fd[0]) < 0) {
            printf("parent: Can\'t close reading side of pipe\n"); exit(-1);
        }
        if (close(fd[1]) < 0) {
            printf("parent: Can\'t close writing side of pipe\n"); exit(-1);
        }

        printf("Parent exit\n");

    } else {

        //Дочерний процесс
        //ребенок запускается(в результате значение S становится равно 1),
        //читает данные из pipe, пишет данные, и дальше выполняет D(S, 1) чтобы запустить родителя
        //и ещё D(S, 1) чтобы заблокироваться до следующего раза
        for (int i = 0; i < n; i++) {
            D(semid, 1);

            size = read(fd[0], resstring, 14);
            if (size < 0) {
                printf("Child: Can\'t read string from pipe\n");
                exit(-1);
            }
            printf("Child read: %s\n", resstring);

            // чтобы не было скучно, будем к первому символу добавлять по 1 (и когда дойдём до 'Z', переходить в 'A')
            resstring[0] = (char)(((resstring[0] - 'A') + 1) % ('Z' - 'A' + 1) + 'A');

            size = write(fd[1], resstring, 14);
            if (size != 14) {
                printf("Child: Can\'t write all string to pipe\n");
                exit(-1);
            }
            printf("Child write: %s\n", resstring);

            D(semid, 1);
        }


        if (close(fd[0]) < 0) {
            printf("child: Can\'t close reading side of pipe\n"); exit(-1);
        }
        if (close(fd[1]) < 0) {
            printf("child: Can\'t close writing side of pipe\n"); exit(-1);
        }
        printf("Child exit\n");
    }

    return 0;
}
