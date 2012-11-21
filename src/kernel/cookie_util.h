#include <csolve.h>
#include <stdlib.h>

struct cookie {
  char NULLTERMSTR CSOLVE_DOMAIN_STR * NNSTRINGPTR NNREF(DOMAIN([V]) = THE_STRING([V])) domain; /*simplification*/
  char NULLTERMSTR * NNSTRINGPTR /* NNREF(DOMAIN([V]) = DOMAIN([domain])) */ attrs;
  char NULLTERMSTR * NNSTRINGPTR /* NNREF(DOMAIN([V]) = DOMAIN([domain])) */ path;
  int  httpOnly;
};

struct cookie_jar {
  struct cookie *c;
  struct cookie_jar *next_jar;
};

int
parse_cookie(struct cookie *c, const char NULLTERMSTR FINAL * STRINGPTR cooke_str, size_t n) OKEXTERN;

void
print_cookie(struct cookie *c) OKEXTERN;

char *
serialize_cookie(struct cookie *s);

void
set_cookie(struct cookie_jar **cookies, struct cookie *c);

