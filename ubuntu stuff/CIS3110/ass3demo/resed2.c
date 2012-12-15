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

#define MAXSTR 1025
#define PORT1 3000
#define PORT2 3001
#define PORT3 3002

void timeServer(int); /* function prototype */
void fileServer(int);
void execServer(int);
int oct2Dec(int oct);

typedef enum { LIS, GET, PUT, EXE, ACK, ERR } PacketType;

typedef struct
{
	PacketType type;
	int size;
	int perm;
	char text[MAXSTR];
} packet;

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int pidFork;
	int i;
	int S[3], pid;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	
	signal(SIGCHLD,SIG_IGN);

	for (i=0; i<3; i++)
	{
		S[i] = socket(AF_INET, SOCK_STREAM, 0);
		if (S[i] < 0) 
			error("ERROR opening socket!\n");
		fcntl(S[i], F_SETFL, O_NONBLOCK);
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(PORT1+i);
		if (bind(S[i], (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
			error("ERROR on binding!\n");
		listen(S[i],5);
	}
	clilen = sizeof(cli_addr);

	if ((pidFork = fork()) == 0)
	{
		while(1)
		{		
			int i=0;
			for (i=0; i<3; i++)
			{
				int NS;
				if((NS = accept(S[i], (struct sockaddr *) &cli_addr, &clilen)) >0 )
				{
					pid = fork();
					if (pid < 0)
					error("ERROR on fork");
					if (pid == 0)  
					{
						close(S[i]);
						if (i == 0)
							timeServer(NS);
						if (i == 1)
							fileServer(NS);
						if (i == 2)
							execServer(NS);
						exit(0);
					}else close(NS);
				}


			}

		}				

	}
	
	return (0);
}

void timeServer (int sock)
{
	int n;
	char buffer[256];
	time_t rawtime;
	time ( &rawtime );

	bzero(buffer,256);
	n = write(sock,ctime (&rawtime),26);
}

void fileServer (int sock)
{
	FILE *fp;
	int n;
	int fd;
	int maxFileSize;
	char buffer[256];
	packet request, response;

	bzero(response.text,1025);
	bzero(buffer,255);

	n = read(sock, &request, sizeof(packet));
	if (n < 0) 
		error("ERROR reading from socket");

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
			n = write(sock, &list, sizeof(list));
		}else{
				response.type = ERR;
				strcat(response.text, "Error getting list of files!!\n");
				n = write(sock, &response, sizeof(packet));
			} 
	}

	if (request.type == GET)
	{
		int fd;

		if ((fd = open(request.text,O_RDONLY)) < 0)
		{
			response.type = ERR;
			strcat(response.text, "Error getting file!! File does not exist or does not have read permissions!!\n");
			n = write(sock, &response, sizeof(packet));
			close(fd);
		}else{
				struct stat buf;
				fstat(fd, &buf);
				int size = buf.st_size;
				int perm = buf.st_mode;

				maxFileSize = size;
				response.type = ACK;
				response.perm = perm;
				response.size = maxFileSize;
				n = write(sock, &response, sizeof(packet));
				char file[maxFileSize];
				read(fd, &file, maxFileSize);

				int i=0;
				for (i=0; i<maxFileSize; i++)
				{
					write(sock, &file[i], 1);
				}
				close(fd);
			}
	}

	if (request.type == PUT)
	{
		if (request.size != 0)
		{
			int chmodPerm = oct2Dec(request.perm);
			if ((fd = open(request.text,O_WRONLY | O_CREAT | O_TRUNC, chmodPerm)) != -1)
			{
				response.type = ACK;
				n = write(sock, &response, sizeof(packet));

				int i=0;
				char file[request.size];
				for (i=0; i<request.size; i++)
				{
					read(sock, &file[i], 1);
				}
				write(fd, &file, request.size);
				close(fd);
				chmod(request.text, chmodPerm);			
			
			}else{
					response.type = ERR;
					strcat(response.text, "Error overwriting file on server: Does not have necessary write permissions!!\n");
					n = write(sock, &response, sizeof(packet));
				}

		}else{
				response.type = ERR;
				strcat(response.text, "Error: File does not exist!!\n");
				n = write(sock, &response, sizeof(packet));
			}
	}	

}

void execServer (int sock)
{
	int n;
	int pidFork;
	char buffer[256];
	packet request, response;
	char *buff;

	bzero(response.text,1025);
	bzero(buffer,255);	
	n = read(sock, &request, sizeof(packet));
	if (n < 0) error("ERROR reading from socket");
	char**args =malloc(sizeof(char*)*10);

	if (request.type == EXE)
	{
		int i=0;
		buff = strtok(request.text, " ");
		while (buff != NULL)
		{
			args[i] = buff;
			buff = strtok (NULL, " ");
			i++;
		}
		
		if ((pidFork = fork()) == 0)
		{
			if (access(args[0], X_OK) == 0)
			{
				response.type = ACK;
				n = write(sock, &response, sizeof(packet));	
				
				close(1);
				close(STDERR_FILENO);
				dup2(sock, STDERR_FILENO);
				dup2(sock,1);		/* Technically dup(fd) would suffice */
				close(sock);

				printf("\n");
				execv(args[0],args);
			}else{
					response.type = ERR;
					strcat(response.text, "Error: File does not have execution permissions!!\n");
					n = write(sock, &response, sizeof(packet));	
				}  		
		}


	}
}

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
	return (s);
}
