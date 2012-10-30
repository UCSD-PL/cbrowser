#include <csolve.h>

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <search.h>
#include "str.h"
#include "proc.h"
#include "kernel.h"
#include "assert.h"
#include "cookie.h"
#include "tags.h"
#include "wget.h"
#include "opt.h"

// socket includes
#include <sys/types.h>
#include <sys/socket.h>

#define MAX_NUM_TABS 10
#define MAX_NUM_ARGS 4
#define UI_PROC_ID MAX_NUM_TABS

// for use by str.h/urlparse(...)
extern char scheme[SCHEME_SIZE];
extern char netloc[NETLOC_SIZE];

/* #define kstr char NULLTERMSTR * NNSTRINGPTR NNSTART */

/* struct tab { */
/*   kstr  LOC(L) tab_origin; */
/*   pid_t proc;                //PID of tab process */
/*   int   REF(TAGSET([V]) = Set_sng([V])) FINAL soc; //socket */
/* }; */

/* struct uiproc { */
/*   pid_t proc; */
/*   int soc; */
/* }; */

/* struct cookie_proc { */
/*   kstr cookie_origin; */
/*   pid_t proc; */
/*   int soc; */
/* }; */

/* struct tab INST(L, PROGRAM_NAME_LOC) (SHAPE_IGNORE_BOUND tabs)[MAX_NUM_TABS];  // array of tabs */
/* int curr = 0;                   // current tab */
/* int num_tabs = 0;               // number of open tabs */

//struct cookie_jar *cookies;
/* void *cookie_proc_tree = NULL; */

/* struct uiproc ui; */

#define WGET_CMD "/usr/bin/wget -q --tries=1 --timeout=1 -O - -U 'Mozilla/5.0 (X11; Linux i686) AppleWebKit/534.30 (KHTML, like Gecko) Ubuntu/11.04 Chromium/12.0.742.112 Chrome/12.0.742.112 Safari/534.30'"

/* void */
/* init_ui_process() */
/* { */
/*   char * args[2+MAX_NUM_ARGS]; //args for exec */
/*   args[0] = UI_PROC; */
/*   add_kargv(args, 2); */
/*   init_piped_process(UI_PROC, */
/*                      args, */
/*                      &ui.proc, */
/*                      &ui.soc); */
/* } */

/* int */
/* cookie_proc_compare(const void *cp1, */
/*                     const void *cp2) */
/* { */
/*   struct cookie_proc *c1 = (struct cookie_proc *)cp1; */
/*   struct cookie_proc *c2 = (struct cookie_proc *)cp2; */
/*   return strcmp(c1->cookie_origin, c2->cookie_origin); */
/* } */

/* struct cookie_proc * */
/* get_cookie_process(char *domain) */
/* { */
/*   struct cookie_proc key; */
/*   struct cookie_proc **retval; */

/*   strcpy(key.cookie_origin, domain); */
/*   retval = tfind(&key, &cookie_proc_tree, cookie_proc_compare); */
/*   assert (retval); */
/*   return *retval; */
/* } */

/* void */
/* init_cookie_process(char *origin) */
/* { */
/*   //Open a socket for this process */
/*   //add struct into hash search table */
/*   int soc; */
/*   struct cookie_proc *cookie_proc; */
/*   char *args[3+MAX_NUM_ARGS]; */
/*   assert (origin != NULL); */

/*   cookie_proc = malloc(sizeof(*cookie_proc)); */
/*   cookie_proc->proc = 0; */
/*   cookie_proc->soc = -1; */
/*   strcpy(cookie_proc->cookie_origin, origin); */

/*   args[0] = COOKIE_PROC; */
/*   //args[1] will be filled in later */
/*   args[2] = origin; */
/*   add_kargv(args, 3); */
/*   init_piped_process(COOKIE_PROC, */
/*                      args, */
/*                      &cookie_proc->proc, */
/*                      &cookie_proc->soc); */

/*   //Should check the return value here */
/*   tsearch((void *)cookie_proc, */
/*           &cookie_proc_tree, */
/*           cookie_proc_compare); */
/* } */


