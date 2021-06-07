#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


/*
 * Протокол связи:
 *  - перед началом передачи послылают друг другу сигналы SIGUSR1 -- проверка готовности к передаче
 *  - приёмник через SIGUSR2 сообщает о готовности к приёму, передатчик пересылает очередной бит числа
 *
 */


void sendBit(pid_t pid, char bit) {
    int signal;
    if (bit == 0) {
        signal = SIGUSR1;
    } else {
        signal = SIGUSR2;
    }

    if (kill(pid, signal) < 0) {
        printf("Unable to send signal\n");
        exit(-1);
    }
}

char connected = 0;
char lastBit = 1;
char received = 0;

void my_handler(int nsig) {
    char bit;
    if (nsig == SIGUSR1) {
        bit = 0;
    } else if (nsig == SIGUSR2) {
        bit = 1;
    }

    if (!connected && bit == 0) {
        connected = 1;
        return;
    }

    if (connected && !received) {
        lastBit = bit;
        received = 1;
        return;
    }

    printf("Got unexpected signal\n");
}

int main(void) {
    // устанавливаем обработчики сигналов
    (void)signal(SIGUSR1, my_handler);
    (void)signal(SIGUSR2, my_handler);

    pid_t my_pid = getpid();
    pid_t other_pid;
    printf("Receiver pid: %d\n", my_pid);
    printf("Please input transmitter pid: ");

    if (scanf("%d", &other_pid) != 1) {
        printf("Incorrect number\n");
        return -1;
    }


    // сообщаем о готовности к передаче
    sendBit(other_pid, 0);

    // ждём подключения приёмника
    printf("Waiting for transmitter\n");
    while(!connected);

    printf("Receiving message\n");

    int message = 0;
    for (int i = 0; i < 32; i++) {
        // сообщаем, что готовы к приёму очередного сигнала
        received = 0;
        sendBit(other_pid, 1);

        // получаем очередной бит
        while (!received);
        message |= (lastBit << i);
    }

    printf("Got message: %d\n", message);

    return 0;
}
