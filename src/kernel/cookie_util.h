#include <stdlib.h>

struct cookie {
  char *attrs;
  char *domain;
  char *path;
  int  httpOnly;
};

struct cookie_jar {
  struct cookie *c;
  struct cookie_jar *next_jar;
};

int
parse_cookie(struct cookie *c, const char *cooke_str, size_t n);

void
print_cookie(struct cookie *c);

char *
serialize_cookie(struct cookie *s);

void
set_cookie(struct cookie_jar **cookies, struct cookie *c);

