#ifndef COOKIE_HASH
# define COOKIE_HASH
#include <csolve.h>
#include "cookie_util.h"

//What's a suitable table size?
//Hashing on domain+path
#define TABLE_SIZE 53 

struct _cookie_list {
  struct _cookie_list * M NullOrSafe cl_next;
  SoupCookie FINAL    * LOC(C) MemSafe SoupCookieDomInv cl_cookie;
};
typedef struct _cookie_list cookie_list;

struct _cookie_list_f {
  struct _cookie_list_f * M NullOrSafe cl_next;
  SoupCookie FINAL      * FINAL LOC(C) MemSafe SoupCookieDomInv cl_cookie;
};
typedef struct _cookie_list_f cookie_list_f;

#define CookieTableInvariant NNREF(PMUT) NNOK NNSIZE_GE(4*TABLE_SIZE)
#define SafeCookieTable M OK SIZE_GE(4*TABLE_SIZE)

/* The cookie hash table */
cookie_list * M NullOrSafe * ARRAY CookieTableInvariant table;

cookie_list * M NullOrSafe * ARRAY SafeCookieTable
init_table() OKEXTERN;

void
add_cookie(cookie_list * M NullOrSafe * ARRAY SafeCookieTable table,
           SoupCookie FINAL * MemSafe SoupCookieDomInv c)
  OKEXTERN;

cookie_list_f * FINAL M NullOrSafe
NNREF(?COOKIE_DOMAIN_GET([DOMAIN([domain_str]);DOMAIN([DEREF([V+4])])]))
NNREF(DOMAIN([DEREF([V+4])]) = DOMAIN([domain_str]))
get_cookies(
  cookie_list_f * M NullOrSafe * /* M NullOrSafe NNSIZE_GE(TABLE_SIZE*4)  */ARRAY SafeCookieTable table ,
  char NULLTERMSTR FINAL * I STRINGPTR REF(DOMAIN([V]) = THE_STRING([V])) domain_str,
  char NULLTERMSTR FINAL * STRINGPTR path
  )
OKEXTERN;

char NImmutable NNREF(? COOKIE_DOMAIN_GET([DOMAIN([domain_str]);DOMAIN([V])]))
serialize_cookie_list(char FINAL NULLTERMSTR * STRINGPTR I /* REF(DOMAIN([V]) = THE_STRING([V])) */ domain_str,
                      cookie_list_f FINAL * MemSafe
                      REF(? COOKIE_DOMAIN_GET([DOMAIN([domain_str]);DOMAIN([DEREF([V+4])])]))
                      l)
OKEXTERN;
#endif
