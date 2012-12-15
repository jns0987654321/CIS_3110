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

int lowestIdle();
//void addProcess(FILE *fp, records *ptr, char *namest, records *b, char *addressst, int *choices, int *isAllocated, int *ch);

int main (int argc, char * argv[])
{
	int i=0;
	int pidFork;
	int exists;
	FILE *fp;
	process *ptr, b;
	double loadavg[3];
	int maxFileSize;
	int scavengerPID;
	
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
				
				while(1)
				{
					fp = fopen("/tmp/db", "r+");
					fseek(fp, 0L, SEEK_END);
					maxFileSize = ftell(fp);
					fseek(fp, 0L, SEEK_SET);
					int numProcesses=0;
					while(ftell(fp) != maxFileSize)
					{
								fread(&b, sizeof(process), 1, fp);	
								sleep(1);
								kill(b.PID, SIGCONT);
								//sleep(10);
								//kill(b.PID, SIGSTOP);
								numProcesses = numProcesses +1;
					}
						//printf("num of pr %d\n", numProcesses);
				}

			}

		}
												
			scavengerPID = pidFork;

				
	if ((pidFork = fork()) == 0)
			{
	
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
		/*					getloadavg(loadavg, sizeof(loadavg) / sizeof(loadavg[0]));
							printf(" %lf\n", loadavg[0]);
							//kill(0, SIGCONT);
				int lowIdle = lowestIdle();
				//printf("this is the lowest %d\n", lowIdle);

		fp = fopen("/tmp/db", "r+");
		fseek(fp, 0L, SEEK_END);
		maxFileSize = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		fread(&b, sizeof(process), 1, fp);		
		//printf("test %f %f % f %f\n", b.PID, b.IDLE, b.BUSY, b.USER);
		fclose(fp);*/
				

		
	return (0);
}

int lowestIdle()
{
	FILE *in;
	extern FILE *popen();
	char buff[1024];
	char *num;	
	int users;
	int i;
	int lowest = 0;
	char time[100];
	int temp = 0;
	
	
	char * line = NULL;
           size_t len = 0;
           ssize_t read;

	in = popen("w -s -f", "r"); // gets PID
	fgets(buff, sizeof(buff), in);
	num = strtok(buff, ",");
	num = strtok (NULL, "users,");
	users = atoi(num);
	char * usrIdle[users];

           	getline(&line, &len, in);

           for (i=0; i<users; i++)
           {
			   	int total =0;
			   	int skip =0;

					getline(&line, &len, in);
					sscanf(line, "%*s %*s %s %*s", buff);
			//	printf("gg %s\n", buff);      
					if (index(buff, 'm') != NULL)
					{
						num = strtok(buff, ":");
				//		printf("halff %s\n", num); 
						total = (atoi(num))*360 + total;  
					//	printf("totot %d\n", total); 
   
						num = strtok(NULL, "m");
						//printf("hal22ff %s\n", num); 
						total = (atoi(num))*60 + total;     
						//printf("totot %d\n", total); 
					}else if (index(buff, ':') != NULL){
						num = strtok(buff, ":");
						//printf("halff %s\n", num); 
						total = (atoi(num))*60 + total;  
						//printf("totot %d\n", total); 
   
						num = strtok(NULL, " ");
						//printf("hal22ff %s\n", num); 
						total = (atoi(num)) + total;     
						//printf("totot %d\n", total); 	
					}
					if (index(buff, '.') != NULL)
					{
						int temp2=0;
						num = strtok(buff, ".");
						//printf("halff %s\n", num); 
						temp2 = (atoi(num)) + temp2;  
						//printf("totot %d\n", total); 
   
						num = strtok(NULL, "s");
						//printf("hal22ff %s\n", num); 
						temp2 = (atoi(num))*(1/100) + temp2;     
						if(temp2 == 0){
						skip = 1;}else{
							total = temp2;
						}
						//printf("totot %d temp22 %d\n", total, temp2); 

					}
					//printf("dddddddddddddddddd %d %d %d", lowest, total, temp);
					if(skip ==0){
					if (total > temp)
					{
						lowest = temp;
						
					}else{
						lowest = total;
					}
					temp = total;
				}

		   }
           
	pclose(in);

	//~ printf("users %d\n", users);
	//~ printf("lowest %d\n", lowest);
	return (lowest);
           
}



