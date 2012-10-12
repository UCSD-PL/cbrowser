#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>

#include "cookie.h"

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

int
parse_cookie(struct cookie *c, const char *cookie_str, size_t n)
{
  char *str, *tok;

  str = strndup(cookie_str, n);
  str = discard_setcookie(str);

  c->httpOnly = 0;

  while (tok = strsep(&str, ";")) {
    parse_field(c, skip_spaces(tok), n);
  } 
  //Assuming a cookie looks like this:
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

void
set_cookie(struct cookie_jar **cookies, struct cookie *c)
{
  char *tmpstr;
  struct cookie_jar *p;

  if (!*cookies) {
    *cookies = malloc(sizeof(**cookies));
    (*cookies)->c = c;
    (*cookies)->next_jar = NULL;
    return;
  }

  p = *cookies;
  while (p->next_jar) {
    if (!strcmp(p->c->domain, c->domain)
        && !strcmp(p->c->path, c->path)) {
      asprintf(&tmpstr, "%s; %s", p->c->attrs, c->attrs);
      free(p->c->attrs);
      p->c->attrs = tmpstr;
      return;
    }
    p = p->next_jar;
  }

  p->next_jar = malloc(sizeof(*(p->next_jar)));
  p = p->next_jar;
  p->c = malloc(sizeof(*(p->c)));
  *(p->c) = *c;
  p->next_jar = NULL;
}

struct cookie_jar *
get_cookies(struct cookie_jar *cookies,
            const char *scheme,
            const char *domain,
            const char *path)
{
  struct cookie_jar *r, *p;

  r = NULL;
  p = cookies;

  
}

void
print_cookie_jar(struct cookie_jar *cookies)
{
  printf ("Cookie jar:\n");
  while(cookies) {
    printf ("\t");
    print_cookie(cookies->c);
    cookies = cookies->next_jar;
  }
}

#if 0
int
main(int argc, char **argv)
{
  struct cookie c;
  struct cookie_jar *cj;

  if(argv[1]) {
    printf("Raw Cookie: [%s]\n", argv[1]);
    parse_cookie(&c, argv[1], strlen(argv[1]));
    print_cookie(&c);
    set_cookie(&cj, &c);
    print_cookie_jar(cj);
  }
}
#endif
