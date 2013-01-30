#ifndef COOKIE_HASH
# define COOKIE_HASH
#include <csolve.h>
#include "cookie_util.h"

#define CookiePtr START VALIDPTR ROOM_FOR(struct cookie)
#define NNCookiePtr NNSTART NNVALIDPTR NNROOM_FOR(struct cookie)

//What's a suitable table size?
//Hashing on domain+path
//Thus I declare 53.
#define TABLE_SIZE 53 

struct cookie_list {
  struct cookie_list * NNSTART NNVALIDPTR NNSIZE_GE(8) next;
  struct cookie * LOC(C) CookiePtr REF(DOMAIN([V]) = THE_STRING([DEREF([V])])) cookie;
};

struct cookie_list 
* NNSTART NNVALIDPTR NNROOM_FOR(struct cookie_list)
  * ARRAY NNSTART NNVALIDPTR NNSIZE_GE(TABLE_SIZE*4) table CHECK_TYPE;

struct cookie_list * LOC(L)
append_lists(struct cookie_list * LOC(L) l1,
             struct cookie_list * LOC(L) NNSTART NNVALIDPTR NNROOM_FOR(struct cookie_list) l2)
OKEXTERN;

void
add_cookie(//int soc,
           struct cookie FINAL * 
           CookiePtr
           /* REF(TAGSET([DEREF([V+4])])  = TAGSET([soc])) //Not Necessary? */
           /* REF(TAGSET([DEREF([V + 4])]) = Set_sng([soc])) */
           /* REF(DOMAIN([DEREF([V + 4])]) = DOMAIN([soc])) */
           REF(DOMAIN([DEREF([V + 4])]) = THE_STRING([DEREF([V])]))
           /* REF(? COOKIE_DOMAIN_GET([DOMAIN([soc]);DOMAIN([V])])) */
           /* REF(? COOKIE_DOMAIN_SET([DOMAIN([soc]);DOMAIN([V])])) */
           /* REF(? COOKIE_DOMAIN_SET([DOMAIN([soc]);DOMAIN([V])])) */ c) OKEXTERN;

struct cookie_list *
NNSTART NNVALIDPTR NNROOM_FOR(struct cookie_list)
NNREF(DOMAIN([DEREF([V + 4])]) = THE_STRING([domain_str]))
NNREF(? COOKIE_DOMAIN_GET([THE_STRING([domain_str]);DOMAIN([V])]))
get_cookies(char NULLTERMSTR ICHAR FINAL *STRINGPTR domain_str,
            char NULLTERMSTR FINAL *STRINGPTR path) OKEXTERN;

/* void */
/* delete_cookies(char NULLTERMSTR *START STRINGPTR domain, */
/*                char NULLTERMSTR *START STRINGPTR path) OKEXTERN; */

char NULLTERMSTR ICHAR * STRINGPTR REF(DOMAIN([V]) = DOMAIN([l]))
serialize_cookie_list(struct cookie_list FINAL *l)
OKEXTERN;
#endif