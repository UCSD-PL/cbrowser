#include <csolve.h>
#include <stdlib.h>

/* struct tab { */
/*   char NULLTERMSTR * REF(? DOMAIN([V;V])) STRINGPTR VALIDPTR LOC(D) domain; */
/*   int REF(? DOMAIN([V; domain])) fd; */
/* }; */

struct tab {
  char NULLTERMSTR REF(DOMAIN_FN([VVADDR]) = DOMAIN_FN([V])) * STRINGPTR VALIDPTR LOC(D) domain;
  int REF(DOMAIN_FN([V]) = DOMAIN_FN([domain])) fd;
};

struct tab INST(D,D) * NNSTART NNVALIDPTR NNROOM_FOR(struct tab)
                     * ARRAY NNSTART NNVALIDPTR NNSIZE_GE(40) tabs CHECK_TYPE;

void change_domain()
{
  char *x;
  if (tabs) {
    if (tabs[0] != NULL) {
      tabs[0]->fd = nondet();
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

  
