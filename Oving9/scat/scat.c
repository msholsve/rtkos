#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <fcntl.h>

#define WIDTH 256

int main(int argc, char *argv[])
{
	if (argc <= 2)
	{
		printf("Needs at least two arguments\n");
		return EXIT_FAILURE;
	}

	char *path = argv[1];
	int block = (argv[2][0] == '0') ? O_NONBLOCK : 0;
	int fd = open(path, O_RDWR | block);
	if (fd == -1)
	{
		printf("open failed with path %s\n", path);
		return EXIT_FAILURE;
	}

	char buffer[256];
	int nBytes;
	while ((nBytes = read(fd, buffer, WIDTH-1)) != 0)
	{
		if (nBytes == -1)
		{
			printf("Something fucked up happened\n");
			return EXIT_FAILURE;
		}
		buffer[nBytes] = '\0';
		printf("%s", buffer);
	}

	return EXIT_SUCCESS;
}
