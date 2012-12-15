/*
 * client.c : simple "echo" client---connects to server, reads strings from
 *            stdin, sends over internet domain socket to server for display
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ERROR(x) do { perror(x); exit(1); } while(0)

#define BUFSIZE 257

int main(int argc, char *argv[])
{
	char *hostname;			/* Host on which to connect to server */
	unsigned short port;	/* Port on which server is listening  */
	int S;					/* fd for socket */
	char buf[BUFSIZE];		/* Input/output buffer */
	FILE *fp;				/* Stream (converted file des.) */

	struct sockaddr_in sock;	/* INTERNET socket space */
	struct hostent *remote;		/* Remote host information */

	if (argc != 3)
	{
		fprintf(stderr,"usage: client <hostname> <port>\n");
		exit(1);
	}
	
	hostname = argv[1];
	port = atoi(argv[2]);
	
	/*
	 * Obtain socket - INTERNET DOMAIN - TCP
	 */
	if ((S = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		ERROR("client: socket");

	/*
	 * Get network address of server
	 */
	if ((remote = gethostbyname(hostname)) == NULL)
	{
		fprintf(stderr,"%s: unknown host\n", hostname);
		exit(1);
	}

	/*
	 * Set address of server - converting to network byte order
	 */
	sock.sin_family = AF_INET;
	sock.sin_port = htons(port);
	memcpy(&sock.sin_addr,remote->h_addr,remote->h_length);

	/*
	 * Attempt connection to server - must be listening for success
	 */
	if (connect(S, (struct sockaddr *)&sock, sizeof(sock)) < 0)
		ERROR("client: connect");

	/*
	 * Using stdio library to write to socket
	 */
	if (!(fp = fdopen(S,"w")))
	{
		fprintf(stderr,">>> Error converting file des. to stream <<<\n");
		exit(1);
	}

	/*
	 * Read input from terminal, echo to server (terminate on newline)
	 */
	while(1)
	{
		fgets(buf,BUFSIZE,stdin);
		if (*buf == '\n')
			break;
		else
		{
			fputs(buf,fp);
			fflush(fp);
		}
	}

	/*
	 * DONE - simply close() connection
	 */
	fclose(fp);
	close(S);

	return(0);

}
