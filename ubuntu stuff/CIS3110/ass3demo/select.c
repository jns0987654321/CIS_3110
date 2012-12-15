/*
 * select.c -- a select() demo
 * source: http://beej.us/guide/bgnet/output/html/multipage/advanced.html#select
 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define STDIN 0  // file descriptor for standard input

int main(void)
{
	struct timeval tv;
	fd_set readfds;

	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	FD_ZERO(&readfds);		//clear the set readfs
	FD_SET(STDIN, &readfds);	//add stdin to the set

	// don't care about writefds and exceptfds:
	select(STDIN+1, &readfds, NULL, NULL, &tv);		//note the use of stdin + 1 for first param

	if (FD_ISSET(STDIN, &readfds))
		printf("A key was pressed!\n");
	else
		printf("Timed out.\n");

	return 0;
}

