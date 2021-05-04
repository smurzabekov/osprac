#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LAST_MESSAGE 255

int main(void)
{
    struct sss {
        struct {
            int x;
            float y;
        };
    } ss;

    ss.x = 123;
    ss.y = 123.0f;
    ss.x = 124;

    int msqid;
    char pathname[]="11-task1a.c";
    key_t  key;
    int len, maxlen;

    struct mymsgbuf // Custom structure for the message
    {
        long mtype;
        union {
            struct {
                char mtext[81];
            } mmsg1;
            struct {
                float mfloat;
                char mchar;
            } mmsg2;
        } info;
    } mybuf;

    if ((key = ftok(pathname,0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    while (1) {
        //
        // In an infinite loop, accept messages of any type in FIFO order
        // with a maximum length of 81 characters
        // until a message of type LAST_MESSAGE is received.
        //
        maxlen = 81;

        if (( len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, 0, 0)) < 0) {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }

        if (mybuf.mtype == 1) {
            printf("message type = %ld, info = %s\n", mybuf.mtype, mybuf.info.mmsg1.mtext);
        } else if (mybuf.mtype == 2) {
            printf("message type = %ld, info.mchar = '%c', info.mfloat=%f\n",
                   mybuf.mtype, mybuf.info.mmsg2.mchar,
                   mybuf.info.mmsg2.mfloat);
        }

        //
        // If the received message is of type LAST_MESSAGE,
        // then terminate and remove the message queue from the system.
        // Otherwise, print the text of the received message.
        //
        if (mybuf.mtype == LAST_MESSAGE) {
            msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
            exit(0);
        }

    }

    return 0;
}
