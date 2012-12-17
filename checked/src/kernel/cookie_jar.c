#include "string.h"
#include "cookie_jar.h"

struct cookie_jar {
  struct cookie * START VALIDPTR ROOM_FOR(struct cookie) REF(DOMAIN([V]) = THE_STRING([DEREF([V])])) c;
  struct cookie_jar * NNSTART NNVALIDPTR NNSIZE_GE(8) next_jar;
};

struct cookie_jar * NNSTART NNVALIDPTR NNROOM_FOR(struct cookie_jar) jar CHECK_TYPE;

extern char CSOLVE_DOMAIN_STR NULLTERMSTR * START VALIDPTR ARRAY  REF(DOMAIN([V]) = DOMAIN([__s]))
  REF(THE_STRING([V]) = THE_STRING([__s]))
domain_strdup (CSOLVE_DOMAIN_STR NULLTERMSTR FINAL char * STRINGPTR __s)
  OKEXTERN;

extern
struct cookie * START ROOM_FOR(struct cookie) VALIDPTR REF(DOMAIN([V]) = THE_STRING([DEREF([V])]))
domainify_cookie(struct cookie FINAL *c) OKEXTERN;

extern void
validcookie(struct cookie FINAL *) OKEXTERN;

struct cookie *
mk_cookie(char FINAL parse_string REF(DOMAIN([V]) = THE_STRING([V])) domain,
          char FINAL parse_string attrs,
          char FINAL parse_string path,
          int httpOnly)
{
  struct cookie *ret = malloc(sizeof(*ret));

  ret->domain = domain_strdup(domain);
  ret->attrs  = domain_strdup(attrs);
  ret->path   = domain_strdup(path);
  ret->httpOnly = httpOnly;

  return domainify_cookie(ret);
}

void
add_cookie(int soc, struct cookie *cookie) CHECK_TYPE
{
  struct cookie_jar *p;
  struct cookie_jar *new;
  struct cookie     *new_cookie;

  new_cookie = mk_cookie(cookie->domain,
                         cookie->attrs,
                         cookie->path,
                         cookie->httpOnly);

  /* Set up the new node */
  new = malloc(sizeof(*new));
  new->c = new_cookie;
  new->next_jar = jar;

  /* Update cookie jar */
  jar = new;
}

struct cookie_list *
get_cookies(char *domain, char *path) CHECK_TYPE
{
  int n = 0;
  struct cookie_list *l = NULL, *cur = NULL, *new = NULL;
  struct cookie_jar  *p;
  struct cookie *c;
  struct cookie *new_cookie;

  char *cookie_domain;
  char *cookie_path;

  if (!jar)
    return NULL;

  p = jar;
  while (p) {
    if (!(c = p->c))
      continue;

    if (may_get_cookies(domain, c->domain) && !strcmp(path, c->path)) {
      new_cookie = mk_cookie(c->domain, c->path, c->attrs, c->httpOnly);
      if (!l) {
        l = mk_list_node(domain, new_cookie);
        cur = l;
      } else {
        cur->next = mk_list_node(domain, new_cookie);
        cur = cur->next;;
      }
    }
    p = p->next_jar;
  }
  return l;
}
