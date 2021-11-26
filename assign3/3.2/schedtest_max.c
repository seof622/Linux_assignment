#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <sys/syscall.h>
#include <time.h>

#define MAX_PROCESSES 10000

struct sched_param param;

int main(){

	char *dir = "./temp/";
	int read_data;
	int status, result;
	char buf[5];
	FILE *fp;
	pid_t pid[MAX_PROCESSES];
	
	for(int i = 0 ; i < MAX_PROCESSES ; i++){
		pid[i] = fork();
		//if child process,
		if(pid[i] == 0){
			//Set Scheduling Policy
			if(!set_schedular(i))printf("Schedule Setting ERROR\n");			
		
			//Memory allocation
			char *index = malloc(sizeof(char) * 15);
			char *dir_cp = malloc(sizeof(char) * 20);
			strcpy(dir_cp, dir);
			sprintf(index, "%d", i);
			strcat(index, ".txt");	
			strcat(dir_cp, index);
			if(!(fp = fopen(dir_cp, "r")))printf("FILE OPEN ERROR!\n");

			//Exception Processing of File read
			while(!fgets(buf, 5, fp)){
			}
			read_data = buf[0];

			//Memory Deallocation
			fclose(fp);
			free(dir_cp);
			free(index);
			exit(1);
		}else if(pid[i] > 0){
			wait(NULL);
		}else{
			printf("Fork Error!\n");
		}
	}
	printf("Priority is MAX\n");
	return 0;
}

int set_schedular(int process_num){

	struct timespec begin, end;
	long time;
	//Initialize Schedule Name
	char *schedule_name[10] = {"SCHED_FIFO_NICE_0",
				"SCHED_FIFO_NICE_HIGH",
				"SCHED_FIFO_NICE_LOW",
				"SCHED_RR_NICE_0",
				"SCHED_RR_NICE_HIGH",
				"SCHED_RR_NICE_LOW",
				"SCHED_OTHER_NICE_0",
				"SCHED_OTHER_NICE_HIGH",
				"SCHED_OTHER_NICE_LOW",
				"SCHED_FIFO_NICE_0"
				};

	//Change Schedule Policy per 1000 child process exit
	switch(process_num % 1000){
		case 0:
			clock_gettime(CLOCK_MONOTONIC, &begin);
			switch(process_num / 1000){
				case 0:	
		                        param.sched_priority = sched_get_priority_max(SCHED_FIFO);
                		        if(sched_setscheduler(0, SCHED_FIFO, &param) != 0){
                                		perror("sched_setscheduler failed \n");
		                                return 0;
                		        }
		                        nice(0);
                		        break;
				case 1:
		                        param.sched_priority = sched_get_priority_max(SCHED_FIFO);
		                        if(sched_setscheduler(0, SCHED_FIFO, &param) != 0){
		                                perror("sched_setscheduler failed \n");
		                                return 0;
		                        }
                		        nice(-20);
		                        break;
                		case 2:
		                        param.sched_priority = sched_get_priority_max(SCHED_FIFO);
		  	                if(sched_setscheduler(0, SCHED_FIFO, &param) != 0){
		                	       perror("sched_setscheduler failed \n");
		        	               return 0;
		                        }
                        		nice(19);
		                        break;
                		case 3:
		                        param.sched_priority = sched_get_priority_max(SCHED_RR);
		                        if(sched_setscheduler(0, SCHED_RR, &param) != 0){
		                                perror("sched_setscheduler failed \n");
		                                return 0;
                		        }
		                        nice(0);
		                        break;
		                case 4:
		                        param.sched_priority = sched_get_priority_max(SCHED_RR);
		                        if(sched_setscheduler(0, SCHED_RR, &param) != 0){
		                                perror("sched_setscheduler failed \n");
		                                return 0;
		                        }
                		        nice(-20);
		                        break;
                		case 5:
		                        param.sched_priority = sched_get_priority_max(SCHED_RR);
		                        if(sched_setscheduler(0, SCHED_RR, &param) != 0){
		                                perror("sched_setscheduler failed \n");
		                                return 0;
		                        }
		                        nice(19);
		                        break;
				case 6:
		                        param.sched_priority = sched_get_priority_max(SCHED_OTHER);
                		        if(sched_setscheduler(0, SCHED_OTHER, &param) != 0){
		                                perror("sched_setscheduler failed \n");
                		                return 0;
		                        }		
                		        nice(0);
		                        break;
                		case 7:
		                        param.sched_priority = sched_get_priority_max(SCHED_OTHER);
		                        if(sched_setscheduler(0, SCHED_OTHER, &param) != 0){
		                                perror("sched_setscheduler failed \n");
                		                return 0;
		                        }	
	                	        nice(-20);
                        		break;
        		        case 8:
		                        param.sched_priority = sched_get_priority_max(SCHED_OTHER);
		                        if(sched_setscheduler(0, SCHED_OTHER, &param) != 0){
		                                perror("sched_setscheduler failed \n");
		                                return 0;
                		        }
		                        nice(19);
		                        break;
				default:
					param.sched_priority = sched_get_priority_max(SCHED_FIFO);
		                        if(sched_setscheduler(0, SCHED_FIFO, &param) != 0){
                		        	perror("sched_setscheduler failed \n");
		                        	return 0;
                		        }
		                        nice(0);
                		        break;
			}
			break;

		//Count 1000 : Timer End & check performance time
		case 999:
			clock_gettime(CLOCK_MONOTONIC, &end);
			time = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec);
                        printf("%d th process (%s) : %lf\n", process_num + 1, schedule_name[process_num/1000] ,(double)time/1000000000);
			sched_yield();
			break;
	}
	return 1;
}
