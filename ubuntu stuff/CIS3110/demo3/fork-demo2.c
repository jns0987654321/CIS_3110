/*
 * Fork demo2 - CIS*3110, W11
 * Jason Ernst, University of Guelph
 *
 * Things to note:
 * - Parent has pid 0, children have their own pid
 * - The repeat value is independent in each process, so each will print 5 times
 * - The order the processes are executed is not always the same
 */
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char * argv[])
{
  int pid;
  int repeat;
  repeat = 0;

  printf("Before I create the child...\n");

  pid = fork();
  while(repeat < 5)
  {
    if(pid == 0)
      printf("Child: ");
    else
      printf("Parent: ");
    printf("repeat: %d\n", repeat);
    repeat++;
  }
}
