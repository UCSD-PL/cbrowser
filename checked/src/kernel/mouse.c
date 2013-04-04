#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <search.h>
// socket includes
#include <sys/types.h>
#include <sys/socket.h>

#include "str.h"
#include "proc.h"
#include "kernel.h"
#include "assert.h"

#define COMMAND "../../../trusted/src/procs/mousepos.py"

char *read_mouse(void)
{
  FILE *p;
  char *mousepos = malloc(256); //should be enough
  ssize_t n;
  p = popen(COMMAND, "r");
  if (p == NULL) {
    fprintf(stderr, "K: Error running mousepos.py\n");
    exit(1);
    return NULL; //durr
  }
  if ((n = fread(mousepos, 1, 256, p)) < 0) {
    fprintf(stderr, "K: Error reading mousepos.py\n");
    exit(1);
    return NULL; //durr
  }
  mousepos[n] = 0;
  return mousepos;
}
