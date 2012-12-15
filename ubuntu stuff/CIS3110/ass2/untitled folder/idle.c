#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

int main (int argc, char * argv[])
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
				printf("gg %s\n", buff);      
					if (index(buff, 'm') != NULL)
					{
						num = strtok(buff, ":");
						printf("halff %s\n", num); 
						total = (atoi(num))*360 + total;  
						printf("totot %d\n", total); 
   
						num = strtok(NULL, "m");
						printf("hal22ff %s\n", num); 
						total = (atoi(num))*60 + total;     
						printf("totot %d\n", total); 
					}else if (index(buff, ':') != NULL){
						num = strtok(buff, ":");
						printf("halff %s\n", num); 
						total = (atoi(num))*60 + total;  
						printf("totot %d\n", total); 
   
						num = strtok(NULL, " ");
						printf("hal22ff %s\n", num); 
						total = (atoi(num)) + total;     
						printf("totot %d\n", total); 	
					}
					if (index(buff, '.') != NULL)
					{
						int temp2=0;
						num = strtok(buff, ".");
						printf("halff %s\n", num); 
						temp2 = (atoi(num)) + temp2;  
						printf("totot %d\n", total); 
   
						num = strtok(NULL, "s");
						printf("hal22ff %s\n", num); 
						temp2 = (atoi(num))*(1/100) + temp2;     
						if(temp2 == 0){
						skip = 1;}else{
							total = temp2;
						}
						printf("totot %d temp22 %d\n", total, temp2); 

					}
					printf("dddddddddddddddddd %d %d %d", lowest, total, temp);
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
	
	

           

	printf("users %d\n", users);
	printf("lowest %d\n", lowest);




	return 0;
}
