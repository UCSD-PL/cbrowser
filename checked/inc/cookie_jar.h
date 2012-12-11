#ifndef BROWSER_COOKIE_JAR
#define BROWSER_COOKIE_JAR

#include <csolve.h>
#include "cookie_util.h"

#define CookiePtr START VALIDPTR ROOM_FOR(struct cookie)
#define CookieList(n) START ARRAY VALIDPTR SIZE_GE(n*4)

struct cookie_list {
  struct cookie_list * NNSTART NNVALIDPTR NNSIZE_GE(8) next;
  struct cookie      * START VALIDPTR ROOM_FOR(struct cookie) cookie;
};

void
add_cookie(int soc,
           struct cookie FINAL * CookiePtr REF(TAGSET([V]) = TAGSET([soc]))
           REF(? COOKIE_DOMAIN_SET([DOMAIN([soc]);DOMAIN([V])])) cookie)
OKEXTERN;

//Here it should be the case that CAN_READ_COOKIE(domain, v) for each cookie
//So maybe the type of a cookie should be something like the domain
//of a cookie pointer is the same as the cookie's domain field etc etc?
struct cookie_list * 
NNSTART NNVALIDPTR NNROOM_FOR(struct cookie_list)
NNREF(? COOKIE_DOMAIN_GET([DOMAIN([domain]); DOMAIN([V])]))
get_cookies(char CSOLVE_DOMAIN_STR NULLTERMSTR * LOC(L)
            STRINGPTR REF(DOMAIN([V]) = THE_STRING([V])) domain,
            char CSOLVE_DOMAIN_STR NULLTERMSTR * LOC(L)
            STRINGPTR path)
OKEXTERN;

struct cookie_list * 
START VALIDPTR ROOM_FOR(struct cookie_list)
REF(? COOKIE_DOMAIN_GET([DOMAIN([domain]); DOMAIN([V])]))
mk_list_node(char FINAL parse_string domain, struct cookie FINAL *c) OKEXTERN;

char CSOLVE_DOMAIN_STR NULLTERMSTR * START STRINGPTR REF(DOMAIN([V]) = DOMAIN([l]))
serialize_cookie_list(struct cookie_list FINAL *l)
OKEXTERN;

#endif
