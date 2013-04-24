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

  if (domain[0] != '.') {
    if (asprintf(&cur, ".%s", domain) < 1) {
      //Error
      exit(1);
    }
  } else {
    cur = domain;
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
    if (cur) {
      next = strchr(cur+1, '.');
    }
  }
  if (i < n) {
    d[i] = 0;
  }

  return d;
}

char *
discard_http(char *s)
{
  char *clean_s;

  printf("COOKIE DISCARD http:// %s", s);
  if (clean_s = strstr(s, "http://")) {
    return strdup(clean_s+strlen("http://"));
  } else {
    return strdup(s);
  }
}

int
check_cookie_domain(char *c_domain, char *host_domain)
{
  char *s1, *s2;
  char *d1, *d2;
  s1 = discard_http(c_domain);
  s2 = discard_http(host_domain);
  if (*s1 != '.') { 
    asprintf(&d1, ".%s", s1);
  } else {
    d1 = s1;
  }
  if (*s2 != '.') { 
    asprintf(&d2, ".%s", s2);
  } else {
    d2 = s2;
  }
  printf("CHECK COOKIE %s %s", d1, d2);
  return (NULL != strstr(d1,d2) ||
          NULL != strstr(d2,d1) ||
          NULL != strstr(d1, "gstatic.com"));
}

void
free_cookie(SoupCookie *c)
{
  soup_cookie_free(c);
}
