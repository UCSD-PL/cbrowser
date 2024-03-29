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
#include "ownership.h"

// socket includes
#include <sys/types.h>
#include <sys/socket.h>

#define MAX_NUM_TABS 10
#define MAX_NUM_ARGS 4
#define UI_PROC_ID MAX_NUM_TABS

// for use by str.h/urlparse(...)
extern char scheme[SCHEME_SIZE];
extern char netloc[NETLOC_SIZE];

struct tab {
  char tab_origin[MAX_ORIGIN_LEN];
  pid_t proc;             //PID of tab process
  int soc;                //socket
};

struct uiproc {
  pid_t proc;
  int soc;
};

struct cookie_proc {
  char cookie_origin[MAX_ORIGIN_LEN];
  pid_t proc;
  int soc;
};

struct tab tabs[MAX_NUM_TABS];  // array of tabs
int curr = 0;                   // current tab
int num_tabs = 0;               // number of open tabs

//struct cookie_jar *cookies;
void *cookie_proc_tree = NULL;

struct uiproc ui;

int kargc;
char *kargv[MAX_NUM_ARGS];

#define WGET_CMD "/usr/bin/wget -q --tries=1 --timeout=1 -O - -U 'Mozilla/5.0 (X11; Linux i686) AppleWebKit/534.30 (KHTML, like Gecko) Ubuntu/11.04 Chromium/12.0.742.112 Chrome/12.0.742.112 Safari/534.30'"

// output string to stderr of kernel process
void
klog(char *str)
{
  fprintf(stderr, "K: %s\n", str);
}

char *
get_uri_follow(char *uri)
{
  size_t n = 0;
  size_t len = 0;
  size_t csize = 2048;
  char *content;
  char *command;
  FILE *p;
  assert(strlen(uri) <= MAX_URI_LEN);

  command = malloc(sizeof(WGET_CMD) + MAX_URI_LEN);

  snprintf(command, sizeof(WGET_CMD) + MAX_URI_LEN, WGET_CMD" %s", uri);
  add_readers(get_readers(uri), command);

  p = popen(command, "r");
  add_readers(get_readers(command), (void *)p);
  if (p == NULL) {
    fprintf(stderr, "K: Error running wget\n");
    exit(1);
  }

  content = calloc(csize, 1);
  while ((n = fread(content + len, 1, csize - len, p)) == csize - len) {
    csize *= 2;
    content = realloc(content, csize);
    len += n;
    if (content == NULL) {
      fprintf(stderr, "K: Error reading wget result");
      exit(1);
    }
  }

  len += n;
  if (content != NULL) {
    add_readers(get_readers(p), content);
    //Make sure the string is null
    //terminated
    content[len] = 0;
  }

  pclose(p);
  rm_data(command);
  free(command);

  return content;
}

