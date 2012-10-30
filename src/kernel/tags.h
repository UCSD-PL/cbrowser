#include <csolve.h>

#define TAGGEDP(_v, _tag) (? Set_mem([_tag; TAGSET([_v])])) 
#define TAGS(_v) TAGSET([_v])
#define UTAGGEDP(_v, _tag) (TAGS(_v) = Set_sng([_tag]))
#define TAGGED(_v, _tag) REF(TAGGEDP(_v, _tag))
#define UTAGGED(_v, _tag) REF(UTAGGEDP(_v, _tag))

#define TagsEq(_v1, _v2) REF(TAGSET([_v1]) = TAGSET([_v2]))

#define Field(_v, _off) DEREF([BLOCK_BEGIN([_v]) + _off])
#define FieldTags(_v, _off)  TAGSET([Field(V, _off)])

void *LOC(L) START REF(V = p) TAGGED(V, x)  
tag(int x, void * START LOC(L) p)
OKEXTERN;

void * START LOC(L) REF(V = dest) REF(TAGS(V) = TAGS(from))
xfer_tags(void * START LOC(L) dest, void * START from)
OKEXTERN;

void
assert_tagged(int x, void FINAL * START TAGGED(V, x) p)
OKEXTERN;

void
assert_uniq_tagged(int x, void FINAL * START UTAGGED(V, x) p)
OKEXTERN;
