/* 
 * Name: Junior Samarooo
 * Student ID #0663108
 * Filename: resec.c
 * 
 * CIS3110 Assignment 3
 * 
 * This program is a simple client requesting time, file management
 * and remote execution services from "resed".
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>
#include <ctype.h>

#define MAXSTR 1025		// max string size
#define PORT1 3000		// port 3000 - time service
#define PORT2 3001		// port 3001 - file management services
#define PORT3 3002		// port 3002 - remote execution service

/* Function prototypes */
void getTime();					// Gets current time of server
void getList();					// Gets list of files from server
void putFile(char *argv[]);		// Puts a file onto server
void getFile(char *argv[]);		// Gets a file from server
void execFile(char *argv[]);	// Executes a file on the server

typedef enum { LIS, GET, PUT, EXE, ACK, ERR } PacketType;

/* Packet structure */
typedef struct
{
	PacketType type;
	int size;
	int perm;
	char text[MAXSTR];
} packet;

/* Error Handler */
void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int sockfd, fd;				// File descriptors
int portno;					// Port required to connect to server
int n;						// Return value of read/write
packet request, response;	// Packets

int main(int argc, char *argv[])
{
	struct sockaddr_in serv_addr;	// Socket buffer
	struct hostent *server;			// Buffer
	
	/* Determination of required Port */	
	if (argc < 3) 
	{
		fprintf(stderr,"usage: resec -t host \nusage: resec -f host command [filename] [perm] \nusage: resec -e host \"command\" \n");
		exit(0);
	}
	if (argc == 3)
	{
		if (strcmp(argv[1],"-t") == 0)
		portno = PORT1;
	}
	if (argc == 4)
	{
		if ((strcmp(argv[1],"-f") == 0) && (strcmp(argv[3],"list") == 0))
			portno = PORT2;
		if (strcmp(argv[1],"-e") == 0)
			portno = PORT3;
	}
	if (argc == 5)
	{
		if ((strcmp(argv[1],"-f") == 0) && (strcmp(argv[3],"get") == 0))
		portno = PORT2;
	}
	if (argc == 6)
	{
		if ((strcmp(argv[1],"-f") == 0) && (strcmp(argv[3],"put") == 0))
		portno = PORT2;
	} 
	
	/*
	 * Obtain socket - INTERNET DOMAIN - TCP
	 */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
		
	/*
	 * Get network address of server
	 */
	server = gethostbyname(argv[2]);
	if (server == NULL) 
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	
	/*
	 * Set address of server - converting to network byte order
	 */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
	serv_addr.sin_port = htons(portno);
	
	/*
	 * Attempt connection to server - must be listening for success
	 */
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");

	if (portno == PORT1)
	{        
		getTime();	// Call "time" function
	}

	if (portno == PORT2)
	{    
		if (argc == 4)
		{
			if (strcmp(argv[3],"list") == 0)
			{
				getList();	// Call "getlist" function			
			}
		}

		if (argc == 5)
		{
			if (strcmp(argv[3],"get") == 0)
			{	
				getFile(argv);	// Call "getFile" function
			}
		}

		if (argc == 6)
		{
			if (strcmp(argv[3],"put") == 0)
			{	
				putFile(argv);	// Call "putFile" function
			}
		}
	}

	if (portno == PORT3)
	{    
		execFile(argv);	// Call file execution function
	}

	return 0;
}

/* Gets current time from server 
 * 
 * Expects a 26character string else outputs
 * server provided error;
 * 
 */
void getTime()
{
	char buffer[26];
	bzero(buffer,26);
	n = read(sockfd,buffer,26);

	if (n != 26)
	{ 
		printf("ERROR getting time!\n");
	}else{
			printf("%s\n",buffer);
	}
	close(n);
	close(sockfd);
}

/* Gets a list of files in the working directory of server 
 * 
 * If there is a error getting the list, outputs server provided
 * error message.
 * 
 */
void getList()
{
	request.type = LIS;
	n = write(sockfd, &request, sizeof(packet));	// writes to socket
	if (n < 0)
	{ 
		error("ERROR writing to socket");
		exit(1);
	}
	
	n = read(sockfd, &response, sizeof(packet));	// reads from socket
	if (n < 0)
	{ 
		error("ERROR reading from socket");
		exit(1);
	}
	
	/* If listing is successfull */
	if (response.type == ACK)
	{
		char list[MAXSTR];
		bzero(list, MAXSTR);
		n = read(sockfd, &list, sizeof(list));
		printf("%s\n", list);
	}
	
	/* If listing is unsuccessfull */
	if (response.type == ERR)
	{
		printf("%s\n", response.text);
	}
}

