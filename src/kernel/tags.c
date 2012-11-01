#include "tags.h"
void *LOC(L) START REF(V = p) TAGGED(V, x)  
tag(int x, void * START LOC(L) p)
{
  return p;
}

void * START LOC(L) REF(V = dest) REF(TAGS(V) = TAGS(from))
xfer_tags(void * START LOC(L) dest, void * START from)
{
  return dest;
}

void
assert_tagged(int x, void FINAL * START TAGGED(V, x) p)
{
}
