#include <csolve.h>

#define MAX_NUM_TABS 10
#define MAX_NUM_ARGS 4
/* #define UI_PROC_ID MAX_NUM_TABS */

#define VALID_TAB REF(&& [V >= 0; V < MAX_NUM_TABS])
