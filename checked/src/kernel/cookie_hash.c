#include "cookie_hash.h"
#include "util.h"
#include "libsoup/soup-cookie.h"
#include "msg.h"
//#include "debug.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

int
hash_fn(char FINAL *d, char FINAL *p)
{
  int hash = 0;

  while (*d != 0)
  {
    hash += *d;
    d++;
  }
  /* while (*p != 0) */
  /* { */
  /*   hash += *p; */
  /*   p++; */
  /* } */

  return hash % TABLE_SIZE;
}

int
hash_cookie(SoupCookie FINAL *c)
{
  //return hash_fn(c->domain, c->path);
  return hash_fn(c->domain, "/");
}

cookie_list_d INST(C,C) *
new_cookie_node(SoupCookie * LOC(C) c)
{
  SoupCookie  *new_cookie;
  cookie_list_d *new_node;

  new_node            = malloc(sizeof(*new_node));
  new_node->cl_cookie = c;
  new_node->cl_next   = NULL;

  return new_node;
}


cookie_list * M NullOrSafe * NNREF(PMUT) NullOrSafe NNSIZE_GE(TABLE_SIZE*4) ARRAY
init_table()
{
  int t;
  cookie_list **table;
  table = malloc(53*sizeof(*table));
  for (t = 0; t < TABLE_SIZE; ++t) {
    table[t] = NULL;
  }

  return table;
}

void
add_cookie(cookie_list ** ARRAY table, SoupCookie *c) CHECK_TYPE
{
  int i,t;
  SoupCookie *new_cookie;
  cookie_list *l, *new_l, *tmp;

  i = hash_cookie(c);

  l = table[i];

  if (l == NULL)
  {
    table[i] = new_cookie_node(soup_cookie_copy(c));
  }
  else
  {
    tmp = NULL;
    while (l) {
      if (!strcmp(l->cl_cookie->path, c->path)) {
        l->cl_cookie = soup_cookie_copy(c);
        return;
      } else {
        tmp = l;
        l = l->cl_next;
      }
    }
    if (tmp) {
      tmp->cl_next = new_cookie_node(soup_cookie_copy(c));
    }
  }
  fprintf(stderr, "SET-COOKIE: %s %s\n", c->domain, c->path);
}

SoupCookie * NNREF(DOMAIN([V]) = DOMAIN([domain])) NNREF(SoupCookieInvariant) MemSafe
copy_if_match(SoupCookie FINAL* c, char FINAL * domain)
{
  SoupCookie *new = NULL;
  char *dom = c->domain;
  int test;

  if (!(test = strcmp(dom, domain))) {
    new = soup_cookie_copy(c);
    return new;
  }

  return new;
}

cookie_list_d *
get_cookies(cookie_list ** ARRAY table, char *domain_str, char *path) CHECK_TYPE
{
  char   *cookie_domain, *new_domain;
  SoupCookie *c, *new_cookie;
  cookie_list_d *l, *head = NULL, *curr = NULL, *new = NULL;
  int i;
  int qed;

  assert (domain_str);
  assert (path);
  /* i = hash_fn(domain_str, path); */
  i = hash_fn(domain_str, "/");

  if (!table) return NULL;

  l = table[i];
  while (l)
  {
    c = l->cl_cookie;
    new_cookie = copy_if_match(c, domain_str);
    if (new_cookie /* && !strcmp(path, soup_cookie_get_path(new_cookie)) */)
    {
      new = new_cookie_node(new_cookie);

      if (!head)
      {
        head = new;
        curr = new;
      }
      else
      {
        curr->cl_next = new;
        curr          = curr->cl_next;
      }
    }
    l = l->cl_next;
  }

  return head;
}

char *
serialize_cookie_list(char *domain_str, cookie_list_d *l) CHECK_TYPE
{
  char *cookie_string = NULL;
  char *response      = NULL;

  while (l) {
    cookie_string = soup_cookie_to_set_cookie_header(l->cl_cookie);

    if (cookie_string) {
      if (response == NULL) {
        response = format_cookie_header(cookie_string);
      } else {
        response = join_cookie_headers(cookie_string, response);
      }
    }
    l = l->cl_next;
  }

  return response;
}
