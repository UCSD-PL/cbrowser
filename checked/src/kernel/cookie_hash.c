#include <string.h>
#include "tags.h"
#include "cookie_hash.h"
#include "util.h"

#include "debug.h"

/* struct cookie_list *  */
/* START VALIDPTR ROOM_FOR(struct cookie_list) */
/* REF(? COOKIE_DOMAIN_GET([DOMAIN([domain]); DOMAIN([V])])) */
/* mk_list_node(char FINAL parse_string domain, struct cookie FINAL *c) OKEXTERN; */

#ifndef CIL
#define domainify_cookie(c,d) c
#else
extern
struct cookie /* INST(CD,CD) */ * START ROOM_FOR(struct cookie) VALIDPTR REF(DOMAIN([V]) = THE_STRING([domain])) REF(THE_STRING([domain]) = THE_STRING([DEREF([V])])) 
domainify_cookie(struct cookie FINAL INST(CD,CD) * CookiePtr REF(THE_STRING([DEREF([V])]) = THE_STRING([domain])) c,
                 char NULLTERMSTR ICHAR FINAL * LOC(CD) STRINGPTR domain) OKEXTERN;
#endif

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
  return hash_fn(c->domain, c->cookie->path);
}

void
check_soup_cookie_dom(SoupCookie FINAL *sc,
                      char NULLTERMSTR ICHAR FINAL *
                      REF(DOMAIN([sc]) = THE_STRING([V])) s) OKEXTERN;

struct cookie FINAL * REF(DOMAIN([V]) = THE_STRING([DEREF([V])])) CookiePtr
  copy_cookie(struct cookie INST(CD,CD) FINAL * REF(DOMAIN([V]) = THE_STRING([DEREF([V])])) CookiePtr c) //CHECK_TYPE
{
  SoupCookie *new_s;
  struct cookie *new;

  char *domstr = c->domain;
  domstr   = immutable_strdup(mutable_strdup(domstr));
  new_s = soup_cookie_copy(c->cookie);

  new         = malloc(sizeof(*new));
  new->domain = domstr;
  new->cookie = new_s;

  return domainify_cookie(new, domstr);
}

void
insert_new_cookie(struct cookie_list ** ARRAY table, int i, struct cookie *c)
{
  struct cookie *new_cookie;
  struct cookie_list *new_l;

  new_cookie = copy_cookie(c);

  new_l = malloc(sizeof(*new_l));
  new_l->next = NULL;
  new_l->cookie = new_cookie;

  table[i] = new_l;
}

void
add_cookie(struct cookie *c) CHECK_TYPE
{
  int i;
  struct cookie_list *l, *new_l;
  struct cookie *new_cookie;

  i = hash_cookie(c);

  if (!table) {
    table = malloc(53*sizeof(*table));
  }

  l = table[i];

  if (l == NULL)
  {
    insert_new_cookie(table, i, c);
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

struct cookie *
copy_if_match(struct cookie FINAL *c, char FINAL *domain)
{
  struct cookie *new = NULL;

  if (!strcmp(mutable_strdup(c->domain), mutable_strdup(domain))) {
    new = copy_cookie(c);
  }

  return new;
}

#ifndef CIL
#define domainify_cookie_list(l) l
#else
struct cookie_list INST(C,C) * LOC(L)
START VALIDPTR ROOM_FOR(struct cookie_list)
REF(DOMAIN([V]) = DOMAIN([DEREF([V + 4])]))
domainify_cookie_list(struct cookie_list FINAL INST(C,C) * LOC(L) l) OKEXTERN;
#endif

struct cookie_list INST(C,C) *
make_cookie_node(struct cookie FINAL * LOC(C) c)
{
  struct cookie_list *l;
  l = malloc(sizeof(*l));

  l->cookie = c;
  l->next   = NULL;

  return domainify_cookie_list(l);
}

struct cookie_list *
get_cookies(char *domain_str, char *path) CHECK_TYPE
{
  struct cookie_list *l, *head = NULL, *curr = NULL, *new = NULL;
  struct cookie *c, *new_cookie;
  char  *cookie_domain, *new_domain;
  int i;

  i = hash_fn(domain_str, path);

  if (!table) return NULL;

  l = table[i];

  while (l)
  {
    c = l->cookie;
    new_cookie = copy_if_match(c, domain_str);
    if (new_cookie &&
        !strcmp(path, soup_cookie_get_path(new_cookie->cookie)))
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

  return NULL;
}

/* char * */
/* serialize_cookie_list(struct cookie_list *l) CHECK_TYPE */
/* { */
/*   char *cookie_string = NULL; */
/*   char *response = NULL; */

/*   while (l) { */
/*     cookie_string = soup_cookie_to_cookie_header(l->cookie->cookie); */
/*     if (response) { */
/*       response = immutable_strdup(strapp("%s; %s", response, cookie_string)); */
/*     } else { */
/*       response = immutable_strdup(cookie_string); */
/*     } */
/*     l = l->next; */
/*   } */

/*   return response; */
/* } */
