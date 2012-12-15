#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
void SIGQUIT_handler( int signum);
void SIGUSR1_handler( int signum );
void SIGUSR2_handler( int signum );
void chToInt(char ch);	
void sendString(char msg[]);

int wpPid;
int deepThroatPid;
char msg[256]= "";
 
int main()
{

	signal(SIGQUIT, SIGQUIT_handler);
	signal(SIGUSR1, SIGUSR1_handler);
	signal(SIGUSR2, SIGUSR2_handler);

	deepThroatPid = getpid();
	printf("DeepThroat PID is %d \n",deepThroatPid);

	while(1)
	{
		printf("Waiting for WashingtonPost (1s delay between updates)...\n");
		//pause();
		pause();
		raise(SIGQUIT);
		// sleep() is a system call and will be interrupted
		// when a signal arrives, e.g. raise(SIGUSR1);
	}

	return(0);
}

void SIGQUIT_handler(int signum)
{
	FILE *in;
	char buff[512];

	in = popen("pidof ass2", "r");
	fgets(buff, sizeof(buff), in);

  	pclose(in);

	wpPid = atoi(buff);
	printf("WashingtonPost PID is  %d \n", wpPid);
	
	

	kill(wpPid, SIGQUIT);
	printf("Enter password to send: \n");
	//scanf("%s", msg);
	
		while(1){
	           char c=getchar();
		 if (c=='\n' || c==EOF) break;
            // msg=msg+c;
	int len = (int)strlen(msg);
		msg[len] = c;
		msg[len+1] = '\0';
			}
	sendString(msg);
	exit(1);
}

   void SIGUSR1_handler( int signum )	 
  {
		   printf("\n>> SIGUSR1's user_defined_handler: Caught signal: %d \n", signum);
		   kill( getpid(), SIGINT);
		
   }

   void SIGUSR2_handler( int signum )	 
  {
		   printf("\n>> SIGUSR2's user_defined_handler: Caught signal: %d \n", signum);
		   kill( getpid(), SIGINT);
		
   }

	void chToInt(char ch)
	{
		int n;
		//char ch = msg[i];
		int test = (int)ch;
		//printf("%d %d", test, wpPid);

  	for( n = 32768; n > 0; n = n / 2 ){
    	if(test & n) {
		kill(wpPid, SIGUSR1); //1 printf("1\n");
		usleep(500);
		}
    	else{
		kill(wpPid, SIGUSR2); //0 printf("0\n");
		usleep(500);
		}
		}
	}

	void sendString(char msg[])
	{
				int len = (int)strlen(msg);
				int i=0;

		for(i=0; i<=len-1; i++){
		char ch = msg[i];		
		chToInt(ch);		
		}
		kill(wpPid, SIGILL);
	}
