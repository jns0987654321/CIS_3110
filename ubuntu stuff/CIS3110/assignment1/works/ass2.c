#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void SIGQUIT_handler( int signum);
void SIGUSR1_handler( int signum );
void SIGUSR2_handler( int signum );
void add(int sum);
void SIGILL_handler( int signum );

int dtPid;
int washingtonPostPid;
int total;
int count, count2=0;
char msgReceived[256] = "";

int main()
{
	signal(SIGQUIT, SIGQUIT_handler);
	signal(SIGUSR1, SIGUSR1_handler);
	signal(SIGUSR2, SIGUSR2_handler);
	signal(SIGILL, SIGILL_handler);

	FILE *in;
	extern FILE *popen();
	char buff[512];
	
	washingtonPostPid = getpid();
	printf("The Washington Post PID = %d\n", washingtonPostPid);	

	 if (!(in = popen("pidof ass1", "r"))) {
		printf("Error getting pidof!!\n");
 	 }
  	while (fgets(buff, sizeof(buff), in) != NULL ) {
        printf("Deepthroat PID: %s", buff);
         }
  	pclose(in);

	dtPid = atoi(buff);
	//while(1){
	printf("Waiting for DeepThroat (5s delay)...\n");
	sleep(1);
	printf("Deepthorat found!!!...\n");
  	kill(dtPid, SIGQUIT);
	sleep(1);
	printf("testtttttttttttttttttttttttttttttttttttttttttttt\n");
	//}
	return(0);
}

void SIGQUIT_handler(int signum)
{
	while(count2 == 0){
	printf("Wating for password....\n");
	pause();
	count2 =1;
	}
	while(count2 == 1){
		printf("Received....\n");
		pause();
	}
}

void SIGUSR1_handler( int signum )	 
  {
		 //  printf("got 1 \n");
		add(1);
		
   }

   void SIGUSR2_handler( int signum )	 
  {
		   //printf("got 0 \n");
		add(0);
		
   }

void add(int sum)
{
	count = count + 1;
	if (sum == 1)
	//printf("count = %d \n",count);
	total = total + sum*(pow(2,(15-(count-1))));
	//printf("total = %d \n",total);

	if(count == 16 )
	{

		char ch; 
		ch = (char)(total);
		//printf("character is %c \n", ch);
		int len = (int)strlen(msgReceived);
		msgReceived[len] = ch;
		msgReceived[len+1] = '\0';
		count = 0;
		count2 = 1;
		total = 0;
		//printf("cleared\n");
	}
		//printf("mess is %s \n",msgReceived);		
}

void SIGILL_handler( int signum )	 
  {
		printf("mess is %s \n",msgReceived);		
		exit(1);
		
   }


