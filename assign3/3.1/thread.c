#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <time.h>
#include <semaphore.h>

void *thread_func(void *);
void cleanup_func();
pid_t gettid(void);
pthread_t tid[4];
unsigned long tid_name[4];
FILE *fp;
sem_t semaphore;
int main(){
	
	int thread_return;
	int thread_num[4] = {0, 1, 2, 3};
	
	//Related Time
	struct timespec begin, end;
	long time;

	//File Open & Semaphore initiallize
	if(!(fp = fopen("temp.txt", "r")))printf("FILE OPEN ERROR!\n");
	sem_init(&semaphore, 0, 1);
	
	//Time check : start
	clock_gettime(CLOCK_MONOTONIC, &begin);

	//Thread Create
	pthread_create(&tid[0], NULL, thread_func, (void *)&thread_num[0]);
	pthread_create(&tid[1], NULL, thread_func, (void *)&thread_num[1]);
	pthread_create(&tid[2], NULL, thread_func, (void *)&thread_num[2]);
	pthread_create(&tid[3], NULL, thread_func, (void *)&thread_num[3]);   
	
	//Final Thread Join -- > Thread 0
	pthread_join(tid[0], (void *)&thread_return);
	
	//Time check : end
	clock_gettime(CLOCK_MONOTONIC, &end);
	time = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec);

	printf("value of thread : %d\n", thread_return);
	printf("%lf\n", (double)time/1000000000);

	//Close File
	fclose(fp);
	return 0;
}

void *thread_func(void *arg){
	
	int retval1, retval2;
	char left_num = 0, rigth_num = 0;
	char buf[5];
	
	//In Critical Section
	sem_wait(&semaphore);

	tid_name[*(int *)arg] = (unsigned long)gettid();

	//Exception Processing : File Read
	while(!fgets(buf, 5, fp)){
		printf("READ ERROR\n");
	}
	left_num = buf[0];
	while(!fgets(buf, 5, fp)){
		printf("READ ERROR\n");	
	}
	rigth_num = buf[0];
	
	//Out Critical Section
	sem_post(&semaphore);
	
	//Thread can perform next instruction, when all Thread Created
	while(!(tid_name[0] && tid_name[1] && tid_name[2] && tid_name[3])){}

	//Thread(1), Thread(3) return and Thread(0), Thread(2) Join There
	// -96 is ascii to integer ( ascii 48 == integer 0) 	

	//Thread 1, 3 return value (= Result of add)
	if(*(int *)arg == 1 || *(int *)arg == 3){
		return (void *)(left_num + rigth_num - 96);
	}
	
	//Thread 2 Join Thread 3 and return value(= Result of add + return value of Thread 3)
	else if(*(int *)arg == 2){
		pthread_join(tid[*(int *)arg + 1], (void *)&retval1);
		retval1 += left_num + rigth_num - 96;
		return (void *)retval1;
	}
	
	//Thread 0 add and return all return value of each thread
	else if(*(int *)arg == 0){
		pthread_join(tid[*(int *)arg + 1], (void *)&retval1);	
		pthread_join(tid[*(int *)arg + 2], (void *)&retval2);
		retval1 += retval2 + left_num + rigth_num - 96;
		return (void *)retval1;
	}		
}

pid_t gettid(void){
	return syscall(__NR_gettid);
}
