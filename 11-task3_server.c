#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


int main(int argc, char **argv)
{
    char needCheckAnotherServer = 1;
    if (argc == 2 && strcmp(argv[1], "-skip") == 0) {
        needCheckAnotherServer = 0;
    } else if (argc != 1) {
        printf("Got incorrect arguments\n");
        exit(-1);
    }

    int msqid;      // IPC descriptor for the message queue
    char pathname[]="11-task3-server.c"; // The file name used to generate the key.
    // A file with this name must exist in the current directory.
    key_t  key;     // IPC key
    int i,len;      // Cycle counter and the length of the informative part of the message

    /**
     * Типы принимаемых сообщений:
     * 1, pid, num -- запрос от клиента
     * 2 -- сообщение от киллера, что нужно остановить сервер
     * 3 -- сообщение, что сейчас не запущен сервер
     */
    struct mymsgbuf // Custom structure for the message
    {
        long mtype;
        int pid;
        int num;
    } mybuf;

    /**
     * Отправляемые сообщения
     * mtype = pid клиента
     * num = результат
     *
     * mtype = 3 -- сообщение, что сервер остановлен и может быть запущен другой сервер
     */
    struct mysndmsgbuf
    {
        long mtype;
        int num;
    } mysndbuf;


    if ((key = ftok(pathname,0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT | IPC_EXCL)) < 0) {
        if (errno != EEXIST) {
            printf("Can\'t get msqid\n");
            exit(-1);
        } else if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
            printf("Can\'t get msqid\n");
            exit(-1);
        }
        // пытаемся забрать сообщение о том, что сейчас сервер не запущен
        if ((len = msgrcv(msqid, (struct msgbuf *) &mybuf, 0, 3, IPC_NOWAIT)) < 0) {
            if (errno == ENOMSG) {
                if (needCheckAnotherServer) {
                    printf("Server already running or was killed incorrectly. "
                           "Run server with -skip argument if you sure, that there is no another"
                           "instance.\n");
                    exit(-1);
                }

            } else {
                printf("Can\'t receive message from queue\n");
                exit(-1);
            }
        }
    }

    while (1) {
        // принимаем сообщения с типами 1-2
        if (( len = msgrcv(msqid, (struct msgbuf *) &mybuf, sizeof(int) * 2, -2, 0)) < 0) {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }
        printf("Server got message: msgtype=%ld\n", mybuf.mtype);

        if (mybuf.mtype == 1) {
            // отправляем сообщение c типом pid
            mysndbuf.mtype = mybuf.pid;
            mysndbuf.num = mybuf.num * mybuf.num;
            if (msgsnd(msqid,  (struct msgbuf *)&mysndbuf, sizeof(int), 0) < 0) {
                printf("Can\'t send message to queue\n");
                msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                exit(-1);
            }
            printf("Server answered to client %d : %d\n", mysndbuf.mtype, mysndbuf.num);
        } else if (mybuf.mtype == 2) {
            printf("Stopping server\n");
            break;
        }
    }

    // сообщаем, что сервер остановлен
    mysndbuf.mtype = 3;
    if (msgsnd(msqid, (struct msgbuf *)&mysndbuf, 0, 0) < 0) {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    printf("Server stopped");


    return 0;
}
