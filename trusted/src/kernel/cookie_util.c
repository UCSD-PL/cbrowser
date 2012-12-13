#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>

#include "cookie_util.h"

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
