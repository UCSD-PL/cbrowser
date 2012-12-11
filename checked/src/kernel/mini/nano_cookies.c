#include <csolve.h>
#include <stdlib.h>

/* struct tab { */
/*   char NULLTERMSTR * REF(? DOMAIN([V;V])) STRINGPTR VALIDPTR LOC(D) domain; */
/*   int REF(? DOMAIN([V; domain])) fd; */
/* }; */

#define DOMAINSTR REF(DOMAIN_FN([VVADDR]) = DOMAIN_FN([V]))
struct tab {
  char NULLTERMSTR DOMAINSTR * STRINGPTR VALIDPTR LOC(D) domain;
  int REF(DOMAIN_FN([V]) = DOMAIN_FN([domain])) fd;
};

struct tab INST(D,D) * NNSTART NNVALIDPTR NNROOM_FOR(struct tab)
                     * ARRAY NNSTART NNVALIDPTR NNSIZE_GE(40) tabs CHECK_TYPE;

extern int REF(V = 0 => DOMAIN_FN([s1]) = DOMAIN_FN([s2]))
compare_strings(char FINAL DOMAINSTR NULLTERMSTR * ARRAY LOC(L) FINAL s1, char FINAL DOMAINSTR NULLTERMSTR * ARRAY LOC(L) FINAL s2) OKEXTERN;

extern void
assert_ok(char FINAL NULLTERMSTR DOMAINSTR * STRINGPTR VALIDPTR FINAL domain,
          int REF(DOMAIN_FN([V]) = DOMAIN_FN([domain]))) OKEXTERN;

void change_domain()
{
  char *x;
  char *y;
  struct tab *t1;
  struct tab *t2;
  int fd1;
  int fd2;
  if (tabs) {
    t1 = tabs[0];
    t2 = tabs[1];
    if (t1 != NULL && t2 != NULL) {
      x = t1->domain;
      /* y = t2->domain; */
      fd1 = t1->fd;
      /* if (compare_strings(x,y) == 0) { */
        assert_ok(x, fd1);
        /* assert_ok(y, fd1); */
      /* } */
    }
  }
}

extern struct tab INST(D,D) * new_tab(char NULLTERMSTR * STRINGPTR domain, int fd) OKEXTERN;

void
init_tabs()
{
  int i;
  struct tab *t;
  for (i = 0; i < 10; i++) {
    t = new_tab("domain", nondet());
    validptr(t);
    if (tabs) {
      tabs[i] = t;
    }
  }
}

int
main(int  argc, char **argv) CHECK_TYPE
{
  tabs = malloc(10*sizeof(*tabs));
  validptr(tabs);
  init_tabs();
  change_domain();
}

  
