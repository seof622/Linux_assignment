#include <stdio.h>
#include <stdint.h> 
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

uint8_t* Operation;
uint8_t* compiled_code;
int fd;

void sharedmem_init(); // ���� �޸𸮿� ����
void sharedmem_exit();
void drecompile_init(); // memory mapping ���� 
void drecompile_exit(); 
void* drecompile(uint8_t *func); //����ȭ�ϴ� �κ�



int main(void)
{
	int (*func)(int a);
	int i;
	struct timespec begin, end;
	long time;

	sharedmem_init();

	drecompile_init();

	func = (int (*)(int a))drecompile(Operation);

	//get Time
	clock_gettime(CLOCK_MONOTONIC, &begin);
	func(1);
	clock_gettime(CLOCK_MONOTONIC, &end);
	time = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec);
    	printf("total execution time : %lf msec\n", (double)time/1000000);

	drecompile_exit();

	sharedmem_exit();

	return 0;
}



void sharedmem_init(uint8_t *func)

{
	//access Shared Memory
	int segment_id;
	segment_id = shmget(1234, PAGE_SIZE, 0);
	Operation = shmat(segment_id, NULL, 0);
}



void sharedmem_exit()

{
	//Detach Shared Memory & Remove
	shmdt(Operation);	
	shmctl(Operation, IPC_RMID, NULL);
}

void drecompile_init()
{

	fd = open("./D_recompile.c", O_RDWR);

	//memory Mapping
	compiled_code = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

	//error detect
	assert(compiled_code != MAP_FAILED);
}



void drecompile_exit()

{
	close(fd);
	munmap(compiled_code, PAGE_SIZE);
}



void* drecompile(uint8_t* func)

{

	int i = 0;
	int check_val = 0;
	
	//Compare Buffer
	int Ins_current[2];
	int Ins_before[2];
	
	//Operand of Mul Operation
	int mul_value = 1;

	//In Div, dl Register
	int edx_val = 0;

	//copy Function & Optimization
	do{
		//if ADD(0x83) or SUB(0x83) or MUL(0x6d)
		while(*func == 0x83 || *func == 0x6d){
			while(*func == 0x83){
				*func++;
		                Ins_current[0] = *func++;
        		        Ins_current[1] = *func++;// <-- Operand
		                if(Ins_current[1] != 0x01 ){
				//if Operand is not 1 --> Not Optimize
					if(check_val == 0){
	        		        	compiled_code[i++] = 0x83;
		        		        compiled_code[i++] = Ins_current[0];
	                			compiled_code[i++] = Ins_current[1];
			        	}else{
					//if repeated in the previous process
						compiled_code[i++] = 0x83;
				                compiled_code[i++] = Ins_before[0];
		                        	compiled_code[i++] = check_val;
                			        compiled_code[i++] = 0x83;
		        	                compiled_code[i++] = Ins_current[0];
	        		                compiled_code[i++] = Ins_current[1];
						check_val = 0;
					}
		                }else{
				//Operand is 1 --> repeat Instruction Optimize
					if(check_val == 0){
                				Ins_before[0] = Ins_current[0];
		                        	Ins_before[1] = Ins_current[1];
                    			}if(Ins_before[1] == Ins_current[1]){
						if(check_val == 0){
							check_val++;
						}else{
		                        		Ins_before[0] = Ins_current[0];
							Ins_before[1] = Ins_current[1];
							check_val++;
						}
		                    	}else if(Ins_before[1] != Ins_current[1]){
						compiled_code[i++] = 0x83;
	        			        compiled_code[i++] = Ins_before[0];
		        			compiled_code[i++] = check_val;
	        			        compiled_code[i++] = 0x83;
			                	compiled_code[i++] = Ins_current[0];
				      	        compiled_code[i++] = Ins_current[1];
        					check_val = 0;
				                break;
        		 	      	}
                		}
			}
	        	if(check_val != 0){
				//if Check val > 0 but don't Processing yet
				compiled_code[i++] = 0x83;
				compiled_code[i++] = Ins_current[0];
				compiled_code[i++] = check_val;
			}
			check_val = 0;
			while(*func == 0x6b){
			//if Operator Mul
				*func++;
		                Ins_current[0] = *func++;
                		Ins_current[1] = *func++;// <-- Operand
			        if(Ins_current[1] != 0x02 ){
					//if Operand is Not 2 --> Not Optimize
					if(check_val == 0){
			                    compiled_code[i++] = 0x6b;
                			    compiled_code[i++] = Ins_current[0];
		        	            compiled_code[i++] = Ins_current[1];
					}else{
					//if repeated in the previous process
						compiled_code[i++] = 0x6b;
			                        compiled_code[i++] = Ins_before[0];
        		        	        for(int j = 0; j < check_val ; j++)
                		        	        mul_value *= 2;
        	        		        compiled_code[i++] = mul_value;
	                        		compiled_code[i++] = 0x6b;
	                	        	compiled_code[i++] = Ins_current[0];
	        		                compiled_code[i++] = Ins_current[1];
		        	                check_val = 0;
					}
		                }else{
		          		if(check_val == 0){
						Ins_before[0] = Ins_current[0];
			                        Ins_before[1] = Ins_current[1];
					}if(Ins_before[1] == Ins_current[1]){
						//if Operand and Operator Same, Optimize
						if(check_val == 0){
							check_val++;
						}else{
                		        		Ins_before[0] = Ins_current[0];
							Ins_before[1] = Ins_current[1];
							check_val++;
						}
		                        }else if(Ins_before[1] != Ins_current[1]){
				                compiled_code[i++] = 0x6b;
        	        			compiled_code[i++] = Ins_before[0];
						//Operation Operand
						for(int j = 0; j < check_val ; j++)
							mul_value *= 2;
						compiled_code[i++] = mul_value;
			        	        compiled_code[i++] = 0x6b;
                				compiled_code[i++] = Ins_current[0];
				                compiled_code[i++] = Ins_current[1];
						check_val = 0;
						break;
		    			}
                     
                		}
        		}
		 	if(check_val != 0){
				//if check_val > 0, Must Processing
                        	compiled_code[i++] = 0x6b;
	                        compiled_code[i++] = Ins_current[0];
				for(int j = 0 ; j < check_val ; j++)
					mul_value *= 2;
                        	compiled_code[i++] = mul_value;
                	}
			check_val = 0;
			mul_value = 1;
		}
		check_val = 1;
		while(*func == 0xf7){
		//if Division
			if(check_val == 1){
				*func++;
				//Operand is current index - 4
				edx_val = i - 4;
				compiled_code[i++] = 0xf7;
				compiled_code[i++] = *func;
			}else{
				*func++;
			}	
			*func++;
			check_val *= 2;
			if(*func != 0xf7)
				//decision Operand Value
				compiled_code[edx_val] = check_val;
		}
			
		check_val = 0;
		mul_value = 1;	

		//if Instruction is Not operation, copy Instruction
		compiled_code[i++] = *func;
	}while(*func++ != 0xC3);

/**********check Optimize Function
	for(i = 0 ; compiled_code[i] != 0xC3; i++){
		printf("%x ", compiled_code[i]);
	}
*/
	//convert permission rw- --> r-x
	mprotect(compiled_code, PAGE_SIZE, PROT_READ | PROT_EXEC);
	return compiled_code;
}





