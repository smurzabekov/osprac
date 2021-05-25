#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

void addOne(char* s, int index) {
    s[index] += 1;
    if (s[index] > 'z') {
        s[index] = 'a';
        addOne(s, index - 1);
    }
}

int main()
{
    int     fd;
    char    fileName[] = "aa";
    char    symlinkName[] = "test/aa";
    (void) umask(0);

    // создаём директорию под файлы
    if (mkdir("test", 0777) < 0) {
        printf("Unable to create dir 'test'. If it exists, please delete it.\n");
        exit(-1);
    }

    // создаём начальный файл
    if ((fd = open("test/aa", O_WRONLY | O_CREAT, 0666)) < 0) {
        printf("Can\'t open file\n");
        exit(-1);
    }
    if (close(fd) < 0) {
        printf("Can\'t close file\n");
    }

    int maxRecursion = 0;
    while (1) {
        addOne(symlinkName, 6);
        if (symlink(fileName, symlinkName) != 0) {
            printf("Unable to create symlink\n");
            exit(-1);
        }

        // проверяем, можно ли всё ещё открыть файл
        if ((fd = open(symlinkName, O_WRONLY, 0666)) < 0) {
            // если нельзя, значит мы достигли максимального уровня рекурсии
            break;
        } else {
            maxRecursion++;
        }

        if (close(fd) < 0) {
            printf("Can\'t close file\n");
        }

        addOne(fileName, 1);
    }

    printf("maxRecursion: %d\n", maxRecursion);


    return 0;
}
