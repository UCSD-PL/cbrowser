#include <csolve.h>
#include <stdlib.h>
#include <string.h>
#include <tags.h>

struct tab {
  char NULLTERMSTR * NNSTART NNSTRINGPTR NNREF(DOMAIN([V]) = THE_STRING([V])) LOC(S)
    origin;

  int REF(DOMAIN([V]) = DOMAIN([origin]))
    fd;
};

struct tab INST(S,X) * NNSTART NNVALIDPTR NNROOM_FOR(struct tab) * ARRAY NNSTART NNVALIDPTR NNSIZE_GE(40) tabs CHECK_TYPE;

void test(int REF(V >= 0) REF(V < 10) i, char NULLTERMSTR * START ARRAY input)
{
  struct tab *t;
  char *s;

  if (!tabs) return;

  t = tabs[i];
  if (t) {
    s = t->origin;
    if (s && !strcmp(input, s)) {
      assert_same_domain(input, s);
    }
  }
}

void bar() OKEXTERN;

void
foo(struct tab * NNSTART NNVALIDPTR NNROOM_FOR(struct tab) * ARRAY NNSTART NNVALIDPTR NNSIZE_GE(40) tabs) CHECK_TYPE
{
  struct tab *t;
  char *tab_origin;
  int tab_idx;

  if (!tabs) return;

  for (tab_idx = 0; tab_idx < 10; tab_idx++) {
    validptr(tabs);
    validptr(tabs + tab_idx);
    if (!(t = tabs[tab_idx])) continue;
    tab_origin = t->origin;
    bar(); //comment to remove type errors
  }
}

extern char NULLTERMSTR * START STRINGPTR REF(DOMAIN([V]) = THE_STRING([V]))
  random_string() OKEXTERN;

extern struct tab INST(S,S) *make_tab() OKEXTERN;

extern
int REF(DOMAIN([V]) = DOMAIN([domain])) REF(V = input)
domainify(int input, char NULLTERMSTR FINAL * START STRINGPTR domain) OKEXTERN;

void
init_tabs(struct tab ** START ARRAY tabs)
{
  int i;
  int fd;
  char *s;
  struct tab *t;

  for(i = 0; i < 10; i++)
  {
    s = random_string();
    fd = nondet();

    t = make_tab();
    t->origin = s;
    t->fd = domainify(fd, s);

    tabs[i] = t;
  }
}

void main()
{
  int i;
  tabs = malloc(10*sizeof(*tabs));
  init_tabs(tabs);

  i = nondet();
  if (0 <= i && i < 10) {
    test(i, random_string());
  }
}
