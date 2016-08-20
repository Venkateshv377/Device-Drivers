#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
	int fd; 
	char gpio_buffer[2];

	fd = open( "/dev/gpio_read", O_RDWR );

	printf( "Value of fd is: %d", fd );

	if( fd < 0 ) 
	{
		printf("Cannot open device \t");
		printf(" fd = %d \n",fd);
		return 0;
	}

	read( fd, &gpio_buffer, 1); 
	printf("GPIO value is: %s \n", gpio_buffer );

	if( close(fd) != 0 ) 
	{
		printf("Could not close device\n");
	}

	return 0;
}
