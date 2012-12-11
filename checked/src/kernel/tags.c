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
assert_untagged_int(int x)
{
}
