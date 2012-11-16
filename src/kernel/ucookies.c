#include <csolve.h>
#include <stdlib.h>
#include <string.h>

struct cookie_proc {
  char NULLTERMSTR * STRINGPTR domain;
  int fd;
};

struct cookie {
  char NULLTERMSTR * NNSTRINGPTR domain;
  char NULLTERMSTR * NNSTRINGPTR data;
};

struct tab {
  int fd;
  char NULLTERMSTR * STRINGPTR LOC(S) domain;
};

extern
struct tab * ROOM_FOR(struct tab) START VALIDPTR REF(? DOMAIN([V;DEREF([V+4])]))
           * SIZE_GE(80) START ARRAY VALIDPTR tabs OKEXTERN;

int foo() {}

extern
valid_tab(struct tab * ROOM_FOR(struct tab) START VALIDPTR REF(? DOMAIN([V;DEREF([V + 4])])) t) OKEXTERN;

extern char NULLTERMSTR * STRINGPTR REF(? DOMAIN([V;V])) rando() OKEXTERN;

int
main() 
{
  tabs[2]->domain = rando();
  valid_tab(tabs[2]);
}
