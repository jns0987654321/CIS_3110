#include <stdio.h>
#include <unistd.h>
#include <signal.h>
void SIGINT_handler( int signum );
void SIGQUIT_handler( int );
void SIGUSR1_handler( int signum );
int main()
{
	/* attach handler for SIGINT and SIGQUIT and SIGUSR1 */
	//signal(SIGINT,SIG_DFL);
	//signal(SIGINT,SIG_IGN);
	//signal(SIGINT, SIGINT_handler);
	signal(SIGQUIT, SIGQUIT_handler);
	//signal(SIGUSR1, SIGUSR1_handler);

	while(1)
	{
		printf("Program executing (1s delay between updates)...\n");
		sleep(1);
		// sleep() is a system call and will be interrupted
		// when a signal arrives, e.g. raise(SIGUSR1);
	}
	return(0);
}

void SIGQUIT_handler(int signum)
{
	printf("<--signal\n>>SIGQUIT's new handler-Caught signal: %d;<<\n\n", signum);
		   }
		   void SIGINT_handler( int signum )
		   {
		   pid_t iPid = getpid();
		   printf("<--signal\n>> SIGQUIT's new handler: Caught signal: %d; ProcessID: %d <<\n\n", signum, iPid);
		   kill(iPid, SIGUSR1);
		   // getpid() is to get the process id of the executing program (i.e. self)
		   }
		   void SIGUSR1_handler( int signum )
		   {
		   printf("\n>> SIGUSR1's user_defined_handler: Caught signal: %d; <<\n\n", signum);
		   kill( getpid(), SIGINT);
		   }