#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


int main()
{
	int tmp, result;
	size_t size;
	char resstring[14];
	char name[] = "example.fifo";
	(void)umask(0);

	if (mknod(name, S_IFIFO | 0666, 0) < 0) {
		printf("Can not create FIFO\n");
		exit(-1);
	}

	printf("FIFO file is created\n");
	printf("Reading data from fifo\n");

	if ((tmp = open(name, O_RDONLY)) < 0) {
		printf("Can not open FIFO for reading\n");
		exit(-1);
	}

	size = read(tmp, resstring, 14);

	if (size < 0) {
		printf("Can not read string\n");
		exit(-1);
	}

	printf("Read from FIFO: %s\n", resstring);
	close(tmp);
	return 0;
}
