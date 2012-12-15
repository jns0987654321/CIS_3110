#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

typedef struct processes
{
	float PID; 
	float IDLE;
	float BUSY;
	float USER;
}process;

//void addProcess(FILE *fp, records *ptr, char *namest, records *b, char *addressst, int *choices, int *isAllocated, int *ch);

int main (int argc, char * argv[])
{
	int i=0;
	int pidFork;
	int exists;
	FILE *fp;
	process *ptr;
	double loadavg[3];
	
			ptr = malloc(sizeof(process));

			char *num;	
			num = strtok(argv[1],"=");
			num = strtok (NULL, " ");
			ptr->IDLE = atoi(num);	

			num = strtok(argv[2],"=");
			num = strtok (NULL, " ");
			ptr->BUSY = atoi(num);	
	
			num = strtok(argv[3],"=");
			num = strtok (NULL, " ");
			ptr->USER = atoi(num);	
			
			if (access("/tmp/db", R_OK) !=-1)
			{
				exists = 1;
			}else{
				exists = 0;
			}			
	
	if 	(exists == 0)
		{
							setsid();

			 if ((pidFork = fork()) == 0)
			{
			fp = fopen("/tmp/db", "ab+");
			printf("Database Created\n");
				fclose(fp);
								sleep(1);
								kill(pidFork, SIGSTOP);

			}
		}
				
	if ((pidFork = fork()) == 0)
			{
				//setsid();
				char**args =malloc(sizeof(char*)*(argc-3));
				for(i =0; i<argc-4;i++){
					args[i] = argv[i+4];
				}
				args[argc-4] = NULL;
								printf("executedd");
				execv(argv[4],args);
			}else if (exists==1){
			printf("addding Already exits\n");
			printf("this is two %d\n", pidFork);
			ptr->PID = pidFork;
			fp = fopen("/tmp/db", "ab+");
			fwrite(ptr, sizeof(process), 1, fp);
				fclose(fp);
				sleep(1);
				kill(pidFork, SIGSTOP);

			};
			if (exists ==0)
			{		
			printf("this is onee %d\n", pidFork);
						ptr->PID = pidFork;
			fp = fopen("/tmp/db", "ab+");
			fseek(fp, 0L, SEEK_SET);
						fwrite(ptr, sizeof(process), 1, fp);
						fclose(fp);
			printf("database really created\n");

			}
							getloadavg(loadavg, sizeof(loadavg) / sizeof(loadavg[0]));
							printf(" %lf\n", loadavg[0]);
							


		//fork();
	/*	fp = fopen("/tmp/db", "r+");
		fseek(fp, 0L, SEEK_END);
		maxFileSize = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		fread(&b, sizeof(process), 1, fp);		
		printf("test %lf\n", b.BUSY);
				printf("this is two %d\n", pidFork);

	fclose(fp);*/
		
	return (0);
}



