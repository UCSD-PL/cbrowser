#include "string.h"
#include "cookie_jar.h"

void
main(char CSOLVE_DOMAIN_STR NULLTERMSTR * STRINGPTR REF(DOMAIN([V]) = THE_STRING([V])) LOC(L) domain,
     char CSOLVE_DOMAIN_STR NULLTERMSTR * LOC(L) STRINGPTR path) CHECK_TYPE
{
  struct cookie_list *l;
  struct cookie *c;
  int i;
  char *s;

  l = get_cookies(domain, path);
  if (l) {
    s = serialize_cookie_list(l);
    assert_may_get2(domain, s);
  }
}
void same_domain_int(int fd, char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL *STRINGPTR REF(DOMAIN([V]) = DOMAIN([fd])) s) OKEXTERN;

void
ok_cookie(struct cookie FINAL * REF(DOMAIN([V]) = DOMAIN([DEREF([V])]))
                                REF(DOMAIN([V]) = THE_STRING([DEREF([V])]))) OKEXTERN;

void main2(int REF(DOMAIN([V]) = DOMAIN([domain])) fd,
           char CSOLVE_DOMAIN_STR NULLTERMSTR * STRINGPTR REF(DOMAIN([V]) = THE_STRING([V])) domain,
           char CSOLVE_DOMAIN_STR NULLTERMSTR * STRINGPTR REF(TAGSET([V]) = Set_sng([fd])) msg_content) CHECK_TYPE
{
  struct cookie *c;


  c = parse_cookie(msg_content, strlen(msg_content));
  if (c && c->domain && may_set_cookies(domain, c->domain)) {
    add_cookie(fd, c);
  }
}


