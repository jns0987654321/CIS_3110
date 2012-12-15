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

#define MAXSTR 1025
#define PORT1 3000
#define PORT2 3001
#define PORT3 3002

void getTime();
void getList();
void putFile(char *argv[]);
void getFile(char *argv[]);
void execFile(char *argv[]);

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

int sockfd;
int portno;
int n;
int fd;
packet request, response;

int main(int argc, char *argv[])
{
	struct sockaddr_in serv_addr;
	struct hostent *server;
		
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

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	server = gethostbyname(argv[2]);
	if (server == NULL) 
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");

	if (portno == PORT1)
	{        
		getTime();
	}

	if (portno == PORT2)
	{    
		if (argc == 4)
		{
			if (strcmp(argv[3],"list") == 0)
			{
				getList();				
			}
		}

		if (argc == 5)
		{
			if (strcmp(argv[3],"get") == 0)
			{	
				getFile(argv);
			}
		}

		if (argc == 6)
		{
			if (strcmp(argv[3],"put") == 0)
			{	
				putFile(argv);
			}
		}
	}

	if (portno == PORT3)
	{    
		execFile(argv);
	}

	return 0;
}

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

void getList()
{
	request.type = LIS;
	
	n = write(sockfd, &request, sizeof(packet));
	if (n < 0)
	{ 
		error("ERROR writing to socket");
		exit(1);
	}
	
	n = read(sockfd, &response, sizeof(packet));
	if (n < 0)
	{ 
		error("ERROR reading from socket");
		exit(1);
	}
	
	if (response.type == ACK)
	{
		char list[MAXSTR];
		bzero(list, MAXSTR);
		n = read(sockfd, &list, sizeof(list));
		printf("%s\n", list);
	}
	
	if (response.type == ERR)
	{
		printf("%s\n", response.text);
	}
}

void putFile(char *argv[])
{
	if (access(argv[4], R_OK) == 0)
	{
		bzero(request.text,1025);
		request.type = PUT;
		strcat(request.text, argv[4]);
		request.perm = atoi(argv[5]);

		struct stat buf;
		stat(argv[4], &buf);
		request.size = buf.st_size;
		write(sockfd, &request, sizeof(packet));
		read(sockfd, &response, sizeof(packet));
		
		if(response.type == ACK)
		{
			char file[request.size];
			int fd;
			fd = open(request.text,O_RDWR);
			read(fd, &file, request.size);

			int i=0;
			for (i=0; i<request.size; i++)
			{
				write(sockfd, &file[i], 1);
			}
			close(fd);
			
		}
		
		if(response.type == ERR)
			printf("%s\n", response.text);

	}else{
			request.size = 0;
			request.type = PUT;
			strcat(request.text, argv[4]);
			request.perm = atoi(argv[5]);
			write(sockfd, &request, sizeof(packet));
			read(sockfd, &response, sizeof(packet));
			if(response.type == ERR)
				printf("%s\n", response.text);
		}	
}

void getFile(char *argv[])
{
	request.type = GET;
	strcpy(request.text,argv[4]);
	
	n = write(sockfd, &request, sizeof(packet));
	if (n < 0)
	{ 
		error("ERROR writing to socket");
		exit(1);
	}
	
	n = read(sockfd, &response, sizeof(packet));
	if (n < 0)
	{ 
		error("ERROR reading from socket");
		exit(1);
	}
	
	if(response.type == ACK)
	{
		char file[response.size];
		fd = open(argv[4],O_WRONLY | O_CREAT | O_TRUNC, response.perm);
		int i=0;
		for (i=0; i<response.size; i++)
		{
			read(sockfd, &file[i], 1);
		}				
		write(fd, &file, response.size);
		close(fd);
		chmod(argv[4], response.perm);
	}
	
	if (response.type == ERR)
	{
		printf("%s\n", response.text);
	}
}

void execFile(char *argv[])
{
	char buffer[MAXSTR];
	request.type = EXE;
	bzero(buffer, 1024);
	strcpy(request.text, argv[3]);
	
	n = write(sockfd, &request, sizeof(packet));
	if (n < 0)
	{ 
		error("ERROR writing to socket");
		exit(1);
	}
	
	n = read(sockfd, &response, sizeof(packet));
	if (n < 0)
	{ 
		error("ERROR reading from socket");
		exit(1);
	}
	
	if (response.type == ERR)
	{
		printf("%s\n", response.text);
	}

	if (response.type == ACK)
	{
		char output;
		do{
			n = read(sockfd, &output, 1);
			printf("%c", output);
			}while(n != 0);
	}
}

