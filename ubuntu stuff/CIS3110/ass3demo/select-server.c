/*
 * select-server.c : 
 * simple echo server - allows client to connect and echos strings
 * received over socket connection to local terminal (stdout)
 * 
 * modification: now allows connections to occur simultaneously
 * (before if we ran connections in a loop, the second connection would
 *  not be displayed until the first terminated with end line)
 * 
 * For more info see: http://www.gnu.org/s/libc/manual/html_node/Server-Example.html#Server-Example
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

#define ERROR(x) do { perror(x); exit(1); } while(0)

#define NAME       257
#define BUFSIZE    257
#define MAXPENDING 1

void handle_connection(int NS, fd_set * activefds);

int main(int argc, char *argv[])
{
	fd_set readfds, activefds;	/* the set of read descriptors */
	unsigned short port;		/* Port to which server will bind */
	char servhost[NAME];		/* Local host name */
	struct sockaddr_in sock;	/* INTERNET socket space */
	struct hostent *server;		/* Local host information */
	int S;						/* fd for socket */
	int NS;						/* fd for connected socket */
	int pid;					/* used to determine parent or child */
	int i;						/* counter to go through FDs in fdset */

	if (argc != 2)
	{
		fprintf(stderr,"usage: select-server <port>\n");
		exit(1);
	}

	port = atoi(argv[1]);

	/*
	 * Get socket - INTERNET DOMAIN - TCP
	 */
	if ((S = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		ERROR("select-server: socket");

	/*
	 * Obtain host name & network address
	 */
	gethostname(servhost, sizeof(servhost));
	if ((server = gethostbyname(servhost)) == NULL)
	{
		fprintf(stderr,"%s: unknown host\n",servhost);
		exit(1);
	}

	/*
	 * Bind to server address - in network byte order
	 */
	sock.sin_family = AF_INET;
	sock.sin_port = htons(port);
	memcpy(&sock.sin_addr, server->h_addr, server->h_length);

	/*
	 * Bind socket to port/addr
	 */
	if (bind(S, (struct sockaddr *)&sock, sizeof(sock)) < 0)
		ERROR("server: bind");

	/*
	 * Listen on this socket
	 */
	if (listen(S,MAXPENDING) < 0)
		ERROR("server: listen");
	
	/* Initialize the set of active sockets. */
    FD_ZERO (&activefds);
    FD_SET (S, &activefds);
	
	while(1)
	{
		/* Block until input arrives on one or more active sockets. */
        readfds = activefds;
        if (select (FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0)
        {
			perror ("select");
            exit (EXIT_FAILURE);
        }
		
		/* Service all the sockets with input pending. */
		for (i = 0; i < FD_SETSIZE; ++i)
		{
			if (FD_ISSET (i, &readfds))
			{
				if (i == S)
				{
					/* Connection request on original socket. */

					/*
					 * Accept connections.  Once connected, the client will be
					 * connected on fd NS, a second and third parameter may be passed
					 * to accept which will be filled in with information regarding the
					 * client connection if desired.
					 *
					 * In this example, once connected the server is done with the
					 * master socket (so closes it).
					 */
					if ((NS = accept(S,NULL,NULL)) < 0)
						ERROR("server: accept");
					FD_SET(NS, &activefds);	//add the new socket desc to our active connections set
				}
				else
				{
					/* Data arriving on an already-connected socket. */
					handle_connection(i, &activefds);
				}
			}
		} /* //end of for */
	} /* //end of while */

	return(0);
}

void handle_connection(int NS, fd_set * activefds)
{
	char buf[BUFSIZE];			/* Input buffer */
	FILE *fp;					/* Stream (converted file des.) */
		
	/*
	 * Using stdio library to read from socket
	 */
	if (!(fp = fdopen(NS,"r")))
	{
		fprintf(stderr,">>> Error converting file des. to stream <<<\n");
		exit(1);
	}

	//if fgets fails we have end of line, quit
	if(!(fgets(buf,BUFSIZE,fp)))
	{
		/*
		 * DONE - simply close() connection
		 */
		fclose(fp);
		close(NS);
		FD_CLR(NS, activefds);
	}
	else
		printf("%s", buf);
}
