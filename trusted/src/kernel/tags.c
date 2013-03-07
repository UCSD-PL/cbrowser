#include "tags.h"

void*
tag(int x, void *p)
{
  return p;
}

void*
xfer_tags(void *dest, void *from)
{
  return dest;
}

void
assert_tagged(int x, void *p)
{
}

void
assert_uniq_tagged(int x, void *p)
{
}

void
assert_untagged_int(int x)
{
}

void
assert_same_tags(int x, int y)
{
}

void
assert_same_domain(char *s1, char *s2)
{
}

int
tags_of_int(int s)
{
}

int
tags_of_ptr(void *p)
{
}

char *
tags_xfer_ptr(int s, char *p)
{
  return p;
}

char *
tags_xfer_ptrb(int s, char *p)
{
  return p;
}

int
tags_union(int s1, int s2)
{
}

char *
domain_strdup(char *s)
{
  return s;
}

int * START VALIDPTR ROOM_FOR(int) LOC(L) 
/* REF(V = p) */
REF(TAGSET([V]) = Set_cup([TAGSET([s]);TAGSET([p])]))
tags_xfer_ptr2(int s, int * LOC(L) p)
{
  return p;
}
