#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int main(void)
{
    int msqid;      // IPC descriptor for the message queue
    char pathname[]="11-task2a.c"; // The file name used to generate the key.
    // A file with this name must exist in the current directory.
    key_t  key;     // IPC key
    int i,len;      // Cycle counter and the length of the informative part of the message
    char msg[81] = "This is text message";
    int maxlen = 81;

    struct mymsgbuf // Custom structure for the message
    {
        long mtype;
        char mtext[81];
    } mybuf;

    if ((key = ftok(pathname,0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }
    //
    // Trying to get access by key to the message queue, if it exists,
    // or create it, with read & write access for all users.
    //
    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    /* Send information */

    for (i = 0; i < 12; i++) {
        // отправляем/принимаем по 2 сообщения
        if (i % 4 >= 2) {
            // отправляем сообщение c типом 2
            mybuf.mtype = 2;
            strcpy(mybuf.mtext, msg);
            len = strlen(mybuf.mtext)+1;

            if (msgsnd(msqid,  (struct msgbuf *)&mybuf, len, 0) < 0) {
                printf("Can\'t send message to queue\n");
                msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                exit(-1);
            }
            printf("Program2 send message: '%s'\n", msg);
        } else {
            // принимаем сообщение с типом 1
            if (( len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, 1, 0)) < 0) {
                printf("Can\'t receive message from queue\n");
                exit(-1);
            }
            strcpy(msg, mybuf.mtext);
            printf("Program1 got message: '%s'\n", msg);
        }
    }

    return 0;
}
