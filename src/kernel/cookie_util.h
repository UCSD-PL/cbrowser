#include <stdlib.h>

struct cookie {
  char NULLTERMSTR * NNSTRINGPTR attrs;
  char NULLTERMSTR * NNSTRINGPTR domain;
  char NULLTERMSTR * NNSTRINGPTR path;
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

