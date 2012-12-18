#ifndef COOKIE_HASH
# define COOKIE_HASH
#include <csolve.h>
#include "cookie_util.h"

struct cookie_list {
  struct cookie_list * NNSTART NNVALIDPTR NNSIZE_GE(8) next;
  struct cookie      * START VALIDPTR ROOM_FOR(struct cookie) cookie;
};

void
insert_cookie(struct cookie FINAL *c) OKEXTERN;

struct cookie_list * NNSTART NNVALIDPTR NNROOM_FOR(struct cookie_list)
get_cookies(char NULLTERMSTR CSOLVE_DOMAIN_STR *domain,
            char NULLTERMSTR CSOLVE_DOMAIN_STR *path) OKEXTERN;

/* REF(DOMAIN([DEREF([V + 4])]) = THE_STRING([domain])) */
void
delete_cookies(char NULLTERMSTR *START STRINGPTR domain,
               char NULLTERMSTR *START STRINGPTR path) OKEXTERN;
#endif
