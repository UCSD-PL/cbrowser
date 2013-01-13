#include <csolve.h>

void assert_eq_strings(char NULLTERMSTR ICHAR FINAL *STRINGPTR REF(THE_STRING([V]) = THE_STRING([s2])) s1,
                       char NULLTERMSTR ICHAR FINAL *STRINGPTR REF(THE_STRING([s1]) = THE_STRING([V])) s2) OKEXTERN;

/* void valid_tab_origin_type(char NULLTERMSTR FINAL *STRINGPTR REF(DOMAIN([V]) = THE_STRING([V]))) OKEXTERN; */
/* void valid_tab(int fd, char NULLTERMSTR FINAL *STRINGPTR REF(DOMAIN([V]) = DOMAIN([fd])) s) */
//  OKEXTERN;

void same_domain_int(int fd, char NULLTERMSTR ICHAR FINAL *STRINGPTR REF(DOMAIN([V]) = DOMAIN([fd])) s)
  OKEXTERN;
void same_domain_eq(char NULLTERMSTR ICHAR FINAL *STRINGPTR REF(DOMAIN([V]) = DOMAIN([s2])) s1,
                    char NULLTERMSTR ICHAR FINAL *STRINGPTR REF(DOMAIN([V]) = DOMAIN([s1])) s2) OKEXTERN;

