#include <csolve.h>

#define TAGGEDP(_v, _tag) (? Set_mem([_tag; TAGSET([_v])])) 
#define UTAGGEDP(_v, _tag) (TAGS(_v) = Set_sng([_tag]))

#define Tagged(_v, _tag) REF(TAGGEDP(_v, _tag))
#define UTagged(_v, _tag) REF(UTAGGEDP(_v, _tag))

#define Tags(_v) TAGSET([_v])
#define TagsEq(_v1, _v2) REF(TAGSET([_v1]) = TAGSET([_v2]))

#define Field(_v, _off) DEREF([BLOCK_BEGIN([_v]) + _off])
#define FieldTags(_v, _off)  TAGSET([Field(V, _off)])

#define Domain(_v) DOMAIN([_v])

void *LOC(L) START REF(V = p) Tagged(V, x)  
tag(int x, void * START LOC(L) p)
OKEXTERN;

void * START LOC(L) REF(V = dest) TagsEq(V, from)//REF(TAGS(V) = TAGS(from))
xfer_tags(void * START LOC(L) dest, void * START from)
OKEXTERN;

void
assert_tagged(int x, void FINAL * START Tagged(V, x) p)
OKEXTERN;

void
assert_uniq_tagged(int x, void FINAL * START UTagged(V, x) p)
OKEXTERN;

void
assert_untagged_int(int REF(? Set_emp([Tags(V)])) x)
OKEXTERN;

void
assert_same_tags(int REF(TAGSET([V]) = TAGSET([y])) x, int y)
OKEXTERN;

void
assert_same_domain(char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL * STRINGPTR s1,
                   char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL * REF(DOMAIN([V]) = DOMAIN([s1])) STRINGPTR s2) OKEXTERN;
