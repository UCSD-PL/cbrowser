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


struct cookie_proc *
get_cookie_proc(char NULLTERMSTR * STRINGPTR domain) OKEXTERN;

void
set_cookie_for(struct tab FINAL * VALIDPTR ROOM_FOR(struct tab) t,
               struct cookie FINAL * //START VALIDPTR ROOM_FOR(struct cookie)
               REF(DEREF([V]) = DEREF([t + 4]))
               //           REF(TAGSET([V]) = Set_sng([DEREF([t]) : int]))
               c) OKEXTERN;

struct tab * REF(V != 0 => DEREF([V]) :int = ifd) NNSTART NNVALIDPTR ROOM_FOR(struct tab)
find_tab(int ifd) OKEXTERN;

struct tab *START VALIDPTR ROOM_FOR(struct tab) * ARRAY START SIZE_GE(40)
init_tabs();

int
main(int fd, struct cookie *c) CHECK_TYPE
{
  struct tab *t;
  t = find_tab(fd);
  if (t) {
    if (c->domain == t->domain) { //Needs to be streq
      set_cookie_for(t, c);
    }
  }
}
