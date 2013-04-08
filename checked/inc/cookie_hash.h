#ifndef COOKIE_HASH
# define COOKIE_HASH
#include <csolve.h>
#include "cookie_util.h"

//What's a suitable table size?
//Hashing on domain+path
#define TABLE_SIZE 53 

struct _cookie_list {
  struct _cookie_list * M NullOrSafe cl_next;
  SoupCookie FINAL    * MemSafe LOC(C) REF(SoupCookieInvariant) REF(DOMAIN([V]) = THE_STRING([DEREF([V+8])])) cl_cookie;
};
typedef struct _cookie_list cookie_list;

struct _cookie_list_f {
  struct _cookie_list_f * M NullOrSafe cl_next;
  SoupCookie FINAL      * MemSafe LOC(C) REF(SoupCookieInvariant) REF(DOMAIN([V]) = THE_STRING([DEREF([V+8])])) FINAL cl_cookie;
};
typedef struct _cookie_list_f cookie_list_f;

int
  REF(DOMAIN([cl]) = DOMAIN([domain]))
  cookie_list_domain(cookie_list_f FINAL * cl,
                     char         NULLTERMSTR FINAL * STRINGPTR /* REF(DOMAIN([V]) = DOMAIN([DEREF([cl+4])])) */ domain)
  OKEXTERN;


/* The cookie hash table */
cookie_list * M NullOrSafe * NNREF(PMUT) NullOrSafe NNSIZE_GE(TABLE_SIZE*4) ARRAY table CHECK_TYPE;

cookie_list * LOC(L)
append_lists(cookie_list * LOC(L) MemSafe l1, cookie_list * LOC(L) MemSafe l2) OKEXTERN;

void
add_cookie(
  cookie_list * M NullOrSafe * NNREF(PMUT) NullOrSafe NNSIZE_GE(TABLE_SIZE*4) ARRAY table ,
  SoupCookie FINAL * MemSafe
           REF(&&[StructDom(0);StructDom(4);StructDom(8);StructDom(12)])
           REF(DOMAIN([V]) = THE_STRING([DEREF([V+8])])) c) OKEXTERN;

cookie_list_f * M NullOrSafe FINAL
/* NNREF(DOMAIN([DEREF([V + 4])]) = THE_STRING([domain_str])) */
/* NNREF(? COOKIE_DOMAIN_GET([DOMAIN([domain_str]);DOMAIN([V])])) */
/* NNREF((DEREF([V]) != 0) => DOMAIN([DEREF([V])]) = DOMAIN([domain_str])) */
/* NNREF(DEREF([V]) > 0 => ? COOKIE_DOMAIN_GET([DOMAIN([domain_str]);DOMAIN([DEREF([V])])])) */
//NNREF(DOMAIN([V]) = DOMAIN([domain_str]))
NNREF(?COOKIE_DOMAIN_GET([DOMAIN([domain_str]);DOMAIN([DEREF([V+4])])]))
NNREF(DOMAIN([DEREF([V+4])]) = DOMAIN([domain_str]))
get_cookies(
  cookie_list * M NullOrSafe * M NullOrSafe NNSIZE_GE(TABLE_SIZE*4) ARRAY table ,
  char NULLTERMSTR FINAL * I STRINGPTR REF(DOMAIN([V]) = THE_STRING([V])) domain_str,
  char NULLTERMSTR FINAL * STRINGPTR path
  )
OKEXTERN;

char NImmutable NNREF(? COOKIE_DOMAIN_GET([DOMAIN([domain_str]);DOMAIN([V])]))
serialize_cookie_list(char FINAL NULLTERMSTR * STRINGPTR I /* REF(DOMAIN([V]) = THE_STRING([V])) */ domain_str,
  cookie_list FINAL * MemSafe
                      NNREF(? COOKIE_DOMAIN_GET([DOMAIN([domain_str]);DOMAIN([DEREF([V+4])])]))
  /* NNREF(DOMAIN([DEREF([V + 4])]) = THE_STRING([domain_str])) */
  /* NNREF(DEREF([V]) > 0 => ? COOKIE_DOMAIN_GET([DOMAIN([domain_str]);DOMAIN([DEREF([V])])])) */
  l)
OKEXTERN;
#endif
            /* char NULLTERMSTR FINAL * STRINGPTR path) OKEXTERN; */
/* serialize_cookie_list(char *domain_str, */
/*   cookie_list FINAL * */
/*                       NNREF(DOMAIN([DEREF([V + 4])]) = THE_STRING([domain_str])) l) */

void
assert_gettable_list(char FINAL Immutable s,
                     cookie_list FINAL * REF(?COOKIE_DOMAIN_GET([DOMAIN([s]);DOMAIN([DEREF([V+4])])])) l) OKEXTERN;
void
assert_gettable_list_fd(int f,
                        cookie_list FINAL * REF(?COOKIE_DOMAIN_GET([DOMAIN([f]);DOMAIN([V])])) l) OKEXTERN;

int REF(?COOKIE_DOMAIN_GET([DOMAIN([a]);DOMAIN([c])]))
check_transitivity(void * REF(?COOKIE_DOMAIN_GET([DOMAIN([V]);DOMAIN([b])])) a,
                   void * REF(?COOKIE_DOMAIN_GET([DOMAIN([V]);DOMAIN([c])])) b,
                   void * c) OKEXTERN;
