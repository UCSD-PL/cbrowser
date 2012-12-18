#include <string.h>
#include "cookie_hash.h"

//What's a suitable table size?
//Hashing on domain+path
//Thus I declare 53.
#define TABLE_SIZE 53 

struct cookie_list *table[TABLE_SIZE];

struct cookie_list * 
START VALIDPTR ROOM_FOR(struct cookie_list)
REF(? COOKIE_DOMAIN_GET([DOMAIN([domain]); DOMAIN([V])]))
mk_list_node(char FINAL parse_string domain, struct cookie FINAL *c) OKEXTERN;

extern char CSOLVE_DOMAIN_STR NULLTERMSTR * START VALIDPTR ARRAY  REF(DOMAIN([V]) = DOMAIN([__s]))
  REF(THE_STRING([V]) = THE_STRING([__s]))
domain_strdup (CSOLVE_DOMAIN_STR NULLTERMSTR FINAL char * STRINGPTR __s)
  OKEXTERN;

extern
struct cookie * START ROOM_FOR(struct cookie) VALIDPTR REF(DOMAIN([V]) = THE_STRING([DEREF([V])]))
domainify_cookie(struct cookie FINAL *c) OKEXTERN;

extern
char NULLTERMSTR CSOLVE_DOMAIN_STR *
domainify_string(char NULLTERMSTR *) OKEXTERN;

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

struct cookie *
copy_cookie(struct cookie FINAL *c)
{
  struct cookie *new;

  new = mk_cookie(c->domain,
                  c->attrs,
                  c->path,
                  c->httpOnly);

  return new;
}

void
insert_cookie(struct cookie *c) CHECK_TYPE
{
  int i;
  struct cookie_list *l;
  struct cookie *new_cookie;

  /* Random for now */
  i = nondetpos();
  CSOLVE_ASSUME(i >= 0 && i < TABLE_SIZE);

  new_cookie = copy_cookie(c);
  l = table[i];

  if (l == NULL)
  {
    l = mk_list_node(c->domain, new_cookie);
    table[i] = l;
  }
  else
  {
    while (l->next)
    {
      l = l->next;
    }
    l->next = mk_list_node(c->domain, new_cookie);
  }
}

struct cookie_list *
get_cookies(char *domain, char *path) CHECK_TYPE
{
  int i;
  struct cookie_list *l, *head, *curr, *new;
  struct cookie *c;

  /* Random for now */
  i = nondetpos();
  CSOLVE_ASSUME(i >= 0 && i < TABLE_SIZE);

  l = table[i];

  if (l == NULL)
  {
    return NULL;
  }

  head = malloc(sizeof(*head));
  head->next = NULL;
  head->cookie = copy_cookie(l->cookie);
  curr = head;
  l = l->next;

  while (l)
  {
    c = l->cookie;
    if (!strcmp(domain, c->domain) && !strcmp(path, c->path))
    {
      new = malloc(sizeof(*new));
      new->next = NULL;
      new->cookie = copy_cookie(l->cookie);

      curr->next = new;
      curr = curr->next;
    }
    l = l->next;
  }

  return head;
}
