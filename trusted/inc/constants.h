#include <csolve.h>

/* Constants */
#define MAX_NUM_TABS 10
#define MAX_NUM_ARGS 4
#define MAX_COOKIES  50
/* #define UI_PROC_ID MAX_NUM_TABS */

/* Predicates */
#define DOMAIN_STR MEMPRED(DOMAIN)
#define ICHAR REF(V = DATA_AT([VVADDR-BLOCK_BEGIN([VVADDR]);THE_STRING([BLOCK_BEGIN([VVADDR])])]))
#define VALID_TAB  REF(&& [V >= 0; V < MAX_NUM_TABS])

/* Handy */
#define nnparse_string NULLTERMSTR DOMAIN_STR * NNSTRINGPTR
#define parse_string NULLTERMSTR DOMAIN_STR * STRINGPTR


