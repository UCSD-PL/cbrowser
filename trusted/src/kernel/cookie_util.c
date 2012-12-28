#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>

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
      d = realloc(d, (2*n)*sizoef(*d));
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
may_get_cookie(char *domain, struct cookie *c)
{
}

int
may_get_cookies(char *d1, char *d2)
{
  char **domains = gettable_domains(d1);
  char *d;

  while (d = *domains) {
    if (!strcmp(d, d2)) {
      return 1;
    }
  }

  return 0;
}

int
may_set_cookies(char *d1, char *d2)
{
  return may_get_cookies(d2, d1);
}

int
parse_field(struct cookie *c, const char *keyval_str, size_t n)
{
  char *tok;

  if (tok = strcasestr(keyval_str, "Domain=")) {
    c->domain = strdup(tok + strlen("Domain="));
  } else if (tok = strcasestr(keyval_str, "Path=")) {
    c->path = strdup(tok + strlen("Path="));
  } else if (tok = strcasestr(keyval_str, "HttpOnly")) {
    c->httpOnly = 1;
  } else {
    c->attrs = strdup(keyval_str);
  }

  return 0;
}

char*
discard_setcookie(char *s)
{
  if (strstr(s, "Set-Cookie: "))
    s += strlen("Set-Cookie: ");
  return s;
}

char*
skip_spaces(char *s)
{
  return s + strspn(s, " \n\r\t");
}

struct cookie *
parse_cookie(const char *cookie_str, size_t n)
{
  struct cookie *c;
  char *str, *tok;

  c = malloc(sizeof(*c));
  str = strndup(cookie_str, n);
  str = discard_setcookie(str);

  c->httpOnly = 0;

  while (tok = strsep(&str, ";")) {
    parse_field(c, skip_spaces(tok), n);
  } 
  return c;
}


char *
serialize_cookie(struct cookie *c)
{  
  char *s;

  if (c->httpOnly) {
    asprintf(&s, "%s; Domain=%s; Path=%s; HttpOnly",
             c->attrs, c->domain, c->path);
  } else {
    asprintf(&s, "%s; Domain=%s; Path=%s",
             c->attrs, c->domain, c->path);
  }

  return s;
}

void
print_cookie(struct cookie *c)
{
  if (!c) return;

  printf("Set-Cookie: %s; Domain=%s; Path=%s",
         c->attrs, c->domain, c->path);

  if (c->httpOnly)
    printf("; HttpOnly");

  printf ("\n");
}
