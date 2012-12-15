/*
 * pause-resume.c
 * This program shows how a process may be paused or resumed by
 * SIGSTOP and SIGCONT
 * Jason Ernst - University of Guelph
 * CIS*3110 W11
 */
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

int main(int argc, char * argv[])
{
  pid_t pid;
  pid = fork();

  //child
  if(pid == 0)
    while(1)
      printf("Child executing...\n");
  else
  {
    sleep(2);
    kill(pid, SIGSTOP);
    printf("\nChild paused\n");
    sleep(2);
    kill(pid, SIGCONT);
    sleep(2);
    kill(pid, SIGKILL);
  }
  return 0;
}
