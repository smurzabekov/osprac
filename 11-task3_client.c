#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    pid_t pid;
    int msqid;      // IPC descriptor for the message queue
    char pathname[]="11-task3-server.c"; // The file name used to generate the key.
    // A file with this name must exist in the current directory.
    key_t  key;     // IPC key
    int i,len;      // Cycle counter and the length of the informative part of the message
    char msg[81] = "This is text message";
    int maxlen = 81;

    /**
     * Принимаемые сообщения
     *
     * mtype = наш номер pid
     * num = результат с сервера
     */
    struct mymsgbuf
    {
        long mtype;
        int num;
    } mybuf;

    /**
     * Отправляемые сообщения
     * mtype = 1
     * pid = наш pid
     * num = запрос к сервер (число с клавиатуры)
     */
    struct mysndmsgbuf
    {
        long mtype;
        int pid;
        int num;
    } mysndbuf;

    if ((key = ftok(pathname,0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    pid  = getpid();

    mysndbuf.mtype = 1;
    mysndbuf.pid = pid;
    printf("Type a number: ");
    if (scanf("%d", &mysndbuf.num) != 1) {
        printf("Unable to read number from keyboard\n");
        exit(-1);
    }

    if (msgsnd(msqid,  (struct msgbuf *)&mysndbuf, sizeof(int) * 2, 0) < 0) {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }
    printf("Client %d send message to server: '%d'\n", mysndbuf.pid, mysndbuf.num);

    // принимаем сообщение с типом pid
    if (( len = msgrcv(msqid, (struct msgbuf *) &mybuf, sizeof(int), mysndbuf.pid, 0)) < 0) {
        printf("Can\'t receive message from queue\n");
        exit(-1);
    }

    printf("Client %d got message: '%d'\n", mysndbuf.pid, mybuf.num);

    return 0;
}
