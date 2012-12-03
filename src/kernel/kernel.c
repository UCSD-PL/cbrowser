#include <csolve.h>

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <search.h>
#include "str.h"
#include "proc.h"
#include "cookie_util.h"
#include "cookie_proc.h"
#include "ui.h"
#include "kernel.h"
#include "assert.h"
#include "tags.h"
#include "wget.h"
#include "opt.h"

// socket includes
#include <sys/types.h>
#include <sys/socket.h>

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


int curr = 0;                   // current tab
int num_tab = 0;               // number of open tabs

//struct cookie_jar *cookies;

void
klog(kstr str)
{
  fprintf(stderr, "K: %s\n", str);
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
write_message(message FINAL *m)
{
  write_message_soc(m->src_fd, m);
}

message*
read_message(int fd)
{
  return read_message_soc(fd);
}

void
render(KERNEL_TABS tabs, int tab_idx)
{
  message *m;
  //  m = create_msg(RENDER, tab_fd(tabs, tab_idx), "");
  //  write_message(m);
}

void
add_tab(KERNEL_TABS tabs)
{
  struct tab *t;
  if (num_tab < MAX_NUM_TABS) {
    curr = num_tab;
    num_tab++;
    init_tab_process(tabs, curr, "None"); //TODO: arguments
    t = tabs[curr];
    if (t) {
      validptr(t);
      init_cookie_process(t->tab_origin);
    }
  } else {
    //TODO: print some error
  }
}

void
switch_tab(KERNEL_TABS tabs, int tab_idx)
{
  if (tab_idx < num_tab) {
    curr = tab_idx;
    render(tabs, tab_idx);
  }
}

void
process_input_char(KERNEL_TABS tabs, char c) CHECK_TYPE
{
  int tab_idx = get_tab_idx(c);
  if (tab_idx >= 0) {
    switch_tab(tabs, tab_idx);
  } else if (c == 'e' /*15*/) {   // F9
    exit(0);
    return;
  } else if (c == 'a' /*16*/) {   // F10
    add_tab(tabs);
  } else if (c == 17) {   // F11
    add_tab(tabs);
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

void
handle_req_uri_follow(message *m)
{
  message *r;
  char *content;
  int uisoc;

  uisoc = ui_soc();

  if (uisoc > 0) {
    // Send message to the UI process
    r = create_msg(REQ_URI_FOLLOW, uisoc, m->content);
    write_message(r);
  }

  // Retrieve contents of the URI stored in m->content
  content = wget(m->content);
  if (!content) {
    return; //error?
  }

  // Send the result back
  r = create_msg(RES_URI, m->src_fd, content);
  write_message(r);
  free(content);
}

void
handle_display_shm(message *m)
{
  int uisoc;
  message *r;

  uisoc = ui_soc();
  if (uisoc > 0) {
   r = create_msg(K2G_DISPLAY_SHM, uisoc, m->content);
   write_message(r);
  }
}

void assert_eq_strings(char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL *STRINGPTR REF(THE_STRING([V]) = THE_STRING([s2])) s1,
                       char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL *STRINGPTR REF(THE_STRING([s1]) = THE_STRING([V])) s2) OKEXTERN;

/* void valid_tab_origin_type(char NULLTERMSTR FINAL *STRINGPTR REF(DOMAIN([V]) = THE_STRING([V]))) OKEXTERN; */
/* void valid_tab(int fd, char NULLTERMSTR FINAL *STRINGPTR REF(DOMAIN([V]) = DOMAIN([fd])) s) */
  OKEXTERN;

void same_domain_int(int fd, char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL *STRINGPTR REF(DOMAIN([V]) = DOMAIN([fd])) s)
  OKEXTERN;
void same_domain_eq(char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL *STRINGPTR REF(DOMAIN([V]) = DOMAIN([s2])) s1,
                    char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL *STRINGPTR REF(DOMAIN([V]) = DOMAIN([s1])) s2) OKEXTERN;

void
handle_set_cookie(KERNEL_TABS tabs, message *m)
{
  int tab_idx;
  struct cookie c;
  struct cookie_proc *cookie_proc;
  struct tab *t;

  c.attrs = NULL;
  c.domain = NULL;
  c.path = NULL;

  if (m->content) {
    parse_cookie(&c, m->content, strlen(m->content));
  }

  if (!c.domain) return;

  for (tab_idx = 0; tab_idx < MAX_NUM_TABS; tab_idx++) {
    if (!(t = tabs[tab_idx]))
      continue;

    if (t->soc == m->src_fd && may_set_cookies(t->tab_origin, c.domain)) {
      cookie_proc = get_cookie_process(c.domain);
      if (cookie_proc) {
        m = create_msg(K2C_SET_COOKIE, cookie_proc->soc,  m->content);
        write_message(m);
      }
      return;
    }
  }
}

void
handle_get_cookie(KERNEL_TABS tabs, message *m)
{
  int tab_idx;
  struct get_cookie c;
  struct cookie_proc *cookie_proc;
  struct tab *t;

  parse_get_cookie(&c, m->content, strlen(m->content));

  if (!c.domain) return;

  for (tab_idx = 0; tab_idx < MAX_NUM_TABS; tab_idx++) {
    if (!(t = tabs[tab_idx]))
      continue;

    if (t->soc == m->src_fd && may_get_cookies(t->tab_origin, c.domain)) {
      cookie_proc = get_cookie_process(c.domain);
      if (cookie_proc) {
        m = create_msg(K2C_GET_COOKIE, cookie_proc->soc, m->content);
        write_message(m);
      }
      return;
    }
  }
}

void
handle_res_cookie(KERNEL_TABS tabs, message *m)
{
  int tab_idx;
  char *s;
  struct res_cookie *rc;
  struct tab *t;

  rc = malloc(sizeof(*rc));
  parse_res_cookie(rc, m->content, strlen(m->content));

  if ((t = tabs[rc->dest]) && rc->domain && rc->cookies) {
    if (may_get_cookies(t->tab_origin, rc->domain)) {
      s = serialize_cookie_list(rc->num_cookies, rc->cookies);
      m = create_msg(RES_COOKIE, t->soc, s);
      write_message(m);
    }
  }
}

void
process_message(KERNEL_TABS tabs, int tab_idx, message * START VALIDPTR ROOM_FOR(message) READ_MSG_T m) CHECK_TYPE
{
  message *r;

  printf("K: process message: tab %d, type %d\n", tab_idx, m->type);
  
  if (tab_idx == MAX_NUM_TABS) { //doubled as UI, clean this up.
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
    handle_req_uri_follow(m);
    break;
  case DISPLAY_SHM:
    if (tab_idx == curr) {
      handle_display_shm(m);
    }
    break;
  case SET_COOKIE:
    handle_set_cookie(tabs, m);
    break;
  case GET_COOKIE:
    handle_get_cookie(tabs, m);
    break;
  case C2K_RES_COOKIE:
    handle_res_cookie(tabs, m);
    break;
  default:
    printf("Uhoh! We don't process that message type!\n");
    return;
  }
}
