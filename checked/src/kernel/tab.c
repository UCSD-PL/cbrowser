#include <csolve.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "constants.h"
#include "str.h"
#include "tab.h"
#include "proc.h"
#include "util.h"
#include "types.h"

#ifndef CIL
# define validptr(x)
# define assert_same_domain(x, y)
# define domainify(x) x
#else
extern
char NULLTERMSTR * STRINGPTR START
REF(DOMAIN([V]) = THE_STRING([V]))
domainify(char NULLTERMSTR * STRINGPTR) OKEXTERN;
#endif

char NULLTERMSTR * NNSTRINGPTR NNSTART 
tab_title(KERNEL_TABS tabs, int VALID_TAB tab_idx) CHECK_TYPE
{
  if (tabs[tab_idx]) {
//    validptr(tabs[tab_idx]);
    return strdupi(tabs[tab_idx]->tab_origin);
  } 
  return NULL;
}

void
tab_kill(KERNEL_TABS tabs, int VALID_TAB tab_idx, int sig) CHECK_TYPE
{
  if (tabs[tab_idx]) {
    kill(tabs[tab_idx]->proc, sig);
  }
}

char NULLTERMSTR * START ARRAY
get_trusted_origin_suffix()
{
  char *str = malloc(MAX_URI_LEN);
  str = domainify(str);
  /* t->tab_origin = str; */
  printf("Enter trusted origin suffix(e.g, google.com): ");
  fflush(stdout);
  scanf("%63s", str); //stores a null terminator at end
  //TODO: make sure the input isn't longer than this
  //  -> don't want to leave stray input for something else
  return str;
}

void
init_tab_process(struct tab **tabs, int tab_idx, char *init_url)
  CHECK_TYPE
{
  //printf("init_tab_process\n");
  struct tab *t;
  char *args[5]; //args for exec
  char *origin;
  int proc;
  int soc;

  if (t = tabs[tab_idx]) {
    free(t);
  }

  origin  = get_trusted_origin_suffix();
  args[2] = origin;
  args[3] = strdupi(init_url);
  args[4] = NULL;
  init_piped_process(TAB_PROC,
                     args,
                     &proc,
                     &soc);

  t = malloc(sizeof(*t));
  t->tab_origin = origin;
  t->proc = proc;
  t->soc  = soc;
  tabs[tab_idx] = t;
  //Organize tab creation into trusted (for now) routine.
}

/* void */
/* init_tab_process(KERNEL_TABS tabs, int VALID_TAB tab_idx, char NULLTERMSTR *STRINGPTR init_url) */
/*   CHECK_TYPE */
/* { */
/*   //printf("init_tab_process\n"); */
/*   struct tab *t; */
/*   char *args[5]; //args for exec */
/*   char *origin; */
/*   int proc; */
/*   int soc; */

/*   if (t = tabs[tab_idx]) { */
/*     free(t); */
/*   } */

/*   origin = get_trusted_origin_suffix(); */
/*   t = malloc(sizeof(*t)); */
/*   t->tab_origin = origin; */
/*   tabs[tab_idx] = t; */

/*   //    call("/usr/bin/clear", NULL); */
    
/*   /\* if ( strcmp(init_url, "None") != 0 ) { *\/ */
/*   /\*   printf("the tab is navigated to %s\n", init_url); *\/ */
/*   /\*   get_trusted_origin_suffix(tab_idx); *\/ */
/*   /\*   urlparse(init_url); //results placed in global scheme and netloc vars *\/ */
/*   /\*   if (tabs[tab_idx].tab_origin) { *\/ */
/*   /\*     while ( !endswith(netloc, tabs[tab_idx].tab_origin) ) { *\/ */
/*   /\*       printf("The origin suffix you typed is inconsistent with the visiting url\n"); *\/ */
/*   /\*       get_trusted_origin_suffix(tab_idx); *\/ */
/*   /\*     } *\/ */
/*   /\*   } else { *\/ */
/*   /\*     exit(1); //Better error/message here. *\/ */
/*   /\*     return; *\/ */
/*   /\*   } *\/ */
/*   /\* } else { *\/ */
/*   /\* t->tab_origin = get_trusted_origin_suffix(); *\/ */
/*   /\* } *\/ */
/*   //setup args for exec */
/*   args[2] = t->tab_origin; */
/*   args[3] = strdup(init_url); */
/*   args[4] = NULL; */
/*   init_piped_process(TAB_PROC, */
/*                      args, */
/*                      &proc, */
/*                      &soc); */
/*   t->proc = proc; */
/*   t->soc  = soc; */


/*   //Organize tab creation into trusted (for now) routine. */
/* } */
