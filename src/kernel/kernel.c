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

#define VALID_TAB REF(&& [V >= 0; V < MAX_NUM_TABS])

// for use by str.h/urlparse(...)
extern char scheme[SCHEME_SIZE];
extern char netloc[NETLOC_SIZE];

#define kstr char NULLTERMSTR * NNSTRINGPTR NNSTART

struct tab {
  kstr  LOC(L) tab_origin;
  pid_t proc;                //PID of tab process
  int   REF(TAGSET([V]) = Set_sng([V])) FINAL soc; //socket
};

struct uiproc {
  pid_t proc;
  int soc;
};

struct cookie_proc {
  kstr cookie_origin;
  pid_t proc;
  int soc;
};

struct tab INST(L, PROGRAM_NAME_LOC) (SHAPE_IGNORE_BOUND tabs)[MAX_NUM_TABS];  // array of tabs
int curr = 0;                   // current tab
int num_tab = 0;               // number of open tabs

//struct cookie_jar *cookies;
void *cookie_proc_tree = NULL;

struct uiproc ui;

int kargc;
char NULLTERMSTR * NNSTRINGPTR LOC(PROGRAM_NAME_LOC) SIZE_GE(1) * NNSTRINGPTR NNSIZE_GE(4*kargc) NNSTART kargv = NULL;

#define WGET_CMD "/usr/bin/wget -q --tries=1 --timeout=1 -O - -U 'Mozilla/5.0 (X11; Linux i686) AppleWebKit/534.30 (KHTML, like Gecko) Ubuntu/11.04 Chromium/12.0.742.112 Chrome/12.0.742.112 Safari/534.30'"

// output string to stderr of kernel process
void
klog(kstr str)
{
  fprintf(stderr, "K: %s\n", str);
}

/*       char NULLTERMSTR * STRINGPTR SIZE_GE(1) LOC(PROGRAM_NAME_LOC) */
/*                        * START NONNULL ARRAY SIZE(argc * 4) argv) */
void
init_piped_process(const kstr procfile,
                   char NULLTERMSTR * STRINGPTR * START ARRAY VALIDPTR SIZE_GE(8) args,
                   pid_t *proc,
                   int   *soc) 
{
  int socs[2];
  char child[16];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, socs) < 0) {
    //TODO: print some error
  }

  // Add the child socket to the args list
  //  Note: the caller of this function must setup args appropriately
  //      - leave the first space in args empty
  //      - list any args specific to the process
  //      - list the command line args
  //      - NULL
  sprintf(child, "%d", socs[1]);
  args[1] = child;

  *proc = run_proc(procfile, args);
  *soc  = socs[0];
}

void
write_message(message *m) CHECK_TYPE
{
  write_message_soc(m->src_fd, m);
}

message*
read_message(int fd)
{
  message *m;
  m = read_message_soc(fd);
  //assert_tagged(fd, m);
  return m;
}

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

//Get rid of this. options should be parsed & then written out as strings, rather than keeping the strings around.
void
add_kargv(char NULLTERMSTR *NNSTRINGPTR LOC(PROGRAM_NAME_LOC) *ARRAY args,
          int pos) GLOBAL(PROGRAM_NAME_LOC)
{
  int i;
  for (i=1; i<kargc; i++) {
    args[pos+i-1] = kargv[i];
  }
  args[pos+kargc-1] = NULL;
}

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

void
init_tab_process(int tab_idx, char NULLTERMSTR * LOC(PROGRAM_NAME_LOC) STRINGPTR init_url)
  GLOBAL(PROGRAM_NAME_LOC)
{
  //printf("init_tab_process\n");
  char * args[4+MAX_NUM_ARGS]; //args for exec
  int proc;
  int soc;

  //    call("/usr/bin/clear", NULL);
    
  if ( strcmp(init_url, "None") != 0 ) {
    printf("the tab is navigated to %s\n", init_url);
    get_trusted_origin_suffix(tab_idx);
    urlparse(init_url); //results placed in global scheme and netloc vars
    if (tabs[tab_idx].tab_origin) {
      while ( !endswith(netloc, tabs[tab_idx].tab_origin) ) {
        printf("The origin suffix you typed is inconsistent with the visiting url\n");
        get_trusted_origin_suffix(tab_idx);
      }
    } else {
      exit(1); //Better error/message here.
      return;
    }
  } else {
    get_trusted_origin_suffix(tab_idx);
  }
  //setup args for exec
  args[0] = strdup(TAB_PROC);
  args[2] = tabs[tab_idx].tab_origin;
  args[3] = init_url;
  add_kargv(args, 4);
  init_piped_process(TAB_PROC,
                     args,
                     &proc,
                     &soc);
  tabs[tab_idx].proc = proc;
  tabs[tab_idx].soc  = soc;
}