/* int */
/* find_process_from_chan(int chan) */
/* { */
/*   int i; */
/*   if (ui.soc == chan) { */
/*     return UI_PROC_ID; */
/*   } */
/*   for (i=0; i<num_tabs; i++) { */
/*     if (tabs[i].soc == chan) { */
/*       return i; */
/*     } */
/*   } */
/*   return -1; */
/* } */

void
print_tab_title(int tab_idx)
{
  if (tab_idx == current_tab()) {
    printf("*%s*", tab_title(tab_idx));
  } else {
    printf("%s", tab_title(tab_idx));
  }
}

void
print_text_display()
{
  int i;
  //    call("/usr/bin/clear", NULL);
  printf("--------------------------------------------------------------------"
         "----------\n");
  printf("| ");
  for (i=0; i < num_tabs(); i++) {
    print_tab_title(i);
    printf(" | ");
  }
  printf("\n");
  printf("--------------------------------------------------------------------"
         "----------\n");
  printf("Enter command: ");
  fflush(stdout);
}

void
kexit()
{
  int i;
  struct cookie_proc *cp;
  //TODO
  for (i = 0; i < num_tabs(); i++) {
    kill_tab(i, SIGINT);
    /* cp = get_cookie_process(tabs[i].tab_origin); */
    /* if (cp) { */
    /*   kill(cp->proc, SIGINT); */
    /* } */
  }

  kill_ui(SIGINT);
  _exit(0);
}

int
set_readfds(fd_set *readfds)
{
  int i, soc;
  int max = 0; //stdin
  struct cookie_proc *cookie_proc;
  FD_ZERO(readfds);
  FD_SET(0, readfds);
  for (i=0; i<num_tabs(); i++) {
    soc = tab_fd(i);
    FD_SET(soc, readfds);
    if (soc > max) {
      max = soc;
    }
    /* cookie_proc = get_cookie_process(tabs[i].tab_origin); */
    /* if (cookie_proc) { */
    /*   soc = cookie_proc->soc; */
    /*   FD_SET(soc, readfds); */
    /*   if (soc > max) { */
    /*     max = soc; */
    /*   } */
    /* } */
  }
  return max;
}

int 
tab_of_fd(int fd)
{
  int i, soc;
    
  /* //check UI process */
  /* soc = ui.soc; */
  /* if  (FD_ISSET(soc, readfds)) { */
  /*   return UI_PROC_ID; */
  /* } */
  //check tabs
  for (i=0; i<num_tabs(); i++) {
    soc = tab_fd(i);
    if (fd == soc) {
      // csolve_assert(i < MAX_NUM_TABS);
      return i;
    }
  }
  return -1;
}

void
handler(int s)
{
  fprintf(stderr, "Interrupt caught, cleaning up...\n");
  exit(0);
}

int
main (int REF(V > 0) argc,
      char NULLTERMSTR * NNSTRINGPTR SIZE_GE(1) LOC(PROGRAM_NAME_LOC)
                       * START NONNULL ARRAY SIZE(argc * 4) argv)
  GLOBAL(PROGRAM_NAME_LOC) CHECK_TYPE
{
  char c;
  fd_set readfds;
  int max_fd;
  int ready_tab;
  message *m;
  int tab_idx;
  int fd;
  /* make_command_args_global(argc, argv); */
  /* parse_options(argc, argv); */
  /* init_ui_process(); */

  signal(SIGINT, handler);
  atexit(kexit);
  
  print_text_display();
  while (1) {
    max_fd = set_readfds(&readfds);
    if (select(max_fd+1, &readfds, NULL, NULL, NULL) > 0) {
      if (FD_ISSET(0, &readfds)) { //stdin
        scanf("%1s", &c);
        //        c = nondet();
        process_input_char(c);
      } else {
        for (fd = 1; 0 == FD_ISSET(fd, &readfds); fd++)
          ;
        tab_idx = tab_of_fd(fd);
        m = read_message(fd);
        if (tab_idx != -1) {
          process_message(tab_idx, m);
        }
      }
      print_text_display();
    }
  }
}