/*	Puts a file from client working directory to the server's working
 * 	direcotry with the indicated permissions.
 * 
 * 	If the file already exists on the servers end, it must have necessary
 * 	writing permissions to be overwritten. Also the file being put must exist.
 * 
 */
void putFile(char *argv[])
{
	/* Checks if file being put exists or not */
	if (access(argv[4], R_OK) == 0)
	{
		bzero(request.text,1025);
		request.type = PUT;
		strcat(request.text, argv[4]);
		request.perm = atoi(argv[5]);

		struct stat buf;	// Required for file information
		stat(argv[4], &buf);
		request.size = buf.st_size;
		write(sockfd, &request, sizeof(packet));	// Send packet to server
		read(sockfd, &response, sizeof(packet));	// Read server packet info
		
		if(response.type == ACK)
		{
			char file[request.size];
			int fd;
			fd = open(request.text,O_RDWR);
			read(fd, &file, request.size);

			int i=0;
			/* Transfers file to server byte by byte */
			for (i=0; i<request.size; i++)
			{
				write(sockfd, &file[i], 1);
			}
			close(fd);
			
		}
		
		/* If overwriting file does not have writing permissions */
		if(response.type == ERR)
			printf("%s\n", response.text);

	}else{
			request.size = 0; // File doesnt exist so size is zero
			request.type = PUT;
			strcat(request.text, argv[4]);
			request.perm = atoi(argv[5]);
			write(sockfd, &request, sizeof(packet));
			read(sockfd, &response, sizeof(packet));
			if(response.type == ERR)
				printf("%s\n", response.text);
		}	
}

/* Downloads a file from the servers working directory to clients working directory 
 * 
 * 	File being requested must exist and have reading permissions.
 * 
 */
void getFile(char *argv[])
{
	request.type = GET;
	strcpy(request.text,argv[4]);
	n = write(sockfd, &request, sizeof(packet));	// Write packet info
	if (n < 0)
	{ 
		error("ERROR writing to socket");
		exit(1);
	}
	
	n = read(sockfd, &response, sizeof(packet));	// Read server packet info
	if (n < 0)
	{ 
		error("ERROR reading from socket");
		exit(1);
	}
	
	if(response.type == ACK)
	{
		char file[response.size];	// File contents buffer
		fd = open(argv[4],O_WRONLY | O_CREAT | O_TRUNC, response.perm);	// Creates file with permissions if it doesnt exist
		
		int i=0;
		/* Loop downloads file from server byte by byte */
		for (i=0; i<response.size; i++)
		{
			read(sockfd, &file[i], 1);
		}				
		write(fd, &file, response.size);
		close(fd);
		chmod(argv[4], response.perm);	// Changes permission of file if it already exists and has writing permissions
	}
	
	/* If file doesnt exist or doesnt have reading permissions */
	if (response.type == ERR)
	{
		printf("%s\n", response.text);
	}
}

/* Executes file on server with program output displayed in stdout of client
 * 
 * File being executed must exist and must have execution permissions.
 * File descriptor is modified to have the executed file ouput displayed
 * in the client.
 * 
 */
void execFile(char *argv[])
{
	/* Set packet information */
	char buffer[MAXSTR];
	request.type = EXE;
	bzero(buffer, 1024);
	strcpy(request.text, argv[3]);
	n = write(sockfd, &request, sizeof(packet));	// Write packet to srver
	if (n < 0)
	{ 
		error("ERROR writing to socket");
		exit(1);
	}
	
	n = read(sockfd, &response, sizeof(packet));	// Read response from server
	if (n < 0)
	{ 
		error("ERROR reading from socket");
		exit(1);
	}
	
	/* If file on server doesnt exist or have execution permissions */
	if (response.type == ERR)
	{
		printf("%s\n", response.text);
	}

	if (response.type == ACK)
	{
		char output;
		/* Loop displays program output in client until EOF */
		do{
			n = read(sockfd, &output, 1);
			printf("%c", output);
			}while(n != 0);
	}
}

