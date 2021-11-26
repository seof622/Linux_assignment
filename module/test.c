#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(int argc, char *argv[]){
	
	int fd;
	int ret;
	int i = 0;
	ssize_t read_size;
	char *temp = "Change text\n";
	char buf[1024] = {0};
	//open
	syscall(336, getpid());
	if(0 < (fd = open("./test.txt", O_RDWR | O_CREAT | O_APPEND, 0644))){
	//write, lseek, read, close		 
		write(fd, temp, strlen(temp));
		lseek(fd, 0, SEEK_SET);
		read_size = read(fd, buf, 1024);
		printf("%s size : %d\n",buf, read_size);  
		close(fd);
	}else{
		printf("Open Error\n");
	}
	//ftrace system call
	
	return 0;
}
