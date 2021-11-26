#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#define MAX_PROCESSES 10000

int main(){

	char *p_dir = "./temp/";
	char *p_dir_copy;

	//make temp directory
	if(!mkdir("./temp", 0755))printf("DIR CREATE ERROR!\n");	
	for(int i = 0 ; i < MAX_PROCESSES ; i ++){
	
		//memory allocation 10bytes
		char *index = malloc(sizeof(char) * 10);
	
		//copy path
		strcpy(p_dir_copy, p_dir);

		//Set File name
		sprintf(index, "%d", i);
		strcat(index, ".txt");
		strcat(p_dir_copy, index);
	
		//File Create and write random integer
		FILE *f_write = fopen(p_dir_copy, "w");
		fprintf(f_write, "%d", 1+rand()%9);
		printf("text : %s index : %s\n", p_dir_copy, index);

		//close file pointer and memory deallocation
		fclose(f_write);
		free(index);
	}

	return 0;
}
