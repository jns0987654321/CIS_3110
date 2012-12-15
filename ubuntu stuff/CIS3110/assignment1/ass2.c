#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void SIGQUIT_handler(int signum);
void SIGUSR1_handler(int signum);
void SIGUSR2_handler(int signum);
void SIGILL_handler(int signum);
void add(int sum); 
int incorrectSignal();

int dtPid;
int washingtonPostPid;
int total;
int count;
int count2=0;
char msgReceived[256] = "";

int main()
{
	signal(SIGQUIT, SIGQUIT_handler);
	signal(SIGUSR1, SIGUSR1_handler);
	signal(SIGUSR2, SIGUSR2_handler);
	signal(SIGILL, SIGILL_handler);

	washingtonPostPid = getpid();
	printf("The Washington Post PID = %d\n", washingtonPostPid);	

	FILE *in;
	extern FILE *popen();
	char buff[512];
	in = popen("pidof ass1", "r"); 
	fgets(buff, sizeof(buff), in);
	printf("Deepthroat PID found: %s", buff);
	pclose(in);

	dtPid = atoi(buff);
	sleep(1);
	kill(dtPid, SIGQUIT);
	printf("Deepthorat found!!! Trying to communicate...\n");
	sleep(1);
	return(0);
}

void SIGQUIT_handler(int signum)
{
	while(count2 == 0)
	{
		printf("Communication verified! Wating for message from Deepthroat....\n");
		pause();
		count2 =1;
}
	while(count2 == 1)
	{
		printf("Received uninterrrupted bit!!...\n");
		pause();
	}
}

void SIGUSR1_handler(int signum)	 
{
	if (incorrectSignal() == 0)
	{
		add(1);
	}
	else
	{
		printf("Bit lost/flipped! Resending bit...\n");
		kill(dtPid, SIGUSR1);
	}
}

void SIGUSR2_handler(int signum)	 
{
	if (incorrectSignal() == 0)
	{
		add(0);
	}
	else
	{
		printf("Bit lost/flipped! Resending bit...\n");
		kill(dtPid, SIGUSR2);
	}
}

void add(int sum)
{
	count = count + 1;
	if (sum == 1)
	total = total + sum*(pow(2,(15-(count-1))));

	if(count == 16)
	{
		char ch; 
		ch = (char)(total);
		int len = (int)strlen(msgReceived);
		msgReceived[len] = ch;
		msgReceived[len+1] = '\0';
		count = 0;
		total = 0;
	}
}

void SIGILL_handler(int signum)	 
{
	printf("Message sent is: %s \n", msgReceived);	
	printf("Message received! application exiting........\n");

	exit(1);
}

int incorrectSignal()
{
	int randNum = 0;
	randNum = rand() % 3;
	return randNum;
}
