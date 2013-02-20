#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "cookie_util.h"


//The policy seems to be that a request for www.foo.com should include
//cookies for .www.foo.com, www.foo.com, .foo.com, .com
char **
gettable_domains(char *domain)
{
  int    i = 0;
  int    n = 10;
  char **d = malloc(10*sizeof(*d));
  char  *cur, *next;

  assert (domain != NULL);

  if (asprintf(&cur, ".%s", domain) < 1) {
    //Error
    exit(1);
  }

  next = cur+1;
  while (cur) {
    if (i >= n) {
      d = realloc(d, (2*n)*sizeof(*d));
      n = 2*n;
    }
    d[i] = cur;
    i++;
    cur = next;
    next = strchr(cur+1, '.');
  }

  return d;
}

void
free_cookie(cookie *c)
{
  free(c->c_domain);
  soup_cookie_free(c->c_cookie);
  free(c);
}
