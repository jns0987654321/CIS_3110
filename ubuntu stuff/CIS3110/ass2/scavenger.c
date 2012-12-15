/* 
 * Name: Junior Samarooo
 * Student ID #0663108
 * Filename: scavenger.c
 * 
 * CIS3110 Assignment 2
 * 
 * This program is a simple scheduling facility for spawning and managing
 * the execution of background processes on a workstation.
 * 
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <string.h>
#include <signal.h>

void SIGINT_handler(int signum);	// handler for signal SIGINT
void SIGTERM_handler(int signum);	// handler for signal SIGTERM
void SIGABRT_handler(int signum);	// handler for signal SIGABRT 
void SIGHUP_handler(int signum);	// handler for signal SIGHUP
void SIGQUIT_handler(int signum);	// handler for signal SIGQUIT 
void killAll();						// handles termination of scavenger
void doSchedule();					// start scheduling
int processExists();				// whether process has been terminated

/* Structure to store process information*/
typedef struct processes
{
	float PID; 
	float IDLE;
	float BUSY;
	float USER;
}process;

int lowestIdle();	// lowest idle time from "w"
FILE *fp;			// file pointer
int maxFileSize;	// max size of database file
process b;			// temporary struct to store process info			
int scavengerPID;	// background scheduler PID
int i=0;			// counter
int pidFork;		// PID of forked process
int exists;			// flag for database existance
process *ptr;		// struct pointer
double loadavg[3];	// stores load average

int main (int argc, char * argv[])
{
	/* Signal Handlers */
	signal(SIGINT, SIGINT_handler);
	signal(SIGTERM, SIGTERM_handler);
	signal(SIGABRT, SIGABRT_handler);
	signal(SIGHUP, SIGHUP_handler);
	signal(SIGQUIT, SIGQUIT_handler);


	ptr = malloc(sizeof(process));

	/* Gets IDLE time from args */
	char *num;	
	num = strtok(argv[1],"=");
	num = strtok (NULL, " ");
	ptr->IDLE = atoi(num);	

	/* Gets BUSY time from args */
	num = strtok(argv[2],"=");
	num = strtok (NULL, " ");
	ptr->BUSY = atoi(num);	

	/* Gets USER time from args */
	num = strtok(argv[3],"=");
	num = strtok (NULL, " ");
	ptr->USER = atoi(num)*60;	//Converts to seconds for comparison	

	if (access("/tmp/db", R_OK) !=-1)
		exists = 1;    //Database exists
	else
		exists = 0;    //Databse does not exist		

	if 	(exists == 0)
	{
		setsid();
		/* If database does not exist, creates it */
		if ((pidFork = fork()) == 0)
		{
			fp = fopen("/tmp/db", "ab+");
			flock(fileno(fp), LOCK_EX);
			fclose(fp);
			
			doSchedule(); //Starts scheduling
		}
	}

	scavengerPID = pidFork;	// stores scavenger PID
	
	/* Execs "pname" argument */
	if ((pidFork = fork()) == 0)
	{
		char**args =malloc(sizeof(char*)*(argc-3));
		for(i =0; i<argc-4;i++)
		{
			args[i] = argv[i+4];
		}
		args[argc-4] = NULL;
		execv(argv[4],args);
	}else if (exists==1)
	{
		/* If database exists adds process info to database */
		ptr->PID = pidFork;
		fp = fopen("/tmp/db", "ab+"); //appends info to end of file
		fwrite(ptr, sizeof(process), 1, fp);
		flock(fileno(fp), LOCK_UN);
		fclose(fp);
		
		sleep(1);
		kill(pidFork, SIGSTOP); // Execs process in a stopped state
	}
	
	/* Adds first process to database */
	if (exists == 0)
	{		
		ptr->PID = pidFork;
		fp = fopen("/tmp/db", "ab+");
		flock(fileno(fp), LOCK_EX);

		fseek(fp, 0L, SEEK_SET);
		fwrite(ptr, sizeof(process), 1, fp);
		
		flock(fileno(fp), LOCK_UN);
		fclose(fp);
	}
		return (0);
}

