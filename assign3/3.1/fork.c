#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define MAX_PROCESSES 8

int main(){

	FILE *f_read;
	char buf[5];
	char left_num = 0;
	char rigth_num = 0;
	int status1, status2, final_sum;

	//Related Time 	
	struct timespec begin, end;	
	long time;

	//Initialize several process
	pid_t pid_leaf[MAX_PROCESSES/2];
	pid_t pid_sub[MAX_PROCESSES/4];
	//File Open
	if(!(f_read =  fopen("./temp.txt", "r+"))){
		printf("FILE OPEN ERROR\n");
	}
	
	//fseek(f_read, 0, SEEK_END);

	//Time check : start
	clock_gettime(CLOCK_MONOTONIC, &begin);
	
	//cal sub process call : root
	pid_t pid = fork();
	if(pid == 0){
	
	//cal sub process call : degree 1
	for(int i = 0 ; i < (MAX_PROCESSES / 4) ; i ++){
		pid_sub[i] = fork();
		if(pid_sub[i] == 0){
		 	
			//cal sub process call : leaf
			for( int j = 0; j < (MAX_PROCESSES / 4) ; j ++){
				pid_leaf[j] = fork();
				
				//if child process
				if(pid_leaf[j] == 0){
					
					//Exception processing
					while(!fgets(buf, 5, f_read)){
						//printf("READ ERROR\n");
					}
					
					//zero index is valid data
					// ** file have (valid data + \n) **
					left_num = buf[0];

					while(!fgets(buf, 5, f_read)){
						//printf("READ ERROR\n");
					}
					rigth_num = buf[0];
					
					// char -> int
					// 96 = 48(char 0) * 2	
					exit(left_num + rigth_num - 96);
				}else{//if all child processes are exit, parent process start process
					if(j == 1){
						waitpid(pid_leaf[0], &status1, 0);
						waitpid(pid_leaf[1], &status2, 0);
						exit((status1 >> 8) + (status2 >> 8));
					}
				}
			}
		}else{
			if(i == 1){
				waitpid(pid_sub[0], &status1, 0);
				waitpid(pid_sub[1], &status2, 0);
				exit((status1 >> 8) + (status2 >> 8));
			}
		} 
	}
	}else{
		waitpid(pid, &final_sum, 0);
		printf("value of fork : %d\n", final_sum >> 8);
	}
	//Time check : end
	clock_gettime(CLOCK_MONOTONIC, &end);
	time = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec);
	printf("%lf\n", (double)time/1000000000);
	fclose(f_read);
	return 0;
}	
