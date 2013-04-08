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
//#include "tags.h"

#define WGET_CMD "/usr/bin/wget -q --tries=1 --timeout=1 -O - -U 'Mozilla/5.0 (X11; Linux i686) AppleWebKit/534.30 (KHTML, like Gecko) Ubuntu/11.04 Chromium/12.0.742.112 Chrome/12.0.742.112 Safari/534.30' '"

kstr NNREF(TAGSET([V]) = TAGSET([uri]))
wget(kstr uri) CHECK_TYPE
{
  size_t n = 0;
  size_t len = 0;
  size_t csize = 2048;
  char * content;
  char * command;
  FILE *p;

  fprintf(stderr, "K: wget(%s)", uri);
  command = malloc(sizeof(WGET_CMD) + MAX_URI_LEN);

  snprintf(command, sizeof(WGET_CMD) + MAX_URI_LEN, WGET_CMD" %s'", uri);
//  command = xfer_tags(command, uri);

  p = popen(command, "r");
  if (p == NULL) { //Fix spec of popen
    fprintf(stderr, "K: Error running wget\n");
    exit(1);
    return NULL; //exit
  }

  content = calloc(csize, 1);

//  content = xfer_tags(content, p);
  while ((n = fread(content + len, 1, csize - len, p)) == csize - len) {
    csize *= 2;
    content = realloc(content, csize);
    len += n;
    if (content == NULL) {
      fprintf(stderr, "k: error reading wget result");
      exit(1);
      return NULL; //exit
    }
  }

  len += n;
  if (content != NULL) {
    //Make sure the string is null
    //terminated
    content[len] = 0;
  }

  pclose(p);
  free(command);

  return content;
}
