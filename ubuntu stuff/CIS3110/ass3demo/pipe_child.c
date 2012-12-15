/*
 * pipe_child.c : Simple echo - reads from stdin -> parent initialized this
 *                to be read side of a pipe.
 */

#include <stdlib.h>
#include <stdio.h>

#define MAXSTR 257

int main(void)
{
	char str[MAXSTR];

	/*
	 * Just read from stdin - echo to stdout (screen)
	 * NOTE: inherits fd table initialized prior to exec!
	 */
	while (fgets(str,MAXSTR,stdin) != NULL)
		printf("%s", str);

	return(0);

}
