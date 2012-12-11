#include <csolve.h>

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <search.h>
#include "str.h"
#include "proc.h"
#include "cookie_proc.h"
#include "ui.h"
#include "kernel.h"
#include "assert.h"
#include "tags.h"
#include "wget.h"

// socket includes
#include <sys/types.h>
#include <sys/socket.h>

#define MAX_NUM_TABS 10
#define MAX_NUM_ARGS 4
#define UI_PROC_ID MAX_NUM_TABS

// for use by str.h/urlparse(...)
extern char scheme[SCHEME_SIZE];
extern char netloc[NETLOC_SIZE];

NNKERNEL_TABS tabs;

void
print_tab_title(int tab_idx)
{
  if (tabs) {
    if (tab_idx == current_tab()) {
      printf("*%s*", tab_title(tabs, tab_idx));
    } else {
      printf("%s", tab_title(tabs, tab_idx));
    }
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
  if (tabs) {
    for (i = 0; i < num_tabs(); i++) {
      tab_kill(tabs, i, SIGINT);
      cp = get_cookie_process(tabs[i]->tab_origin);
      if (cp) {
        kill(cp->proc, SIGINT);
      }
    }
  }

  ui_kill(SIGINT);
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
    soc = tab_fd(tabs, i);
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
    
  //check UI process
  soc = ui_soc();
  if  (soc == fd) {
    return -1;//UI_PROC_ID;
  }
  //check tabs
  for (i=0; i<num_tabs(); i++) {
    soc = tab_fd(tabs, i);
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
  tabs = malloc(10*sizeof(*tabs));
  ui_init();

  signal(SIGINT, handler);
  atexit(kexit);
  
  print_text_display();
  while (1) {
    max_fd = set_readfds(&readfds);
    if (select(max_fd+1, &readfds, NULL, NULL, NULL) > 0) {
      if (FD_ISSET(0, &readfds)) { //stdin
        scanf("%1s", &c);
        //        c = nondet();
        process_input_char(tabs, c);
      } else {
        for (fd = 1; 0 == FD_ISSET(fd, &readfds); fd++)
          ;
        tab_idx = tab_of_fd(fd);

        m = read_message_soc(fd);
        if (tab_idx != -1) {
          process_message(tabs,tab_idx, m);
        }

      }
      print_text_display();
    }
  }
}
