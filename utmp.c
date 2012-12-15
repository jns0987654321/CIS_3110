/*
 * How to read information from utmp
 * source: http://www.cpp-home.com/tutorials/119_4.htm
 * Jason Ernst, University of Guelph
 * CIS3110 W11
 */
#include <stdio.h>
#include <stdlib.h>
#include <utmp.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
int main(int argc, char * argv[])
{
  /* full path for the UTMP file on our sample system. */
  #define UTMP_PATH "/var/run/utmp"

  /* buffer to read one utmp record. */
  struct utmp utmp_entry;
  /* buffer to store a user name. */
  char user_name[UT_NAMESIZE+1];
  /* file descriptor for the "utmp" file. */
  int fd;

  /* open the utmp file for reading. */
  fd = open(UTMP_PATH, O_RDONLY);
  if (fd < 0) {
    perror("open");
    exit(1);
  }

  printf("Currently logged-in users:\n");
  /* start scanning the file, entry by entry. */
  while (1) {
    int rc = read(fd, &utmp_entry, sizeof(utmp_entry));
    if (rc < 0) {
        perror("read");
        exit(1);
    }
    if (rc == 0) /* end of file - exit the reading loop. */
        break;

    /* This is Linux specific - only records whose ut_type field is    */
    /* USER_PROCESS, represent logged in users. the rest are temporary */
    /* records of various types.                                       */
    if (utmp_entry.ut_type != USER_PROCESS)
        continue;

    /* copy the user name field to our user name variable. */
    strncpy(user_name, utmp_entry.ut_name, UT_NAMESIZE);
    /* make sure this string is terminated with a null character. */
    user_name[UT_NAMESIZE] = '\0';

    printf("'%s',", user_name);
  }

  printf("\n");
  close(fd);
  return 0;
}
