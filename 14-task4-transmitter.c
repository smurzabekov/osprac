#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


/*
 * Протокол связи:
 *  - перед началом передачи послылают друг другу сигналы SIGUSR1 -- проверка готовности к передаче
 *  - передатчик пересылает очердной бит числа, приёмник через SIGUSR2 сообщает о приёме
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
char need_to_send = 0;

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

    if (!need_to_send && bit == 1) {
        need_to_send = 1;
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

    printf("Transmitter pid: %d\n", my_pid);
    printf("Please input receiver pid: ");

    if (scanf("%d", &other_pid) != 1) {
        printf("Incorrect number\n");
        return -1;
    }

    int message;
    printf("Please input number to transmit: ");
    if (scanf("%d", &message) != 1) {
        printf("Incorrect number\n");
        return -1;
    }

    // сообщаем о готовности к передаче
    sendBit(other_pid, 0);

    // ждём подключения приёмника
    printf("Waiting for receiver\n");
    while(!connected);

    printf("Transmitting message\n");
    for (int i = 0; i < 32; i++) {
        // ждём готовности к приёму
        while (!need_to_send);

        // отправляем
        need_to_send = 0;
        sendBit(other_pid, (message >> i) & 1);
    }

    printf("Transmission finished\n");

    return 0;
}

