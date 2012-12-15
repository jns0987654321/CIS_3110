/* 
 * Name: Junior Samarooo
 * Student ID #0663108
 * Filename: resed.c
 * 
 * CIS3110 Assignment 3
 * 
 * This program is a simple server providing time, file management
 * and remote execution services to client "resec".
 * 
 */
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>

#define MAXSTR 1025		// max string size
#define PORT1 3000		// port 3000 - time service
#define PORT2 3001		// port 3001 - file management services
#define PORT3 3002		// port 3002 - remote execution service

/* Function Prototypes */
void timeServer(int);	//	Performs time service
void fileServer(int);	//	Performs file management services
void execServer(int);	//	Performs file execution services
int oct2Dec(int oct);	//	Converts octal to decimal (used for chmod)

typedef enum { LIS, GET, PUT, EXE, ACK, ERR } PacketType;

/* Packet structure */
typedef struct
{
	PacketType type;
	int size;
	int perm;
	char text[MAXSTR];
} packet;

/* Error handler */
void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int pidFork, pid;	// PID's
	int i;				// Counter
	int S[3];			// Socket array
	socklen_t clilen;	// Client address size
	struct sockaddr_in serv_addr, cli_addr;		// Internet address structure
	
	signal(SIGCHLD,SIG_IGN);	// Zombie process handler
	
	/* Loop to create socket, bind and listen (for all ports) */
	for (i=0; i<3; i++)
	{
		S[i] = socket(AF_INET, SOCK_STREAM, 0); // create new socket
		if (S[i] < 0) 
			error("ERROR opening socket!\n");
		fcntl(S[i], F_SETFL, O_NONBLOCK);	// set to non-blocking
		bzero((char *) &serv_addr, sizeof(serv_addr));	// sets all values in buffer to zero
		
		/*
		* Bind to server address - in network byte order
		*/
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(PORT1+i);	// ports 3000,3001,3002
		
		/*
		* Bind socket to port/addr
		*/
		if (bind(S[i], (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
			error("ERROR on binding!\n");
		listen(S[i],5); // Listen on this socket
	}
	clilen = sizeof(cli_addr);

	/* Fork creates persistent server */
	if ((pidFork = fork()) == 0)
	{
		/* Server runs forever */
		while(1) 
		{		
			int i=0;
			for (i=0; i<3; i++)
			{
				int NS;
				if((NS = accept(S[i], (struct sockaddr *) &cli_addr, &clilen)) >0 ) // accepts client connection
				{
					pid = fork();	// Forks off to deal with connection
					if (pid < 0)
						error("ERROR on fork");
					if (pid == 0)  
					{
						close(S[i]);
						if (i == 0)
							timeServer(NS);	// Calls time request function
						if (i == 1)
							fileServer(NS);	// Calls file management function
						if (i == 2)
							execServer(NS);	// Calls file execution function
						exit(0);
					}else close(NS);
				}


			}

		}				

	}
	
	return (0);
}

/*	Sends time of server to client */
void timeServer (int sock)
{
	int n;
	char buffer[256];
	time_t rawtime;
	time ( &rawtime );

	bzero(buffer,256);
	n = write(sock,ctime (&rawtime),26);	// time string must be 26 bytes
}

/* File management services
 * 
 * -List files on server
 * -Get file from server
 * -Put file to server from client
 * 
 */
void fileServer (int sock)
{
	FILE *fp;					// File pointer for popen()
	int n;						// Return value of read/write
	int fd;						// File descriptor
	int maxFileSize;			// Max size of requested file
	char buffer[256];			// Buffer
	packet request, response;	// Packets

	/* Zeros buffers */
	bzero(response.text,1025);
	bzero(buffer,255);

	n = read(sock, &request, sizeof(packet)); // read packet from client
	if (n < 0) 
		error("ERROR reading from socket");

	/*
	 * File listing Service
	 */
	if (request.type == LIS)
	{
		if ((fp = popen("ls", "r")) != NULL)
		{
			char list[MAXSTR];
			bzero(list, MAXSTR);
			response.type = ACK;
			n = write(sock, &response, sizeof(packet));
			while (fgets(buffer, sizeof(buffer), fp))
				strcat(list, buffer);
			strcat(list, "\0");
			n = write(sock, &list, sizeof(list)); // send list of files to client
		}else{
				/* Error in listing files */
				response.type = ERR;
				strcat(response.text, "Error getting list of files!!\n");
				n = write(sock, &response, sizeof(packet));
			} 
	}	// end of "list" service

	/*
	 * "Get File" service
	 */
	if (request.type == GET)
	{
		int fd;	// file descriptor
		/* Check if file exists or has read permissions */
		if ((fd = open(request.text,O_RDONLY)) < 0)
		{
			/* File doesnt exist */
			response.type = ERR;
			strcat(response.text, "Error getting file!! File does not exist or does not have read permissions!!\n");
			n = write(sock, &response, sizeof(packet));
			close(fd);
		}else{
				/* File exists/has read permissions */
				struct stat buf;	// Struct containing file information
				fstat(fd, &buf);
				int size = buf.st_size;	// size of file
				int perm = buf.st_mode;	// permissions of file

				maxFileSize = size;
				response.type = ACK;
				response.perm = perm;
				response.size = maxFileSize;
				n = write(sock, &response, sizeof(packet));	// send packet to client
				char file[maxFileSize];
				read(fd, &file, maxFileSize);	// read clients response

				int i=0;
				/* Sends file to client byte by byte */
				for (i=0; i<maxFileSize; i++)
				{
					write(sock, &file[i], 1);
				}
				close(fd);
			}
	} // end of "get file" service
	
	/*
	 * "Put file" service
	 */
	if (request.type == PUT)
	{
		if (request.perm > 7 || request.perm < 0)
		{
			response.type = ERR;
			strcat(response.text, "Error: Permissions must be a number from 0-9!!\n");
			n = write(sock, &response, sizeof(packet));
		}
		
		/* Checks if file exits */
		if (request.size != 0 && request.perm < 8 && request.perm >= 0)
		{
			int chmodPerm = oct2Dec(request.perm);	// Converts user permissions to decimal for chmod()
			
			/* Creates file if doesnt exist */
			if ((fd = open(request.text,O_WRONLY | O_CREAT | O_TRUNC, chmodPerm)) != -1)
			{
				response.type = ACK;
				n = write(sock, &response, sizeof(packet));

				int i=0;
				char file[request.size];
				/* Loop downloads file byte by byte from client */
				for (i=0; i<request.size; i++)
				{
					read(sock, &file[i], 1);
				}
				write(fd, &file, request.size);	// writes file contents
				close(fd);
				chmod(request.text, chmodPerm);	// set file permissions
			
			}else{
					/* Files exists and does not have write permissions when overwriting */
					response.type = ERR;
					strcat(response.text, "Error overwriting file on server: Does not have necessary write permissions!!\n");
					n = write(sock, &response, sizeof(packet));
				}

		}else{
				/* File doesnt exist */
				response.type = ERR;
				strcat(response.text, "Error: File does not exist!!\n");
				n = write(sock, &response, sizeof(packet));
			}
	}	// end of "put file" service	

}

/*
 * "File execution" service 
 */
void execServer (int sock)
{
	int n;						// return value of read/write
	int pidFork;				// PID of fork
	char buffer[256];			// buffer
	packet request, response;	// packets
	char *buff;					// buffer

	bzero(response.text,1025);
	bzero(buffer,255);	
	n = read(sock, &request, sizeof(packet));	// gets client packet
	if (n < 0) error("ERROR reading from socket");
	char**args = malloc(sizeof(char*)*10);	// allocate memory for execv() argumetns

	if (request.type == EXE)
	{
		int i=0;
		
		/* Tokenize command to be executed */
		buff = strtok(request.text, " ");
		while (buff != NULL)
		{
			args[i] = buff;
			buff = strtok (NULL, " ");
			i++;
		}
		
		/* Forks off process to deal with execution */
		if ((pidFork = fork()) == 0)
		{
			/* Checks is file exists and has execution permissions */
			if (access(args[0], X_OK) == 0)
			{
				response.type = ACK;
				n = write(sock, &response, sizeof(packet));	
				
				/* Replaces stdout and stderr of executed file with socket */
				close(1);
				close(STDERR_FILENO);
				dup2(sock, STDERR_FILENO);
				dup2(sock,1);
				close(sock);

				printf("\n");	// Executed program output is sent through the client socket conection
				execv(args[0],args);	// Executes program and its arguments
			}else{
					/* File doesnt exist/does not have execution permissions */
					response.type = ERR;
					strcat(response.text, "Error: File does not have execution permissions or does not exist!!\n");
					n = write(sock, &response, sizeof(packet));	
				}  		
		}


	}
}

/* Octal to decimal converter 
 * 
 * Converts user requested permissions to decimal so it can be
 * used with chmod() to change permissions.
 *  
 */
int oct2Dec(int oct)
{
	int n,r,s=0,i;
	n=oct*100;
	for(i=0;n!=0;i++)
	{
		r=n%10;
		s=s+r*(int)pow(8,i);
		n=n/10;
	}
	return (s);	// returns decimal number
}
