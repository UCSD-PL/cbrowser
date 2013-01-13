#include <csolve.h>

/* Predicates */
//#define DOMAIN_STR MEMPRED(DOMAIN)
#define ICHAR //IMMUTABLE(THE_STRING)
#define VALID_TAB  REF(&& [V >= 0; V < MAX_NUM_TABS])

/* Handy */
#define nn_immutable_string NULLTERMSTR ICHAR * NNSTRINGPTR
#define nn_string NULLTERMSTR ICHAR * STRINGPTR

//DOMAIN([V]) = DOMAIN(CHAR_AT(STRING(VVADDR), i))
//V           = CHAR_AT(STRING(VVADDR), i)
strcmp(s1, s2) => string(s1) = string(s2)
