#ifndef COOKIE_HASH
# define COOKIE_HASH
#include <csolve.h>
#include "cookie_util.h"

//What's a suitable table size?
//Hashing on domain+path
//Thus I declare 53.
#define TABLE_SIZE 53 

struct _cookie_list {
  struct _cookie_list * FINAL NullOrSafe cl_next;
  cookie              * FINAL MemSafe LOC(C) REF(DOMAIN([V]) = THE_STRING([DEREF([V])])) cl_cookie;
} _cookie_list;
typedef struct _cookie_list cookie_list;


/* The cookie hash table */
cookie_list * NullOrSafe * NullOrSafe NNSIZE_GE(TABLE_SIZE*4) ARRAY table CHECK_TYPE;

cookie_list * LOC(L)
append_lists(cookie_list * LOC(L) MemSafe l1, cookie_list * LOC(L) MemSafe l2) OKEXTERN;

void
add_cookie(cookie FINAL * MemSafe
           REF(DOMAIN([DEREF([V + 4])]) = THE_STRING([DEREF([V])])) c) OKEXTERN;

cookie_list * NullOrSafe
NNREF(DOMAIN([DEREF([V + 4])]) = THE_STRING([domain_str]))
NNREF(? COOKIE_DOMAIN_GET([DOMAIN([domain_str]);DOMAIN([V])]))
NNREF(DEREF([V]) > 0 => ? COOKIE_DOMAIN_GET([DOMAIN([domain_str]);DOMAIN([DEREF([V])])]))
get_cookies(char FINAL Immutable REF(DOMAIN([V]) = THE_STRING([V])) domain_str,
            char NULLTERMSTR FINAL * STRINGPTR path) OKEXTERN;

char Immutable REF(DOMAIN([V]) = DOMAIN([l]))
serialize_cookie_list(cookie_list FINAL *l)
OKEXTERN;
#endif

void
assert_gettable_list(char FINAL Immutable s,
                     cookie_list FINAL * REF(?COOKIE_DOMAIN_GET([DOMAIN([s]);DOMAIN([V])])) l) OKEXTERN;
void
assert_gettable_list_fd(int f,
                        cookie_list FINAL * REF(?COOKIE_DOMAIN_GET([DOMAIN([f]);DOMAIN([V])])) l) OKEXTERN;

int REF(?COOKIE_DOMAIN_GET([DOMAIN([a]);DOMAIN([c])]))
check_transitivity(void * REF(?COOKIE_DOMAIN_GET([DOMAIN([V]);DOMAIN([b])])) a,
                   void * REF(?COOKIE_DOMAIN_GET([DOMAIN([V]);DOMAIN([c])])) b,
                   void * c) OKEXTERN;
