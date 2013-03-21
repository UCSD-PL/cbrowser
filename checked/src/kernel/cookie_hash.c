#include <string.h>
#include "cookie_hash.h"
#include "util.h"
#include "libsoup/soup-cookie.h"
#include <assert.h>

#include "debug.h"

/* struct cookie_list *  */
/* START VALIDPTR ROOM_FOR(struct cookie_list) */
/* REF(? COOKIE_DOMAIN_GET([DOMAIN([domain]); DOMAIN([V])])) */
/* mk_list_node(char FINAL parse_string domain, struct cookie FINAL *c) OKEXTERN; */

#ifndef CIL
#define domainify_cookie(c,d) c
#else
extern
cookie /* INST(CD,CD) */ * START ROOM_FOR(cookie) VALIDPTR REF(DOMAIN([V]) = THE_STRING([domain])) REF(THE_STRING([domain]) = THE_STRING([DEREF([V])])) 
domainify_cookie(cookie FINAL INST(CD,CD) * MemSafe REF(THE_STRING([DEREF([V])]) = THE_STRING([domain])) c,
                 char NULLTERMSTR FINAL * IMMUTABLE LOC(CD) STRINGPTR domain) OKEXTERN;
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
hash_cookie(cookie FINAL *c)
{
  return hash_fn(c->c_domain, c->c_cookie->path);
}

void
check_soup_cookie_dom(SoupCookie FINAL *sc,
                      char NULLTERMSTR FINAL * IMMUTABLE
                      REF(DOMAIN([sc]) = THE_STRING([V])) s) OKEXTERN;

cookie FINAL * REF(DOMAIN([V]) = THE_STRING([DEREF([V])])) MemSafe
copy_cookie(cookie INST(CD,CD) FINAL * REF(DOMAIN([V]) = THE_STRING([DEREF([V])])) MemSafe c) //CHECK_TYPE
{
  SoupCookie *new_s;
  cookie *new;

  char *domstr = c->c_domain;
  domstr   = strdupi(domstr);//immutable_strdup(mutable_strdup(domstr));
  new_s = soup_cookie_copy(c->c_cookie);

  new         = malloc(sizeof(*new));
  new->c_domain = domstr;
  new->c_cookie = new_s;

  return domainify_cookie(new, domstr);
}

void
insert_new_cookie(cookie_list ** ARRAY table, int i, cookie *c)
{
  cookie *new_cookie;
  cookie_list *new_l;

  new_cookie = copy_cookie(c);

  new_l = malloc(sizeof(*new_l));
  new_l->cl_next = NULL;
  new_l->cl_cookie = new_cookie;

  table[i] = new_l;
}

void
add_cookie(cookie *c) CHECK_TYPE
{
  int i,t;
  cookie *new_cookie;
  cookie_list *l, *new_l;

  i = hash_cookie(c);

  if (!table) {
    table = malloc(53*sizeof(*table));
    for (t = 0; t < TABLE_SIZE; ++t) {
      table[t] = NULL;
    }
  }

  l = table[i];

  if (l == NULL)
  {
    insert_new_cookie(table, i, c);
  }
  else
  {
    while (l->cl_next)
    {
      l = l->cl_next;
    }
    new_cookie = copy_cookie(c);
    new_l = malloc(sizeof(*l));
    new_l->cl_next = NULL;
    new_l->cl_cookie = new_cookie;
    l->cl_next = new_l;
  }
}

cookie *
copy_if_match(cookie FINAL *c, char FINAL *domain)
{
  cookie *new = NULL;

  if (!strcmp(mutable_strdup(c->c_domain), mutable_strdup(domain))) {
    new = copy_cookie(c);
  }

  return new;
}

#ifndef CIL
#define domainify_cookie_list(l) l
#else
cookie_list INST(C,C) * LOC(L)
MemSafe
REF(DOMAIN([V]) = DOMAIN([DEREF([V + 4])]))
domainify_cookie_list(cookie_list FINAL INST(C,C) * LOC(L) l) OKEXTERN;
#endif

cookie_list INST(C,C) *
make_cookie_node(cookie FINAL * LOC(C) c)
{
  cookie_list *l;
  l = malloc(sizeof(*l));

  l->cl_cookie = c;
  l->cl_next   = NULL;

  return domainify_cookie_list(l);
}

cookie_list *
get_cookies(char *domain_str, char *path) CHECK_TYPE
{
  char   *cookie_domain, *new_domain;
  cookie *c, *new_cookie;
  cookie_list *l, *head = NULL, *curr = NULL, *new = NULL;
  int i;

  assert (domain_str);
  assert (path);
  i = hash_fn(domain_str, path);

  if (!table) return NULL;

  l = table[i];

  while (l)
  {
    c = l->cl_cookie;
    new_cookie = copy_if_match(c, domain_str);
    if (new_cookie &&
        !strcmp(path, soup_cookie_get_path(new_cookie->c_cookie)))
    {
      new = make_cookie_node(new_cookie);

      if (!head) {
        head = new;
        curr = new;
      } else {
        curr->cl_next = new;
        curr          = curr->cl_next;
      }
    }
    l = l->cl_next;
  }

  return NULL;
}

char *
serialize_cookie_list(char *domain_str, cookie_list *l) CHECK_TYPE
{
  char *cookie_string = NULL;
  char *response      = NULL;

  while (l) {
    // Sanity check
    if (!soup_cookie_domain_matches(l->cl_cookie->c_cookie, domain_str))
      continue;

    cookie_string = soup_cookie_to_cookie_header(l->cl_cookie->c_cookie);
    cookie_string = immutable_strdup(cookie_string);

    if (response) {
      response = strapp(freeze_ptr("%s; %s"), response, cookie_string);
    } else {
      response = cookie_string;
    }
    l = l->cl_next;

  }

  return response;
}
