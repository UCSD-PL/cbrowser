#include <string.h>
#include "tags.h"
#include "cookie_hash.h"
#include "util.h"

#include "debug.h"

struct cookie_list * 
START VALIDPTR ROOM_FOR(struct cookie_list)
REF(? COOKIE_DOMAIN_GET([DOMAIN([domain]); DOMAIN([V])]))
mk_list_node(char FINAL parse_string domain, struct cookie FINAL *c) OKEXTERN;

extern
struct cookie /* INST(CD,CD) */ * START ROOM_FOR(struct cookie) VALIDPTR REF(DOMAIN([V]) = THE_STRING([domain])) REF(THE_STRING([domain]) = THE_STRING([DEREF([V])])) 
domainify_cookie(struct cookie FINAL INST(CD,CD) * CookiePtr REF(THE_STRING([DEREF([V])]) = THE_STRING([domain])) c,
                 char NULLTERMSTR DOMAIN_STR FINAL * LOC(CD) STRINGPTR domain) OKEXTERN;

extern
char NULLTERMSTR DOMAIN_STR *
domainify_string(char NULLTERMSTR *) OKEXTERN;

int
hash_fn(char FINAL *d, char FINAL *p)
{
  int hash = 0;

  while (*d != 0)
  {
    hash += *d;
    d++;
  }
  while (*p != 0)
  {
    hash += *p;
    p++;
  }

  return hash % TABLE_SIZE;
}

int
hash_cookie(struct cookie FINAL *c)
{
  return hash_fn(c->domain, domainify_string(c->cookie->path));
}

/* struct cookie * */
/* mk_cookie(char FINAL parse_string REF(DOMAIN([V]) = THE_STRING([V])) domain, */
/*           char FINAL parse_string attrs, */
/*           char FINAL parse_string path, */
/*           int httpOnly) */
/* { */
/*   struct cookie *ret = malloc(sizeof(*ret)); */

/*   ret->domain = domain_strdup(domain); */
/*   ret->attrs  = domain_strdup(attrs); */
/*   ret->path   = domain_strdup(path); */
/*   ret->httpOnly = httpOnly; */

/*   return domainify_cookie(ret); */
/* } */
/* struct cookie * */
/* copy_cookie2(struct cookie FINAL *c, char *dom) */
/* { */
/*   SoupCookie *new_s; */
/*   struct cookie *new; */

/*   dom = strdup(dom); */
/*   new_s = soup_cookie_copy(c->cookie); */

/*   new = malloc(sizeof(*new)); */

/*   new->cookie = new_s; */
/*   new->domain = dom; */
/*   return domainify_cookie(new, new->domain); */
/*   /\* new->domain = strdup(c->domain); *\/ */
/*   /\* new->cookie = soup_cookie_copy(c->cookie); *\/ */
/*   /\* new = mk_cookie(c->domain, *\/ */
/*   /\*                 c->attrs, *\/ */
/*   /\*                 c->path, *\/ */
/*   /\*                 c->httpOnly); *\/ */

/*   /\* return new; *\/ */
/* } */

void
check_soup_cookie_dom(SoupCookie FINAL *sc,
                      char NULLTERMSTR DOMAIN_STR FINAL *
                      REF(DOMAIN([sc]) = THE_STRING([V])) s) OKEXTERN;

struct cookie FINAL * REF(DOMAIN([V]) = THE_STRING([DEREF([V])])) CookiePtr
copy_cookie(struct cookie INST(CD,CD) FINAL * REF(DOMAIN([V]) = THE_STRING([DEREF([V])])) CookiePtr c) CHECK_TYPE
{
  SoupCookie *new_s;
  struct cookie *new;

  char *domstr = c->domain;
  domstr   = strdup(domstr);
  new_s = soup_cookie_copy(c->cookie);

  new         = malloc(sizeof(*new));
  new->domain = domstr;
  new->cookie = new_s;

  return domainify_cookie(new, domstr);
  /* return domainify_cookie(new, dom); */
  /* new->domain = strdup(c->domain); */
  /* new->cookie = soup_cookie_copy(c->cookie); */
  /* new = mk_cookie(c->domain, */
  /*                 c->attrs, */
  /*                 c->path, */
  /*                 c->httpOnly); */

  /* return new; */
}

