#include <csolve.h>
#include <stdlib.h>

#define nnparse_string NULLTERMSTR CSOLVE_DOMAIN_STR * NNSTRINGPTR
//NNREF(DOMAIN([V]) = THE_STRING([V]))
#define parse_string NULLTERMSTR CSOLVE_DOMAIN_STR * STRINGPTR
//REF(DOMAIN([V]) = THE_STRING([V]))

struct cookie {
  char nnparse_string NNREF(DOMAIN([V]) = THE_STRING([V])) domain; /*simplification*/
  char nnparse_string attrs;
  char nnparse_string path;
  int  httpOnly;
};

struct get_cookie {
  char nnparse_string NNREF(DOMAIN([V]) = THE_STRING([V])) domain;
  char nnparse_string scheme;
  char nnparse_string path;
};

struct res_cookie {
  int dest;
  struct cookie * NNSTART NNVALIDPTR NNROOM_FOR(struct cookie) cookie;
};

struct cookie_jar {
  struct cookie *c;
  struct cookie_jar *next_jar;
};

int
parse_cookie(struct cookie *c, const char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL * STRINGPTR cooke_str, size_t n) OKEXTERN;

int
parse_get_cookie(struct get_cookie *,
                 const char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL * STRINGPTR get_cookie_str,
                 size_t n) OKEXTERN;

int
parse_res_cookie(struct res_cookie *,
                 const char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL * STRINGPTR res_cookie_str,
                 size_t n) OKEXTERN;

void
print_cookie(struct cookie *c) OKEXTERN;

char *
serialize_cookie(struct cookie *s);

void
set_cookie(struct cookie_jar **cookies, struct cookie *c);

int REF(V != 0 => ? COOKIE_DOMAIN_SET([DOMAIN([d1]);DOMAIN([d2])]))
may_set_cookies(char FINAL parse_string d1, char FINAL parse_string d2) OKEXTERN;

int REF(V != 0 => ? COOKIE_DOMAIN_GET([DOMAIN([d1]);DOMAIN([d2])]))
may_get_cookies(char FINAL parse_string d1, char FINAL parse_string d2) OKEXTERN;

void
assert_may_set(char FINAL parse_string REF(? COOKIE_DOMAIN_SET([DOMAIN([V]); DOMAIN([d2])])) d1,
               char FINAL parse_string REF(? COOKIE_DOMAIN_SET([DOMAIN([d1]); DOMAIN([V])])) d2) OKEXTERN;

