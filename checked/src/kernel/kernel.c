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
#include "cookie_jar.h"
#include "ui.h"
#include "kernel.h"
#include "assert.h"
#include "tags.h"
#include "wget.h"
#include "opt.h"

#include "debug.h"

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

void
handle_set_cookie(KERNEL_TABS tabs, message *m)
{
  int tab_idx;
  struct cookie *c;
  struct tab *t;

  if (m->content) {
    c = parse_cookie(m->content, strlen(m->content));
  } else {
    return;
  }

  if (!c) return;

  for (tab_idx = 0; tab_idx < MAX_NUM_TABS; tab_idx++) {
    if (!(t = tabs[tab_idx]))
      continue;

    if (t->soc == m->src_fd && may_set_cookies(t->tab_origin, c->domain)) {
      add_cookie(m->src_fd, c);
      return;
    }
  }
}

void
handle_get_cookie(KERNEL_TABS tabs, message *m)
{
  int tab_idx;
  struct get_cookie *c;
  struct cookie_list *l;
  struct tab *t;

  c = parse_get_cookie(m->content, strlen(m->content));

  if (!c) return;

  for (tab_idx = 0; tab_idx < MAX_NUM_TABS; tab_idx++) {
    if (!(t = tabs[tab_idx]))
      continue;

    //Need transitivity here
    //if (t->soc == m->src_fd && may_get_cookies(t->tab_origin, c->domain)) {
    if (t->soc == m->src_fd && !strcmp(t->tab_origin, c->domain)) {
      l = get_cookies(c->domain, c->path);
      if (l) {
        m = create_msg(RES_COOKIE, t->soc, serialize_cookie_list(l));
        write_message(m);
      }

      return;
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
  default:
    printf("Uhoh! We don't process that message type!\n");
    return;
  }
}