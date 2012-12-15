/* 
 * Name: Junior Samarooo
 * Student ID #0663108
 * Filename: deepthroat.c
 * 
 * CIS3110 Assignment 1
 * 
 * This program called "deepthroat" is responsible for sending a message
 * via signals to the receiver called "thewashingtonpost" which decodes 
 * it.
 * 
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

void SIGQUIT_handler(int signum); // handler for signal SIGQUIT 
void SIGUSR1_handler(int signum); // handler for signal SIGUSR1 
void SIGUSR2_handler(int signum); // handler for signal SIGUSR2 
void chToInt(char ch);			// Converts character to integer 
void sendString(char msg[]);	// Sends message to thewashingtonpost
 
int wpPid;						// Process ID of thewasingtonpost
int deepThroatPid;				// Process ID of deepthroat
char msg[256]= "";				// Message to be sent to thewashingtonpost

int main()
{	
	/* Signal handlers */
	signal(SIGQUIT, SIGQUIT_handler);
	signal(SIGUSR1, SIGUSR1_handler);
	signal(SIGUSR2, SIGUSR2_handler);

	deepThroatPid = getpid();
	printf("DeepThroat PID is %d \n",deepThroatPid);

	while(1)
	{
		printf("Waiting for WashingtonPost (system paused)...\n");
		pause(); // sleeps until a system interrupt occurs
		raise(SIGQUIT);
	}
	
	return(0);
}

/* This handler gets the process id of thewashingtonpost. It also deals
 * with getting user input for the message to be sent.
*/
void SIGQUIT_handler(int signum)
{
	FILE *in;
	extern FILE *popen();
	char buff[512];
	in = popen("pidof thewashingtonpost", "r"); // gets PID
	fgets(buff, sizeof(buff), in);
	pclose(in);

	wpPid = atoi(buff); // converts PID to an integer
	printf("WashingtonPost PID is  %d \n", wpPid);
	kill(wpPid, SIGQUIT); // sends signal to washingtonpost SIGQUIT handler
	printf("Communication with WashingtonPost verified!!! \n");
	printf("Enter message to send: \n");

	/* loop gets user input*/
	while(1)
	{
		char c=getchar();
		if (c=='\n' || c==EOF) break;
		int len = (int)strlen(msg);
		msg[len] = c;
		msg[len+1] = '\0';
	}
	
	sendString(msg); // calls function to send message to thewashingtonpost
	printf("Message sent! application exiting........\n");
	exit(1);
}

/* This handler deals with resending a "1" bit when an incorrect bit
 * is received 
*/
void SIGUSR1_handler(int signum)	 
{
	kill(wpPid, SIGUSR1);
	usleep(5000);
}

/* This handler deals with resending a "0" bit when an incorrect bit
 * is received 
*/
void SIGUSR2_handler(int signum)	 
{
	kill(wpPid, SIGUSR2);
	usleep(5000);
}

/* this function sends the characters of the message to the "chToInt"
 * function so that it can be converted to bits.
 */
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

/* This function converts characters of the string to be sent to an integer 
 * so that it can be sent via signals via bits.
 */
void chToInt(char ch)
{
	int num;
	int charNum = (int)ch;

	for(num = 32768; num > 0; num = num/2)
	{
		if(charNum & num) 
		{
			kill(wpPid, SIGUSR1); // sends a "1" to thewashingtonpost
			usleep(5000);
		}
		else
		{
			kill(wpPid, SIGUSR2); //sends a "0" to thewashingtonpost
			usleep(5000);
		}
	}
}

