#include <csolve.h>

/* Predicates */
#define DOMAIN_STR MEMPRED(DOMAIN)
//#define ICHAR      IMMUTABLE(THE_STRING)
#define VALID_TAB  REF(&& [V >= 0; V < MAX_NUM_TABS])

#define Field(_v, _off) DEREF([BLOCK_BEGIN([_v]) + _off])
#define Domain(_v) DOMAIN([_v])

char NULLTERMSTR * VALIDPTR LOC(S) REF(Domain(V) = Domain(d)) I
lift_domain(void *d, char NULLTERMSTR * I VALIDPTR LOC(S) s) OKEXTERN;

void * LOC(L) VALIDPTR REF(Domain(V) = Domain(p)) I
freeze_ptr(void * VALIDPTR LOC(L) p) OKEXTERN;


/* Handy */
#define nn_immutable_string NULLTERMSTR ICHAR * NNSTRINGPTR
#define Immutable NULLTERMSTR * STRINGPTR I
#define NImmutable NULLTERMSTR * NNSTRINGPTR I //NNREF(PMUT => (0 = 1)) //IMMUTABLE
#define MemSafe OK 
#define NullOrSafe NNOK 