void
init_piped_process(const char *procfile,
                   char  *args[],
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
write_message(int tab_idx, message *m)
{
  if (tab_idx == UI_PROC_ID) {
    write_message_soc(ui.soc, m);
  } else {
    assert (is_reader(tabs[tab_idx].soc, &(m->type)));
    assert (is_reader(tabs[tab_idx].soc, m->content));
    write_message_soc(tabs[tab_idx].soc, m);
  }
}

void
read_message(int fd, message *m)
{
  //printf("read_message tab %d\n", tab_idx);
  if (fd == UI_PROC_ID) {
    read_message_soc(ui.soc, m);
  } else {
    read_message_soc(fd, m);
    add_reader(fd, &(m->type));
    add_reader(fd, m->content);
  }
  //TODO: klog m
}

void
init_ui_process()
{
  char * args[2+MAX_NUM_ARGS]; //args for exec
  args[0] = UI_PROC;
  add_kargv(args, 2);
  init_piped_process(UI_PROC,
                     args,
                     &ui.proc,
                     &ui.soc);
}

void
add_kargv(char *args[], int pos)
{
  int i;
  for (i=1; i<kargc; i++) {
    args[pos+i-1] = kargv[i];
  }
  args[pos+kargc-1] = NULL;
}

int
cookie_proc_compare(const void *cp1,
                    const void *cp2)
{
  struct cookie_proc *c1 = (struct cookie_proc *)cp1;
  struct cookie_proc *c2 = (struct cookie_proc *)cp2;
  return strcmp(c1->cookie_origin, c2->cookie_origin);
}

struct cookie_proc *
get_cookie_process(char *domain)
{
  struct cookie_proc key;
  struct cookie_proc **retval;

  strcpy(key.cookie_origin, domain);
  retval = tfind(&key, &cookie_proc_tree, cookie_proc_compare);
  assert (retval);
  return *retval;
}

void
init_cookie_process(char *origin)
{
  //Open a socket for this process
  //add struct into hash search table
  int soc;
  struct cookie_proc *cookie_proc;
  char *args[3+MAX_NUM_ARGS];
  assert (origin != NULL);

  cookie_proc = malloc(sizeof(*cookie_proc));
  cookie_proc->proc = 0;
  cookie_proc->soc = -1;
  strcpy(cookie_proc->cookie_origin, origin);

  args[0] = COOKIE_PROC;
  //args[1] will be filled in later
  args[2] = origin;
  add_kargv(args, 3);
  init_piped_process(COOKIE_PROC,
                     args,
                     &cookie_proc->proc,
                     &cookie_proc->soc);

  //Should check the return value here
  tsearch((void *)cookie_proc,
          &cookie_proc_tree,
          cookie_proc_compare);
}

void
init_tab_process(int tab_idx, char *init_url)
{
  //printf("init_tab_process\n");
  char * args[4+MAX_NUM_ARGS]; //args for exec

  //    call("/usr/bin/clear", NULL);
    
  if ( strcmp(init_url, "None") != 0 ) {
    printf("the tab is navigated to %s\n", init_url);
    get_trusted_origin_suffix(tab_idx);
    urlparse(init_url); //results placed in global scheme and netloc vars
    while ( !endswith(netloc, tabs[tab_idx].tab_origin) ) {
      printf("The origin suffix you typed is inconsistent with the visiting url\n");
      get_trusted_origin_suffix(tab_idx);
    }
  } else {
    get_trusted_origin_suffix(tab_idx);
  }
  //setup args for exec
  args[0] = TAB_PROC;
  args[2] = tabs[tab_idx].tab_origin;
  args[3] = init_url;
  add_kargv(args, 4);
  init_piped_process(TAB_PROC,
                     args,
                     &tabs[tab_idx].proc,
                     &tabs[tab_idx].soc);
}

void
process_message(int tab_idx, message *m)
{
  char *content;
  struct cookie c;
  struct cookie_proc *cookie_proc;
  reader_list_t *readers;

  printf("K: process message: tab %d, type %d\n", tab_idx, m->type);
  if (tab_idx == UI_PROC_ID) {
    return;
  }
  /* There's a control flow dependency on &m->type, so propagate
     that information to all data below */
  readers = get_readers(&(m->type));
  r_xfer(m->content, &(m->type));
  switch (m->type) {
  case NAVIGATE:
    init_tab_process(curr, m->content);
    curr = num_tabs - 1;
    break;
  case REQ_URI_FOLLOW:
    write_message(UI_PROC_ID, m);
    content = get_uri_follow(m->content);
    create_res_uri(m, content);
    /* The function create_res_uri should know about the context under
       which it performs any assignments.
       The following reflect the control dependency on m->type. */
    add_readers(readers, m->content);
    add_readers(readers, &(m->type));
    write_message(tab_idx, m);
    r_free(content);
    m->content = NULL;
    break;
  case DISPLAY_SHM:
    if (tab_idx == curr) {
      //create_k2g_display_shm(m, m->shmid, m->size);
      m->type = K2G_DISPLAY_SHM;
      write_message(UI_PROC_ID, m);
    }
    break;
  case SET_COOKIE:
    cookie_proc = get_cookie_process(tabs[tab_idx].tab_origin);
    assert (cookie_proc);
    parse_cookie(&c, m->content, strlen(m->content));
    //Just forward the set-cookie request for now
    print_cookie(&c);
    m->type = K2C_SET_COOKIE;
    write_message_soc(cookie_proc->soc, m);
    break;
  case C2K_SET_COOKIE:
    break;
  default:
    printf("Uhoh! We don't process that message type!\n");
    return;
  }
}

void
get_trusted_origin_suffix(int tab_idx)
{
  printf("Enter trusted origin suffix(e.g, google.com): ");
  fflush(stdout);
  scanf("%63s", tabs[tab_idx].tab_origin); //stores a null terminator at end
  //TODO: make sure the input isn't longer than this
  //  -> don't want to leave stray input for something else
}

void
render(int tab_idx)
{
  message m;
  create_render(&m);
  write_message(tab_idx, &m);
}

void
add_tab()
{
  if (num_tabs < MAX_NUM_TABS) {
    curr = num_tabs;
    num_tabs++;
    init_tab_process(curr, "None"); //TODO: arguments
    init_cookie_process(tabs[curr].tab_origin);
  } else {
    //TODO: print some error
  }
}

void
switch_tab(int tab_idx)
{
  if (tab_idx < num_tabs) {
    curr = tab_idx;
    render(tab_idx);
  }
}

int
find_process_from_chan(int chan)
{
  int i;
  if (ui.soc == chan) {
    return UI_PROC_ID;
  }
  for (i=0; i<num_tabs; i++) {
    if (tabs[i].soc == chan) {
      return i;
    }
  }
  return -1;
}

void
print_tab_title(int tab_idx)
{
  if (tab_idx == curr) {
    printf("*%s*", tabs[tab_idx].tab_origin);
  } else {
    printf("%s", tabs[tab_idx].tab_origin);
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
  for (i=0; i<num_tabs; i++) {
    print_tab_title(i);
    printf(" | ");
  }
  printf("\n");
  printf("--------------------------------------------------------------------"
         "----------\n");
  printf("Enter command: ");
  fflush(stdout);
}

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
    kexit();
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

void
kexit()
{
  int i;
  struct cookie_proc *cp;
  //TODO
  for (i = 0; i < num_tabs; i++) {
    kill(tabs[i].proc, SIGINT);
    cp = get_cookie_process(tabs[i].tab_origin);
    if (cp) {
      kill(cp->proc, SIGINT);
    }
  }

  kill(ui.proc, SIGINT);
  _exit(0);
}

void
make_command_args_global(int argc, char *argv[])
{
  int i;
  if (argc > MAX_NUM_ARGS) {
    //TODO: print some error
  }
  kargc = argc;
  for (i=0; i<argc; i++) {
    kargv[i] = argv[i];
  }
}

int
set_readfds(fd_set *readfds)
{
  int i, soc;
  int max = 0; //stdin
  struct cookie_proc *cookie_proc;
  FD_ZERO(readfds);
  FD_SET(0, readfds);
  for (i=0; i<num_tabs; i++) {
    soc = tabs[i].soc;
    FD_SET(soc, readfds);
    if (soc > max) {
      max = soc;
    }
    cookie_proc = get_cookie_process(tabs[i].tab_origin);
    if (cookie_proc) {
      soc = cookie_proc->soc;
      FD_SET(soc, readfds);
      if (soc > max) {
        max = soc;
      }
    }
  }
  return max;
}

int
get_tabidx_to_read(fd_set *readfds)
{
  int i, soc;
    
  /* //check UI process */
  /* soc = ui.soc; */
  /* if  (FD_ISSET(soc, readfds)) { */
  /*   return UI_PROC_ID; */
  /* } */
    
  //check tabs
  for (i=0; i<num_tabs; i++) {
    soc = tabs[i].soc;
    if (FD_ISSET(soc, readfds)) {
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
main(int argc, char *argv[])
{
  char c;
  fd_set readfds;
  int max_fd;
  int ready_tab;
  message m;
  int tab_idx;
  int fd;
  make_command_args_global(argc, argv);
  parse_options(argc, argv);
  init_ui_process();

  signal(SIGINT, handler);
  atexit(kexit);
  
  
  print_text_display();
  while (1) {
    max_fd = set_readfds(&readfds);
    if (select(max_fd+1, &readfds, NULL, NULL, NULL) > 0) {
      if (FD_ISSET(0, &readfds)) { //stdin
        scanf("%1s", &c);
        process_input_char(c);
      } else {
        for (fd = 1; 0 == FD_ISSET(fd, &readfds); fd++)
          ;
        read_message_soc(fd, &m);
        if ((tab_idx = get_tabidx_to_read(&readfds)) != -1) {
          add_reader(fd, m.content);
        }
        if (fd != ui.soc)
          process_message(tab_idx, &m);
      }
      print_text_display();
    }
  }
}
