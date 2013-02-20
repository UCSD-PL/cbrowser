#include <csolve.h>
#include "constants.h"
#include "types.h"

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
assert_untagged_ptr(void * REF(? Set_emp([Tags(V)])) x)
OKEXTERN;

void
assert_same_tags(int REF(TAGSET([V]) = TAGSET([y])) x, int y)
OKEXTERN;

void
assert_same_domain(char NULLTERMSTR ICHAR FINAL * STRINGPTR s1,
                   char NULLTERMSTR ICHAR FINAL * REF(DOMAIN([V]) = DOMAIN([s1])) STRINGPTR s2) OKEXTERN;

void
assert_domain_get(char FINAL Immutable s1,
                  char FINAL Immutable REF(?COOKIE_DOMAIN_GET([DOMAIN([s1]);DOMAIN([V])])) s2) OKEXTERN;

int
REF(TAGSET([V]) = TAGSET([s]))
tags_of_int(int s) OKEXTERN;

int
REF(TAGSET([V]) = TAGSET([p]))
tags_of_ptr(void FINAL * VAR(a) p) OKEXTERN;

int
REF(TAGSET([V]) = Set_cup([TAGSET([s1]);TAGSET([s2])]))
tags_union(int s1, int s2) OKEXTERN;

char NULLTERMSTR ICHAR * LOC(L) NNSTART NNSTRINGPTR REF(p > 0 => V > 0)
/* REF(V = p) */
REF(TAGSET([V]) = Set_cup([TAGSET([s]);TAGSET([p])]))
tags_xfer_ptr(int s, char NULLTERMSTR ICHAR FINAL * NNSTART NNSTRINGPTR LOC(L) p) OKEXTERN;

char NULLTERMSTR * LOC(L) NNSTART NNSTRINGPTR REF(p > 0 => V > 0)
/* REF(V = p) */
REF(TAGSET([V]) = Set_cup([TAGSET([s]);TAGSET([p])]))
tags_xfer_ptrb(int s, char NULLTERMSTR FINAL * NNSTART NNSTRINGPTR LOC(L) p) OKEXTERN;

int * START VALIDPTR ROOM_FOR(int) LOC(L) 
/* REF(V = p) */
REF(TAGSET([V]) = Set_cup([TAGSET([s]);TAGSET([p])]))
tags_xfer_ptr2(int s, int * LOC(L) p) OKEXTERN;

int
/* REF(V = d) */
REF(TAGSET([V]) = TAGSET([s]))
tags_xfer_int(int s, int d) OKEXTERN;
