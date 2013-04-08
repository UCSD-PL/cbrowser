#include <string.h>
#include "cookie_hash.h"
#include "util.h"
#include "libsoup/soup-cookie.h"
#include <assert.h>

#include "debug.h"

/* int */
/* hash_fn(char FINAL *d, char FINAL *p) */
/* { */
/*   int hash = 0; */

/*   while (*d != 0) */
/*   { */
/*     hash += *d; */
/*     d++; */
/*   } */
/*   while (*p != 0) */
/*   { */
/*     hash += *p; */
/*     p++; */
/*   } */

/*   return hash % TABLE_SIZE; */
/* } */

/* int */
/* hash_cookie(SoupCookie FINAL *c) */
/* { */
/*   return hash_fn(c->domain, c->path); */
/* } */

/* void */
/* check_soup_cookie_dom(SoupCookie FINAL *sc, */
/*                       char NULLTERMSTR FINAL * I */
/*                       REF(DOMAIN([sc]) = THE_STRING([V])) s) OKEXTERN; */


/* cookie_list_f INST(C,C) * */
/* new_cookie_node(SoupCookie * LOC(C) c) */
/* { */
/*   SoupCookie  *new_cookie; */
/*   cookie_list_f *new_node; */

/*   csolve_assert(c != NULL); */
/*   new_node            = malloc(sizeof(*new_node)); */
/*   new_node->cl_cookie = c; */
/*   new_node->cl_next   = NULL; */

/*   return new_node; */
/* } */


/* cookie_list * M NullOrSafe * NNREF(PMUT) NullOrSafe NNSIZE_GE(TABLE_SIZE*4) ARRAY  */
/* init_table() */
/* { */
/*   int t; */
/*   cookie_list **table; */
/*   table = malloc(53*sizeof(*table)); */
/*   for (t = 0; t < TABLE_SIZE; ++t) { */
/*     table[t] = NULL; */
/*   } */

/*   return table; */
/* } */

/* void */
/* add_cookie(cookie_list ** ARRAY table, SoupCookie *c) CHECK_TYPE */
/* { */
/*   int i,t; */
/*   SoupCookie *new_cookie; */
/*   cookie_list *l, *new_l; */

/*   i = hash_cookie(c); */

/*   if (!table) { */
/*     table = malloc(53*sizeof(*table)); */
/*     for (t = 0; t < TABLE_SIZE; ++t) { */
/*       table[t] = NULL; */
/*     } */
/*   } */

/*   l = table[i]; */

/*   if (l == NULL) */
/*   { */
/*     table[i] = new_cookie_node(soup_cookie_copy(c)); */
/*   } */
/*   else */
/*   { */
/*     while (l->cl_next) */
/*     { */
/*       l = l->cl_next; */
/*     } */
/*     l->cl_next = new_cookie_node(soup_cookie_copy(c)); */
/*   } */
/* } */

/* SoupCookie * NNREF(DOMAIN([V]) = DOMAIN([domain])) NNREF(SoupCookieInvariant) MemSafe */
/* copy_if_match(SoupCookie FINAL* c, char FINAL * domain) */
/* { */
/*   SoupCookie *new = NULL; */
/*   char *dom = c->domain; */
/*   int test; */

/*   if (!(test = strcmp(dom, domain))) { */
/*     new = soup_cookie_copy(c); */
/*     return new; */
/*   } */

/*   return new; */
/* } */

/* cookie_list_f * */
/* get_cookies(cookie_list ** ARRAY table, char *domain_str, char *path) CHECK_TYPE */
/* { */
/*   char   *cookie_domain, *new_domain; */
/*   SoupCookie *c, *new_cookie; */
/*   cookie_list_f *l, *head = NULL, *curr = NULL, *new = NULL; */
/*   int i; */
/*   int qed; */

/*   assert (domain_str); */
/*   assert (path); */
/*   i = hash_fn(domain_str, path); */

/*   if (!table) return NULL; */

/*   l = table[i]; */

/*   while (l) */
/*   { */
/*     c = l->cl_cookie; */
/*     new_cookie = copy_if_match(c, domain_str); */
/*     if (new_cookie && !strcmp(path, soup_cookie_get_path(new_cookie))) */
/*     { */
/*       new = new_cookie_node(new_cookie); */

/*       if (!head) */
/*       { */
/*         head = new; */
/*         curr = new; */
/*       } */
/*       else */
/*       { */
/*         curr->cl_next = new; */
/*         curr          = curr->cl_next; */
/*       } */
/*     } */
/*     l = l->cl_next; */
/*   } */

/*   return head; */
/* } */

char *
serialize_cookie_list(char *domain_str, cookie_list *l) CHECK_TYPE
{
  char *cookie_string = NULL;
  char *response      = NULL;

  while (l) {
    // Sanity check
    if (!soup_cookie_domain_matches(l->cl_cookie, domain_str))
      continue;

    cookie_string = soup_cookie_to_cookie_header(l->cl_cookie);
    //    cookie_string = immutable_strdup(cookie_string);

    if (response) {
      response = strapp(freeze_ptr("%s; %s"), response, cookie_string);
    } else {
      response = cookie_string;
    }
    l = l->cl_next;

  }

  return response;
}
