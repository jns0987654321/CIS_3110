/*
 * pipe_parent.c : Simple echo b/w parent/child process via pipe
 *                 (child reads from stdin - pipe f.d. inherited from parent)
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAXSTR 257

int main(void)
{
	int pipefds[2];
	int pid;
	FILE *fp;
	char str[MAXSTR];

	/*
	 * Create pipe
	 */
	if (pipe(pipefds) < 0)
	{
		fprintf(stderr,">>> Error creating pipe <<<\n");
		exit(1);
	}

	/*
	 * fork - child makes "read end" stdin & exec's
	 * NOTE: as fd manipulation occurs after fork, parent's stdin is unchanged
	 */
	if ((pid = fork()) == 0)
	{
		/* Child */
		dup2(pipefds[0],0);		/* Automatically close f.d. 0 */
		/*
		 * Now don't need pipe copy of descriptor, and won't be
		 * using write side of pipe --- close both
		 */
		close(pipefds[0]);
		close(pipefds[1]);

		execl("pipe_child","pipe_child",NULL);
	}

	/* 
	 * Due to exec(), child never executes this
	 */
	/* Parent */

	/*
	 * Convert write side of pipe to stream, close read side
	 */
	if (!(fp = fdopen(pipefds[1],"w")))
	{
		fprintf(stderr,">>> Error opening stream <<<\n");
		exit(1);
	}
	close(pipefds[0]);

	while(1)
	{
		fgets(str,MAXSTR,stdin);
		fputs(str,fp);
		fflush(fp);			/* ensure buffer is flushed */
	}

	fclose(fp);

	return(0);

}
