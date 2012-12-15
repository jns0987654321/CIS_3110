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

#define MAXSTR 1025
#define PORT1 3000
#define PORT2 3001
#define PORT3 3002

void dostuff(int); /* function prototype */

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
	int pidFork, i;
	signal(SIGCHLD,SIG_IGN);
	
	int S, NS, pid;
		fd_set readfds, activefds;	/* the set of read descriptors */
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     
     S = socket(AF_INET, SOCK_STREAM, 0);
			    if (S < 0) 
			       error("ERROR opening socket");
			    bzero((char *) &serv_addr, sizeof(serv_addr));
			    serv_addr.sin_family = AF_INET;
				serv_addr.sin_addr.s_addr = INADDR_ANY;
				serv_addr.sin_port = htons(3001);
			             				if (bind(S, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
			             error("ERROR on binding one");
			             listen(S,5);
	
			    clilen = sizeof(cli_addr);
	
	if ((pidFork = fork()) == 0)
		{
			while(1)
			{

				NS = accept(S, (struct sockaddr *) &cli_addr, &clilen);
				
			        if (NS < 0) 
			            error("ERROR on accept");
			        pid = fork();
			        if (pid < 0)
			            error("ERROR on fork");
			        if (pid == 0)  {
						close(S);
			            dostuff(NS);
			            exit(0);
			        }
			        else close(NS);
			}		
				
				 /* //end of for */
	 /* //end of while */
			   // return (0);
				
			//}	
		}
	
	return (0);
}

void dostuff (int sock)
{
	FILE *fp;
   int n;
   char buffer[256];
   packet request, response;

   bzero(buffer,256);
   n = read(sock, &request, sizeof(packet));
   if (n < 0) error("ERROR reading from socket");
   if (request.type == LIS)
	{
			printf("listing\n");
			fp = popen("ls", "r"); 
			while (fgets(buffer, sizeof(buffer), fp))
					strcat(response.text, buffer);
					response.type = ACK;
	}else{
		response.type = ERR;
		strcat(response.text, "Error listing files!!..ERRR");
		}
	n = write(sock, &response, sizeof(packet));
				printf(" this is %s\n", response.text);



	
   
}
