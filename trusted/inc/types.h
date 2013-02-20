#include <csolve.h>

/* Predicates */
#define DOMAIN_STR MEMPRED(DOMAIN)
#define ICHAR      IMMUTABLE(THE_STRING)
#define VALID_TAB  REF(&& [V >= 0; V < MAX_NUM_TABS])

/* Handy */
#define nn_immutable_string NULLTERMSTR ICHAR * NNSTRINGPTR
#define Immutable NULLTERMSTR ICHAR * STRINGPTR
#define NImmutable NULLTERMSTR ICHAR * NNSTRINGPTR
#define MemSafe OK 
#define NullOrSafe NNOK 
