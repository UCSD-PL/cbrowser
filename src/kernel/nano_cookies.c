#include <csolve.h>
#include <stdlib.h>

struct tab {
  char NULLTERMSTR * REF(? DOMAIN([V;V])) STRINGPTR VALIDPTR LOC(D) domain;
  int REF(? DOMAIN([V; domain])) fd;
};

/* extern struct tab INST(D,D) tabs[10] OKEXTERN; */
extern struct tab INST(D,D) tabs[10] OKEXTERN;

void change_domain2(char NULLTERMSTR * LOC(D) STRINGPTR dom)
CHECK_TYPE GLOBAL(D)
{
  tabs[0].domain = "foo";
  //  tabs[0].domain = "ASDF";
}

  
