#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int
main (void)
{
  char *cmd = "puppet apply --help 2>&1";
  char buf[BUFSIZ];
  FILE *ptr;
  int stat;

  if ((ptr = popen (cmd, "r")) != NULL)
    while (fgets (buf, BUFSIZ, ptr) != NULL)
      (void) printf ("%s", buf);
  stat = pclose (ptr);
  printf ("The exit status is: %d\n", WEXITSTATUS (stat));
  exit (0);
}
