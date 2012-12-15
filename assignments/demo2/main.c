#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
int main()
{
	fprintf("\n");
	int pid = fork();
	printf("previous pid= %d\n", getppid() );
	printf("present pid= %d\n", getpid() );
	printf("child pid= %d\n", pid);
	printf("group pid= %d\n", getpgrp() );
	
	if(pid == 0)
	{
		printf("Hello, everybody! I\'m a child program/process !\n");
	}
	else
	{
		printf("Hello, Guys and Girls! I\'m a parent process ! My child's process id [ pid ] is %d\n" , pid);
	}
	printf("This one you will see in both programs!! It should be printed twice. Bye!\n\n\n");
	return(0);
}