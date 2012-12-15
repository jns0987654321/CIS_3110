/*
 * server2.c : simple echo server - allows client to connect and echos strings
 *            received over socket connection to local terminal (stdout)
 * 
 * modification: loops so that it can handle connections over and over
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

int main(int argc, char *argv[])
{
	unsigned short port;		/* Port to which server will bind */
	char servhost[NAME];		/* Local host name */
	struct sockaddr_in sock;	/* INTERNET socket space */
	struct hostent *server;		/* Local host information */
	int S;						/* fd for socket */
	int NS;						/* fd for connected socket */
	char buf[BUFSIZE];			/* Input buffer */
	FILE *fp;					/* Stream (converted file des.) */

	if (argc != 2)
	{
		fprintf(stderr,"usage: server <port>\n");
		exit(1);
	}

	port = atoi(argv[1]);

	/*
	 * Get socket - INTERNET DOMAIN - TCP
	 */
	if ((S = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		ERROR("server: socket");

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
	
	
	//loop to continue handling connections
	while(1)
	{
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

		/*
		 * Using stdio library to read from socket
		 */
		if (!(fp = fdopen(NS,"r")))
		{
			fprintf(stderr,">>> Error converting file des. to stream <<<\n");
			exit(1);
		}

		while (fgets(buf,BUFSIZE,fp))
			printf("%s", buf);

		/*
		 * DONE - simply close() connection
		 */
		fclose(fp);
		close(NS);
	}
	
	return(0);
}
