#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "str.h"
#include "proc.h"
#include "kernel.h"
#include "assert.h"

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

struct tab tabs[MAX_NUM_TABS];  // array of tabs
int curr = 0;                   // current tab
int num_tabs = 0;               // number of open tabs

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
  FILE *p;
  assert(strlen(uri) <= MAX_URI_LEN);

  char *command = malloc(sizeof(WGET_CMD) + MAX_URI_LEN);
  snprintf(command, sizeof(WGET_CMD) + MAX_URI_LEN, WGET_CMD" %s", uri);
  p = popen(command, "r");
  if (p == NULL) {
    fprintf(stderr, "K: Error running wget\n");
  }

  content = calloc(csize, 1);
  while ((n = fread(content + len, 1, csize - len, p)) == csize - len) {
    csize *= 2;
    content = realloc(content, csize);
    len += n;
    if (content == NULL) {
      fprintf(stderr, "K: Error reading wget result");
    }
  }

  len += n;
  if (content != NULL) {
    //Make sure the string is null
    //terminated
    content[len] = 0;
  }

  fclose(p);
  free(command);

  return content;
  // TODO: don't forget that calling function needs to close file stream
}

///////////////////////////
// from PipedProcess class

void
init_piped_process(int tab_idx, const char *procfile, char * args[])
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
    
  ///////
  //int i = 0;
  //while (args[i]) {
  //    printf("%d: %s\n", i, args[i]);
  //    i++;
  //}
  ///////
    
  if (tab_idx == UI_PROC_ID) { //UI Process init
    ui.proc = run_proc(procfile, args);
    ui.soc = socs[0];
  } else { // tab init
    tabs[tab_idx].proc = run_proc(procfile, args);
    tabs[tab_idx].soc = socs[0];
  }
}

void
write_message(int tab_idx, message *m)
{
  if (tab_idx == UI_PROC_ID) {
    write_message_soc(ui.soc, m);
  } else {
    write_message_soc(tabs[tab_idx].soc, m);
  }
}

void
read_message(int tab_idx, message *m)
{
  //printf("read_message tab %d\n", tab_idx);
  if (tab_idx == UI_PROC_ID) {
    read_message_soc(ui.soc, m);
  } else {
    read_message_soc(tabs[tab_idx].soc, m);
  }
  //TODO: klog m
}

/////////////////
// from UIProcess

void
init_ui_process()
{
  char * args[2+MAX_NUM_ARGS]; //args for exec
  args[0] = UI_PROC;
  add_kargv(args, 2);
  init_piped_process(UI_PROC_ID, UI_PROC, args);
}

/////////////////////////
// from TabProcess class

void
add_kargv(char *args[], int pos)
{
  int i;
  for (i=1; i<kargc; i++) {
    args[pos+i-1] = kargv[i];
  }
  args[pos+kargc-1] = NULL;
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
  //args[2] = tabs[tab_idx].tab_origin;
  args[3] = init_url;
  add_kargv(args, 4);
  init_piped_process(tab_idx, TAB_PROC, args);
}

void
process_message(int tab_idx, message *m)
{
  printf("K: process message: tab %d, type %d\n", tab_idx, m->type);
  if (tab_idx == UI_PROC_ID) {
    return;
  }
  switch (m->type) {
  case NAVIGATE:
    init_tab_process(curr, m->uri);
    curr = num_tabs - 1;
    break;
  case REQ_URI_FOLLOW:
    write_message(UI_PROC_ID, m);
    char *content = get_uri_follow(m->uri);
    create_res_uri(m, content);
    write_message(tab_idx, m);
    free(content);
    break;
  case DISPLAY_SHM:
    if (tab_idx == curr) {
      //create_k2g_display_shm(m, m->shmid, m->size);
      m->type = K2G_DISPLAY_SHM;
      write_message(UI_PROC_ID, m);
    }
    break;
    //case REQ_SOCKET:
    //message status_msg;
    //create_set_status(&status_msg, 
    //break;
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

/////////////////////
// from Kernel class

void
add_tab()
{
  if (num_tabs < MAX_NUM_TABS) {
    curr = num_tabs;
    num_tabs++;
    init_tab_process(curr, "None"); //TODO: arguments
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
  if (ascii >=1 && ascii <=4) {
    return ascii - 1;
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
  //TODO
  for (i = 0; i < num_tabs; i++) {
    kill(tabs[i].proc, 9);
  }

  kill(ui.proc, 9);

  exit(0);
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
  FD_ZERO(readfds);
  FD_SET(0, readfds);
  for (i=0; i<num_tabs; i++) {
    soc = tabs[i].soc;
    FD_SET(soc, readfds);
    if (soc > max) {
      max = soc;
    }
  }
  return max;
}

int
get_tabidx_to_read(fd_set *readfds)
{
  int i, soc;
    
  //check UI process
  soc = ui.soc;
  if ( FD_ISSET(soc, readfds) ) {
    return UI_PROC_ID;
  }
    
  //check tabs
  for (i=0; i<num_tabs; i++) {
    soc = tabs[i].soc;
    if ( FD_ISSET(soc, readfds) ) {
      return i;
    }
  }
  return -1;
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
  make_command_args_global(argc, argv);
  parse_options(argc, argv);
  init_ui_process();
  while (1) {
    print_text_display();
    //printf("determining input to handle....\n");
    max_fd = set_readfds(&readfds);
    if( select(max_fd+1, &readfds, NULL, NULL, NULL) > 0 ) {
      //printf("determined input to handle\n");
      if ( FD_ISSET(0, &readfds) ) { //stdin
        /* printf("handling stdin\n"); */
        //c = getchar();
        scanf("%1s", &c);
        process_input_char(c);
        //printf("got input char %c\n", c);
      } else {
        //TODO: read message from one of the processes
        /* printf("message received from process\n"); */
        if ( (tab_idx = get_tabidx_to_read(&readfds)) != -1 ) {
          read_message(tab_idx, &m);
          //printf("message type: %d\nmessage: %s\n", m.type, m.uri);
          //printf("message read\n");
          process_message(tab_idx, &m);
          //printf("message processed\n");
          //return;
        }
      }
    }
  }
  /*char buf[64];
    FILE *f = get_uri_follow("");
    fscanf(f, "%s", buf);
    int e = pclose(f);
    printf("%s\n", buf);
    printf("%d\n", e);*/
}
