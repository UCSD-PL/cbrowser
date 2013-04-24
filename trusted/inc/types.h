#include <csolve.h>

#define I IPTR
#define M MPTR

/* Predicates */
#define DOMAIN_STR MEMPRED(DOMAIN)
//#define ICHAR      IMMUTABLE(THE_STRING)
#define VALID_TAB  REF(&& [V >= 0; V < MAX_NUM_TABS])

#define Field(_v, _off) DEREF([BLOCK_BEGIN([_v]) + _off])
#define Domain(_v) DOMAIN([_v])
#define String(_v) THE_STRING([_v])
#define Source(_v) SOURCE([_v])

void * LOC(L) VALIDPTR REF(Domain(V) = Domain(p)) I
freeze_ptr(void * VALIDPTR LOC(L) p) OKEXTERN;

/* Handy */
#define nn_immutable_string NULLTERMSTR ICHAR * NNSTRINGPTR
#define Immutable NULLTERMSTR * STRINGPTR I
#define NImmutable NULLTERMSTR * NNSTRINGPTR I //NNREF(PMUT => (0 = 1)) //IMMUTABLE
#define MemSafe OK 
#define NullOrSafe NNOK 

void
assert_src_eq(int REF(SOURCE([V]) = p2),
              int p2) OKEXTERN;
void
assert_src_domain(char FINAL NULLTERMSTR * STRINGPTR REF(DOMAIN([SOURCE([V])]) = DOMAIN([p2])) p1,
                  char FINAL NULLTERMSTR * STRINGPTR p2) OKEXTERN;
