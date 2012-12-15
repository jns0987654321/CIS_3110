#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
void SIGQUIT_handler(int signum);
void SIGUSR1_handler(int signum);
void SIGUSR2_handler(int signum);

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
		printf("Waiting for WashingtonPost (system paused)...\n");
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
	printf("Communication with WashingtonPost verified!!! \n");
	printf("Enter message to send: \n");

	while(1)
	{
		char c=getchar();
		if (c=='\n' || c==EOF) break;
		int len = (int)strlen(msg);
		msg[len] = c;
		msg[len+1] = '\0';
	}
	
	sendString(msg);
	printf("Message sent! application exiting........\n");
	exit(1);
}

void SIGUSR1_handler(int signum)	 
{
	kill(wpPid, SIGUSR1);
	usleep(5000);
}

void SIGUSR2_handler(int signum)	 
{
	kill(wpPid, SIGUSR2);
	usleep(5000);
}

void chToInt(char ch)
{
	int num;
	int charNum = (int)ch;

	for(num = 32768; num > 0; num = num/2)
	{
		if(charNum & num) 
		{
			kill(wpPid, SIGUSR1); //1 printf("1\n");
			usleep(5000);
		}
		else
		{
			kill(wpPid, SIGUSR2); //0 printf("0\n");
			usleep(5000);
		}
	}
}

void sendString(char msg[])
{
	int len = (int)strlen(msg);
	int i=0;

	for(i=0; i<=len-1; i++)
	{
		char ch = msg[i];		
		chToInt(ch);		
	}
	kill(wpPid, SIGILL);
}