void
process_message(int tab_idx, message *m) CHECK_TYPE
{
  char *content;
  message *response;
  struct cookie c;
  struct cookie_proc *cookie_proc;

  printf("K: process message: tab %d, type %d\n", tab_idx, m->type);
  if (tab_idx == UI_PROC_ID) {
    return;
  }
  /* There's a control flow dependency on &m->type, so propagate
     that information to all data below */
  if (m->content == NULL) return; //error

  switch (m->type) {
  /* case NAVIGATE: */
  /*   init_tab_process(curr, m->content); */
  /*   curr = num_tabs - 1; */
  /*   break; */
  case REQ_URI_FOLLOW:
    if (tab_idx < 0 || tab_idx >= MAX_NUM_TABS) return; //error
    if (m->content == NULL) return; //error
    //write_message(m); to UI
    content = wget(m->content);
    if (!content) {
      return; //error?
    }
    /* The function create_msg should know about the context under
       which it performs any assignments.
       The following reflect the control dependency on m->type. */
    response = create_msg(RES_URI, m->src_fd, content);
    assert_tagged(m->src_fd, response);
    assert_tagged(m->src_fd, response->content);
    write_message(response);
    free(content);
    break;
  /* case DISPLAY_SHM: */
  /*   if (tab_idx == curr) { */
  /*     //create_k2g_display_shm(m, m->shmid, m->size); */
  /*     m->type = K2G_DISPLAY_SHM; */
  /*     write_message(UI_PROC_ID, m); */
  /*   } */
  /*   break; */
  /* case SET_COOKIE: */
  /*   cookie_proc = get_cookie_process(tabs[tab_idx].tab_origin); */
  /*   assert (cookie_proc); */
  /*   parse_cookie(&c, m->content, strlen(m->content)); */
  /*   //Just forward the set-cookie request for now */
  /*   print_cookie(&c); */
  /*   m->type = K2C_SET_COOKIE; */
  /*   write_message_soc(cookie_proc->soc, m); */
  /*   break; */
  /* case C2K_SET_COOKIE: */
  /*   break; */
  default:
    printf("Uhoh! We don't process that message type!\n");
    return;
  }
}

void
get_trusted_origin_suffix(int tab_idx)
{
  tabs[tab_idx].tab_origin = malloc(MAX_URI_LEN);
  printf("Enter trusted origin suffix(e.g, google.com): ");
  fflush(stdout);
  scanf("%63s", tabs[tab_idx].tab_origin); //stores a null terminator at end
  //TODO: make sure the input isn't longer than this
  //  -> don't want to leave stray input for something else
}

void
render(int tab_idx)
{
  message *m;
  m = create_msg(RENDER, tab_fd(tab_idx), NULL);
  write_message(m);
}

void
add_tab()
{
  if (num_tab < MAX_NUM_TABS) {
    curr = num_tab;
    num_tab++;
    init_tab_process(curr, "None"); //TODO: arguments
    /* init_cookie_process(tabs[curr].tab_origin); */
  } else {
    //TODO: print some error
  }
}

void
switch_tab(int tab_idx)
{
  if (tab_idx < num_tab) {
    curr = tab_idx;
    render(tab_idx);
  }
}

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

int
get_tab_idx(char ascii)
{
  if (ascii >= '0' && ascii <= '9') {
    return ascii - '0';
  } else {
    return -1;
  }
}

void
process_input_char(char c)
{
  int tab_idx = get_tab_idx(c);
  if (tab_idx >= 0) {
    switch_tab(tab_idx);
  } else if (c == 'e' /*15*/) {   // F9
    exit(0);
    return;
  } else if (c == 'a' /*16*/) {   // F10
    add_tab();
  } else if (c == 17) {   // F11
    add_tab();
  } else if (c == 18) {   // F12
    //TODO: mouse_click();
    fprintf(stderr, "Ooops! We don't handle mouse clicks yet.\n");
  } else {
    fprintf(stderr, "Ooops! We don't handle that character yet.\n");
    //TODO write message to current tab
  }
}

int
current_tab()
{
  return curr;
}

int
num_tabs()
{
  return num_tab;
}

int
tab_fd(int VALID_TAB tab_idx) CHECK_TYPE
{
  return tabs[tab_idx].soc;
}

char NULLTERMSTR * NNSTRINGPTR NNSTART LOC(PROGRAM_NAME_LOC)
tab_title(int VALID_TAB tab_idx) GLOBAL(PROGRAM_NAME_LOC) CHECK_TYPE
{
  return tabs[tab_idx].tab_origin;
}

void
kill_tab(int VALID_TAB tab_idx, int sig) CHECK_TYPE
{
  kill(tabs[tab_idx].proc, sig);
}

void
kill_ui(int sig)
{
  kill(ui.proc, sig);
}
  


