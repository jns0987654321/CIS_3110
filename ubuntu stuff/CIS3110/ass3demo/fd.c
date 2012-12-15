/*
 * fd.c : replaces f.d. 0 (stdin) with that of provided file, echoing
 *        contents to stdout.
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSIZE 257

int main(int argc, char *argv[])
{
	char buf[BUFSIZE];
	int fd;

	if (argc != 2)
	{
		fprintf(stderr,"usage: fd <filename>\n");
		exit(1);
	}

	/*
	 * Open provided file for reading (low-level open)
	 */
	if ((fd = open(argv[1],O_RDONLY)) < 0)
	{
		fprintf(stderr,">>> Error opening file (%s) <<<\n",argv[1]);
		exit(1);
	}

	/*
	 * Close stdin (fd 0), replace (dup) with new file.
	 * NOTE: can close old file descriptor --- not needed
	 */
	close(0);
	dup2(fd,0);		/* Technically dup(fd) would suffice */
	close(fd);

	/*
	 * Read from stdin
	 */
	while (fgets(buf,BUFSIZE,stdin) != NULL)
		printf("%s", buf);

	return(0);
}