void
add_cookie(int soc, struct cookie *c) CHECK_TYPE
{
  int i;
  struct cookie_list *l, *new_l;
  struct cookie *new_cookie;

  i = hash_cookie(c);
  l = table[i];

  if (l == NULL)
  {
    new_cookie = copy_cookie(c);
    new_l = malloc(sizeof(*l));
    new_l->next = NULL;
    new_l->cookie = new_cookie;
    table[i] = new_l;
  }
  else
  {
    while (l->next)
    {
      l = l->next;
    }
    new_cookie = copy_cookie(c);
    new_l = malloc(sizeof(*l));
    new_l->next = NULL;
    new_l->cookie = new_cookie;
    l->next = new_l;
  }
}

void
expected_node(
struct cookie_list FINAL * 
NNSTART NNVALIDPTR NNROOM_FOR(struct cookie_list)
NNREF(THE_STRING([DEREF([V])]) = THE_STRING([domain]))
NNREF(DOMAIN([DEREF([V + 4])]) = THE_STRING([domain])) n,
char FINAL NULLTERMSTR DOMAIN_STR *STRINGPTR domain) OKEXTERN;

void
expected_cookie(struct cookie FINAL * CookiePtr REF(DOMAIN([V]) = THE_STRING([DEREF([V])])) c) OKEXTERN;

struct cookie *
copy_if_match(struct cookie FINAL *c, char FINAL *domain)
{
  struct cookie *new = NULL;

  if (!strcmp(c->domain, domain)) {
    new = copy_cookie(c);
  }

  return new;
}

struct cookie_list FINAL *
NNSTART NNVALIDPTR NNROOM_FOR(struct cookie_list)
NNREF(DOMAIN([DEREF([V + 4])]) = THE_STRING([domain]))
NNREF(DEREF([V]) = 0)
make_cookie_node_cond(struct cookie FINAL * CookiePtr REF(DOMAIN([V]) = THE_STRING([DEREF([V])])),
                      char FINAL NULLTERMSTR DOMAIN_STR *STRINGPTR domain) OKEXTERN;

struct cookie_list INST(C,C) *
make_cookie_node(struct cookie FINAL * LOC(C) c)
{
  struct cookie_list *l;
  l = malloc(sizeof(*l));

  l->cookie = c;
  l->next   = NULL;

  return l;
}
                 

struct cookie_list *
get_cookies(char *domain_str, char *path) CHECK_TYPE
{
  struct cookie_list *l, *head = NULL, *curr = NULL, *new = NULL;
  struct cookie *c, *new_cookie;
  char  *cookie_domain, *new_domain;

  l = table[hash_fn(domain_str, path)];

  while (l)
  {
    c = l->cookie;
    new_cookie = copy_if_match(c, domain_str);
    if (new_cookie &&
        !strcmp(path, domainify_string(soup_cookie_get_path(new_cookie->cookie))))
    {
      new = make_cookie_node(new_cookie);

      if (!head) {
      head = new;
      curr = new;
      } else {
        curr->next = new;
        curr = curr->next;
      }
    }
    l = l->next;
  }

  return head;
}

#if 0
char *
serialize_cookie_list(struct cookie_list *l) CHECK_TYPE
{
  char *cookie_string = NULL;
  char *response = NULL;

  while (l) {
    cookie_string = soup_cookie_to_cookie_header(l->cookie->cookie);
    if (response) {
      response = strapp("%s; %s", response, cookie_string);
    } else {
      response = cookie_string;
    }
    l = l->next;
  }

  return response;
}
#endif
