#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int fd;
	char gpio_buffer[2];
	char choice;

	if (argc < 2)
	{
		printf("Enter correct number of argument\n");
		printf("Ex: ./a.out 1\n");
	}

	fd = open( "/dev/gpio_write", O_RDWR );

	printf( "Value of fd is: %d", fd );

	if( fd < 0 )
	{
		printf("Cannot open device \t");
		printf(" fd = %d \n",fd);
		return 0;
	}

	write( fd, argv[1], 1 );		
	printf("GPIO value is: %s \n", gpio_buffer );

	if( close(fd) != 0 )
	{
		printf("Could not close device\n");
	}

	return 0;
}
