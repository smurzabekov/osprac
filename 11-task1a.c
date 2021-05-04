#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LAST_MESSAGE 255 // Message type for termination of program 11-1b.c

int main(void)
{
    int msqid;      // IPC descriptor for the message queue
    char pathname[]="11-task1a.c"; // The file name used to generate the key.
    // A file with this name must exist in the current directory.
    key_t  key;     // IPC key
    int i,len;      // Cycle counter and the length of the informative part of the message

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
    //
    // Trying to get access by key to the message queue, if it exists,
    // or create it, with read & write access for all users.
    //
    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    /* Send information */

    for (i = 1; i <= 5; i++) {
        if (i % 2 == 0) {
            // формируем буффер со строковым сообщением
            mybuf.mtype = 1;
            strcpy(mybuf.info.mmsg1.mtext, "This is text message");

            len = strlen(mybuf.info.mmsg1.mtext)+1;
        } else {
            // формируем буффер с вторым с типом ссообщения
            mybuf.mtype = 2;
            mybuf.info.mmsg2.mchar = (char)('a' + i);
            mybuf.info.mmsg2.mfloat = 100.f / (float)i;
            len = sizeof(mybuf.info.mmsg2);
        }


        //
        // Send the message. If there is an error,
        // report it and delete the message queue from the system.
        //
        if (msgsnd(msqid,  (struct msgbuf *)&mybuf, len, 0) < 0) {
            printf("Can\'t send message to queue\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(-1);
        }
    }

    /* Send the last message */

    mybuf.mtype = LAST_MESSAGE;
    len         = 0;

    if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0) {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    return 0;
}