/* This function finds the lowest idle time of the system and the 
 * number of users logged into the system
*/
int lowestIdle()
{
	FILE *in;				// file pointer
	extern FILE *popen();	// file pointer
	char buff[512];			// temp buffer 
	char *num;				// pointer for strtok
	int users;				// number of users logged in
	int i;					// counter
	int lowest = 0;			// lowest idle time
	int temp = 0;			// 	stores previous total
	char * line = NULL;		//	temp buffer
	size_t len = 0;			//	length of "line"

	/* Finds number of users */
	in = popen("w -s -f", "r"); 
	fgets(buff, sizeof(buff), in);
	num = strtok(buff, ",");
	num = strtok (NULL, "users,");
	users = atoi(num);
	getline(&line, &len, in);

	/* Converts idle time to seconds */ 
	for (i=0; i<users; i++)
	{
		int total = 0;	//flag
		int skip = 0;	//flag

		getline(&line, &len, in);
		sscanf(line, "%*s %*s %s %*s", buff);
		
		/* Converts HH:MM format to secs */
		if (index(buff, 'm') != NULL)
		{
			num = strtok(buff, ":");
			total = (atoi(num))*360 + total;  
			num = strtok(NULL, "m");
			total = (atoi(num))*60 + total;     
			}else if (index(buff, ':') != NULL)
			{
				/* Converts MM:SS to secs */
				num = strtok(buff, ":");
				total = (atoi(num))*60 + total;  
				num = strtok(NULL, " ");
				total = (atoi(num)) + total;     
			}
		/* Converts SS.CC to secs */	
		if (index(buff, '.') != NULL)
		{
			int temp2 = 0; 
			num = strtok(buff, ".");
			temp2 = (atoi(num)) + temp2;  
			num = strtok(NULL, "s");
			temp2 = (atoi(num))*(1/100) + temp2;     
			if(temp2 == 0)
			{
				skip = 1; // used to skip zero idle time
			}else{
				total = temp2;
			}
		}
		/* Finds lowest IDLE time when the time is not zero */
		if(skip ==0)
		{
			if (total > temp){
				lowest = temp;
			}else{
				lowest = total;
			}
			temp = total;
		}
	}
	pclose(in);

	return (lowest);	//returns lowest idle time
}

/* Deletes database files and terminates processes */
void SIGINT_handler(int signum)	 
{
	killAll();	
}

/* Deletes database files and terminates processes */
void SIGTERM_handler(int signum)	 
{
	killAll();	
}

/* Deletes database files and terminates processes */
void SIGABRT_handler(int signum)	 
{
	killAll();	
}

/* Deletes database files and terminates processes */
void SIGHUP_handler(int signum)	 
{
	killAll();	
}
/* Deletes database files and terminates processes */
void SIGQUIT_handler(int signum)	 
{
	killAll();	
}

/* Function that deletes database files and terminates processes */
void killAll()
{
	fp = fopen("/tmp/db", "r+");
	fseek(fp, 0L, SEEK_END);
	maxFileSize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	
	while(ftell(fp) != maxFileSize)
	{
		fread(&b, sizeof(process), 1, fp);	
		kill(b.PID, SIGKILL); //kills each active process
	}
	fclose(fp);
	
	/* Removes database file */
	if(remove ("/tmp/db") !=0)
		printf("Error deleting file\n");
	else
		printf("File successfully deleted and processes exited\n");
		
	kill(scavengerPID, SIGKILL);	//kills scavenger
}

/* Starts scheduling processes */
void doSchedule()
{
	while(1)
	{
		sleep(10);
		getloadavg(loadavg, sizeof(loadavg) / sizeof(loadavg[0]));

		fp = fopen("/tmp/db", "r+");
		flock(fileno(fp), LOCK_EX);

		fseek(fp, 0L, SEEK_END);
		maxFileSize = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		int numProcesses=0;
		while(ftell(fp) != maxFileSize)
		{
			fread(&b, sizeof(process), 1, fp);	
			if(loadavg[0] < b.IDLE)
			{	
				sleep(1);
				kill(b.PID, SIGCONT);
				sleep(60);
				kill(b.PID, SIGSTOP);}
				numProcesses = numProcesses +1;

			if(ftell(fp) == 0)
		}
		/* add comparisons of IDLE and USER values (not implemented yet) 
		 * if (loadavg[0] < IDLE) or if (lowestIDLEtime < USER)
		 * 		active
		 * 
		 * if (loadavg[0] > BUSY) or if (lowestIDLEtime > USER)
		 * 		busy 
		 */
		
	}
}

int processExists()
{
	if(kill(b.PID, SIGKILL) == -1){
	return (0);
	}else{
	return (1);
	}
	
	// Use SIGKILL to check if PID exists returns -1 if it doesn't
	// exist. Remove it from database or dont read this struct. 
	//(still to be implemented)
}

