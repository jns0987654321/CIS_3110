#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <fcntl.h>

#define MAXSTR 1025
#define PORT1 3000
#define PORT2 3001
#define PORT3 3002

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
    int sockfd, portno, n, fd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    packet request, response;
    
    if (argc < 3) 
    {
       fprintf(stderr,"usage resec -t host \nusage resec -f host command [filename] [perm] \nusage resec -e host \"command\" ");
       exit(0);
    }
    
    if (argc == 3)
    {
		if (strcmp(argv[1],"-t") == 0)
			portno = PORT1;
	}
	if (argc == 4)
    {
		if (strcmp(argv[1],"-f") == 0)
			portno = PORT2;
	}
	if (argc == 5)
	{
		if (strcmp(argv[1],"-f") == 0)
			portno = PORT2;
	}
	if (argc == 6)
	{
		if (strcmp(argv[1],"-f") == 0)
			portno = PORT2;
	} 
	if (argc >= 4)
	{
		if (strcmp(argv[1],"-e") == 0)
			portno = PORT3;
	}
		
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[2]);
    if (server == NULL) {
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
        
    printf("ready\n");
    if (portno == PORT1)
    {        char buffer[256];

		printf("we in port 1\n");
		bzero(buffer,256);
				printf("we in port 1ass\n");

		n = read(sockfd,buffer,255);
				printf("we in port 1big\n");

			if (n < 0){ 
				error("ERROR reading from socket");
				exit(1);}
		printf("Time1 is %s\n",buffer);
		close(n);
		close(sockfd);
	}
	
	if (portno == PORT2)
	{    

				printf("we in port 2 \n");
		if (argc == 4){
		if (strcmp(argv[3],"list") == 0)
		{				
			request.type = LIS;
			n = write(sockfd, &request, sizeof(packet));
				if (n < 0){ 
				error("ERROR writing to socket");
				exit(1);}
			n = read(sockfd, &response, sizeof(packet));
			if (n < 0){ 
				error("ERROR reading from socket");
				exit(1);}
				if (response.type == ACK)
				{
					n = read(sockfd, &response, sizeof(packet));
					printf("%s\n", response.text);
				}
				if (response.type == ERR)
				{
					n = read(sockfd, &response, sizeof(packet));
					printf("%s\n", response.text);
				}
				

		}else{
	//	printf("Command invalid!\n");
			request.type = ERR;
			n = write(sockfd, &request, sizeof(packet));
				if (n < 0){ 
				error("ERROR writing to socket");
				exit(1);}
			n = read(sockfd, &response, sizeof(packet));
			if (n < 0){ 
				error("ERROR reading from socket");
				exit(1);}
			printf("%s\n", response.text);
		}
		}
		
		if (argc == 5)
		{
			
			if (strcmp(argv[3],"get") == 0)
			{	
					request.type = GET;
					strcpy(request.text,argv[4]);
			n = write(sockfd, &request, sizeof(packet));
				if (n < 0){ 
				error("ERROR writing to socket");
				exit(1);}
			n = read(sockfd, &response, sizeof(packet));
			if (n < 0){ 
				error("ERROR reading from socket");
				exit(1);}
			if(response.type == ACK)
			{
				char file[response.size];
				fd = open(argv[4],O_CREAT/O_RDWR);
				n = read(sockfd, &file, response.size);
				close(fd);

				printf("created!!!\n");
				
				
				
			}
			if (response.type == ERR)
			{
				printf("%s\n", response.text);
				
			}
				
		}else{
		printf("Command invalid!\n");
			request.type = ERR;
			n = write(sockfd, &request, sizeof(packet));
				if (n < 0){ 
				error("ERROR writing to socket");
				exit(1);}
			n = read(sockfd, &response, sizeof(packet));
			if (n < 0){ 
				error("ERROR reading from socket");
				exit(1);}
			printf("%s\n", response.text);
		}
			
		}
		
		if (argc == 6)
		{
			
			if (strcmp(argv[3],"put") == 0)
			{	
			
			
			
			}else
			printf("Command invalid");
		}
	}
	
	if (portno == PORT3)
	{    char buffer[MAXSTR];
		int i;
		printf("we in port 3 \n");
		
		request.type = EXE;
		bzero(buffer, 1024);
		request.size = argc;
		for(i=3; i<argc; i++)
		{
			strcat(buffer, argv[i]);
			strcat(buffer, " ");
		}
		strcpy(request.text, buffer);
			n = write(sockfd, &request, sizeof(packet));
				if (n < 0){ 
				error("ERROR writing to socket");
				exit(1);}
			n = read(sockfd, &response, sizeof(packet));
			if (n < 0){ 
				error("ERROR reading from socket");
				exit(1);}
			if (response.type == ERR)
			{
				printf("%s\n", response.text);
			}
						//	printf("HHWW \n");

						//n = write(sockfd, &response, sizeof(packet));

			if (response.type == ACK)
			{
				char *buer = NULL;
				bzero(buffer, 1024);
				n = read(sockfd, &buer, sizeof(buer));
				printf("%s [%d]\n ", buer, sizeof(buer);
			//	n = read(sockfd, &response, sizeof(packet));
			//	printf("ack rwo %s\n", response.text);
			}
									//	printf("HH333WW \n");

		
	}
	
	
	
    return 0;
}

