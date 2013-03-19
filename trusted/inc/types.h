#include <csolve.h>

/* Predicates */
#define DOMAIN_STR MEMPRED(DOMAIN)
//#define ICHAR      IMMUTABLE(THE_STRING)
#define VALID_TAB  REF(&& [V >= 0; V < MAX_NUM_TABS])

#define Field(_v, _off) DEREF([BLOCK_BEGIN([_v]) + _off])
#define Domain(_v) DOMAIN([_v])

char NULLTERMSTR * VALIDPTR LOC(S) REF(Domain(V) = Domain(d))
lift_domain(void *d, char NULLTERMSTR * VALIDPTR LOC(S) s) OKEXTERN;

void * LOC(L) START VALIDPTR REF(Domain(V) = Domain(p)) IMMUTABLE
freeze_ptr(void * START VALIDPTR LOC(L) p) OKEXTERN;


/* Handy */
#define nn_immutable_string NULLTERMSTR ICHAR * NNSTRINGPTR
#define Immutable NULLTERMSTR * STRINGPTR IMMUTABLE
#define NImmutable NULLTERMSTR * NNSTRINGPTR IMMUTABLE
#define MemSafe OK 
#define NullOrSafe NNOK 
