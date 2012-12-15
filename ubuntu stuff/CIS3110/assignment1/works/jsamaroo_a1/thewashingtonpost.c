/* 
 * Name: Junior Samarooo
 * Student ID #0663108
 * Filename: thewashingtonpost.c
 * 
 * CIS3110 Assignment 1
 * 
 * This program called "thewashingtonpost" is responsible for receiving 
 * the message sent to "deepthroat" via signals.
 * 
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void SIGQUIT_handler(int signum);	// handler for signal SIGQUIT 
void SIGUSR1_handler(int signum);	// handler for signal SIGUSR1
void SIGUSR2_handler(int signum);	// handler for signal SIGUSR2 
void SIGILL_handler(int signum);	// handler for signal SIGILL 
void add(int sum); 		 // Sums the interger value of the bits received 
int incorrectSignal();	// Simulates incorrect bit received

int dtPid;				//	Process ID of depthroat
int washingtonPostPid;	//	Process ID of thewashingtonpost
int total;				//	total value of the "add" function
int count, count2=0;	//	flags
char msgReceived[256] = "";	// message received from deepthroat

int main()
{
	/* Signal handlers */
	signal(SIGQUIT, SIGQUIT_handler);
	signal(SIGUSR1, SIGUSR1_handler);
	signal(SIGUSR2, SIGUSR2_handler);
	signal(SIGILL, SIGILL_handler);

	washingtonPostPid = getpid();
	printf("The Washington Post PID = %d\n", washingtonPostPid);	

	FILE *in;
	extern FILE *popen();
	char buff[512];
	in = popen("pidof deepthroat", "r"); // gets PID of deepthroat
	fgets(buff, sizeof(buff), in);
	printf("Deepthroat PID found: %s", buff);
	pclose(in);

	dtPid = atoi(buff); // converts PID to integer
	sleep(1);
	kill(dtPid, SIGQUIT); // verifies communication with deepthroat
	printf("Deepthorat found!!! Trying to communicate...\n");
	sleep(1);
	return(0);
}

/* This handler is used to acknowledge communication with deepthroat */
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

/* This handler receives bit "1" from deepthroat */
void SIGUSR1_handler(int signum)	 
{
	if (incorrectSignal() == 0) // checks for correct signal received
	{
		add(1);
	}
	else
	{
		printf("Bit lost/flipped! Resending bit...\n");
		kill(dtPid, SIGUSR1);
	}
}

/* This handler receives bit "0" from deepthroat */
void SIGUSR2_handler(int signum)	 
{
	if (incorrectSignal() == 0) // checks for correct signal received
	{
		add(0);
	}
	else
	{
		printf("Bit lost/flipped! Resending bit...\n");
		kill(dtPid, SIGUSR2);
	}
}

/* This function sums the interger value of the bits received from 
 * deepthraot so it can be converted back to a character.
 */
void add(int sum)
{
	count = count + 1;
	if (sum == 1)
	total = total + sum*(pow(2,(15-(count-1)))); // conversion to int

	if(count == 16) // stores charcters in an array
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

/* This handler responsible for acknowledging the received message */
void SIGILL_handler(int signum)	 
{
	printf("Message sent is: %s \n", msgReceived);	
	printf("Message received! application exiting........\n");

	exit(1);
}

/* This function simulates incorrect signals being received */
int incorrectSignal()
{
	int randNum = 0;
	randNum = rand() % 3;
	return randNum;
}
