#ifndef BROWSER_COOKIE_UTIL
#define BROWSER_COOKIE_UTIL

#include <csolve.h>
#include <stdlib.h>
#include "constants.h"
#include "types.h"
#include <libsoup/soup-types.h>
#ifdef CIL
#define SoupDate int *
/* #define SoupURI  char * */
#endif
#include <libsoup/soup-uri.h>
#include <libsoup/soup-cookie.h>

/* typedef struct { */
/*   char Immutable FINAL LOC(CD) c_domain; */
/*   SoupCookie FINAL * FINAL REF(DOMAIN([V]) = THE_STRING([c_domain])) REF(&&[StructDom(0);StructDom(4);StructDom(8);StructDom(12)]) ROOM_FOR(SoupCookie) START VALIDPTR c_cookie; */
/* } cookie; */

/* void */
/* free_cookie(cookie *c) OKEXTERN; */

int REF(V != 0 => ?COOKIE_DOMAIN_GET([DOMAIN([host_domain]);DOMAIN([c_domain])]))
check_cookie_domain(char FINAL NULLTERMSTR * I STRINGPTR
                    REF(|| [?COOKIE_DOMAIN_GET([DOMAIN([host_domain]);DOMAIN([SOURCE([V])])]);
                            ?COOKIE_DOMAIN_GET([DOMAIN([SOURCE([V])]);DOMAIN([V])])])
                    c_domain,
                        char FINAL NULLTERMSTR * I STRINGPTR REF(DOMAIN([V]) = THE_STRING([V]))
                    host_domain)
  OKEXTERN;


char NULLTERMSTR FINAL * IPTR STRINGPTR
REF(SOURCE([V]) = SOURCE([c])) REF(DOMAIN([V]) = DOMAIN([c]))
format_cookie_header(char FINAL NULLTERMSTR * IPTR STRINGPTR c) OKEXTERN;

char NULLTERMSTR FINAL * IPTR STRINGPTR
REF(|| [DOMAIN([V]) = DOMAIN([c1]); DOMAIN([V]) = DOMAIN([c2])])
join_cookie_headers(char FINAL NULLTERMSTR * IPTR STRINGPTR
                    c1,
                    char FINAL NULLTERMSTR * IPTR STRINGPTR
                    c2)
                        OKEXTERN;

char NULLTERMSTR FINAL *
  I
  NNSTART
  NNSTRINGPTR
  NNREF((VVADDR + 8) < BLOCK_END([VVADDR]))
  NNREF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])]))
  NNREF(DOMAIN([V]) = THE_STRING([V])) FINAL
  * ARRAY START VALIDPTR SIZE_GE(4) //REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([DEREF([V])])]))
gettable_domains(char FINAL NULLTERMSTR * STRINGPTR REF(DOMAIN([V]) = THE_STRING([V])) d) OKEXTERN;
#endif
