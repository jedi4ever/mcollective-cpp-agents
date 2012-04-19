#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "listener.h"
int main(int argc, char *argv[])
{
  Mcollective::Listener a;
  a.DoSomething();
  sleep(200000);
}
