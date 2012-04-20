#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "listener.h"
int main(int argc, char *argv[])
{
  Mcollective::Listener a;
  a.StartThreads();
  sleep(101010100);
}
