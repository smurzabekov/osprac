#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


int main(void)
{
    int msqid;      // IPC descriptor for the message queue
    char pathname[]="11-task3-server.c"; // The file name used to generate the key.
    // A file with this name must exist in the current directory.
    key_t  key;     // IPC key
    int i,len;      // Cycle counter and the length of the informative part of the message

    /**
     * Отправляемые сообщения
     * mtype = 3 -- сообщение для остановки сервера
     */
    struct mysndmsgbuf
    {
        long mtype;
    } mysndbuf;

    if ((key = ftok(pathname,0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get msqid \n");
        exit(-1);
    }


    // сообщаем, что сервер необходимо остановить
    mysndbuf.mtype = 2;
    if (msgsnd(msqid, (struct msgbuf *)&mysndbuf, 0, 0) < 0) {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    printf("Send stop message");


    return 0;
}
