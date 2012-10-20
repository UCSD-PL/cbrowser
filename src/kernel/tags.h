#include <csolve.h>

#define TAGGED(tag) REF(TAGP([V;tag]) = 1)

void *LOC(L) TAGGED(x) START REF(V = p) 
tag(int x, void * START LOC(L) p)
OKEXTERN;

void
assert_tagged(int x, void * START TAGGED(x) p)
OKEXTERN;
