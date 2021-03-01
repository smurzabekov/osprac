#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	int tmp, result; 
	size_t size; 
	char resstring[14]; 
	char name[]="example.fifo";
	(void)umask(0);

	printf("WRITING data to FIFO\n");

	if((tmp = open(name, O_WRONLY)) < 0){
		printf("Can not open FIFO for writing\n");
		exit(-1);
	}

	size = write(tmp, "TESTING WRITE TO FIFO FILE", 14);
	printf("I wrote to fifo\n");
	if(size != 14) {
		printf("Can not write all string to FIFO\n");
		exit(-1);
	}

	close(tmp);
	return 0;
}
