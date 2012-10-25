#include <csolve.h>

#define TAGGEDP(_v, _tag) (? Set_mem([_tag; TAGSET([_v])])) 
#define TAGS(_v) TAGSET([_v])
#define TAGGED(_v, _tag) REF(TAGGEDP(_v, _tag))

#define TagsEq(_v1, _v2) REF(TAGSET([_v1]) = TAGSET([_v2]))

#define Field(_v, _off) DEREF([BLOCK_BEGIN([_v]) + _off])
#define FieldTags(_v, _off)  TAGSET([Field(V, _off)])

void *LOC(L) START REF(V = p) TAGGED(V, x)  
tag(int x, void * START LOC(L) p)
OKEXTERN;

void * START REF(V = dst) REF(TAGS(V) = (Set_cup([TAGS(src);TAGS(dst)])))
xfer_tags(void * START dst, void * START src)
OKEXTERN;

void
assert_tagged(int x, void FINAL * START TAGGED(V, x) p)
OKEXTERN;
