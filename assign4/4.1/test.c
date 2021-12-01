#include <linux/unistd.h>
#include <sys/types.h>
#include <unistd.h>
	
int main(){
	syscall(336, getpid());
	return 0;
}
