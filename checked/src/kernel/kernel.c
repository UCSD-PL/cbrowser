#include <csolve.h>

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <search.h>
//#include "str.h"
#include "proc.h"
/* #include "cookie_util.h" */
//#include "cookie_proc.h"
//#include "cookie_jar.h"
#include "cookie_hash.h"
#include "ui.h"
#include "kernel.h"
#include "assert.h"
#include "wget.h"
#include "mouse.h"
#include "opt.h"
#include "util.h"

//#include <libsoup/soup-cookie.h>

#include "debug.h"

// socket includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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

message*
read_message(int fd)
{
  return read_message_soc(fd);
}

void
render(KERNEL_TABS tabs, int REF(V > 0) REF(V < 10) tab_idx)
{
  message *m;
  mtypes render = RENDER;
  struct tab *t;
  int x;
  if (t = tabs[tab_idx]) {
    x = t->soc;
    m = create_message(&render, &x, freeze_ptr(""));
    write_message(m);
    free_message(m);
  }
}

void
add_tab(KERNEL_TABS tabs)
{
  struct tab *t;
  if (num_tab < MAX_NUM_TABS) {
    curr = num_tab;
    num_tab++;
    init_tab_process(tabs, curr, NULL); //TODO: arguments
    t = tabs[curr];
  } else {
    //TODO: print some error
  }
}

void
handle_mouse_click(KERNEL_TABS tabs, int tab_idx)
{
  struct tab *t = NULL;
  mtypes click = MOUSE_CLICK;
  char *mouse;
  if (t = tabs[tab_idx]) {
    int soc = tabs[tab_idx]->soc;
    if (mouse = read_mouse()) {
      message *m = create_message(&click, &soc, freeze_ptr(mouse));
      write_message(m);
      free_message(m);
    }
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
process_input_char(KERNEL_TABS tabs, char c)  CHECK_TYPE
{
  int tab_idx = get_tab_idx(c);
  tab_idx = -1;
  struct tab *t = NULL;
  printf("input char: %c\n", c);
  if (tab_idx >= 0) {
    switch_tab(tabs, tab_idx);
//  } else if (c == 'e' /*15*/) {   // F9
//    exit(0);
//    return;
  } else if (c == '=' /*16*/) {   // F10
    add_tab(tabs);
  } else if (c == 17) {   // F11
    add_tab(tabs);
  } else if (c == 18) {   // F12
    handle_mouse_click(tabs, curr);
  } else {
    if (t = tabs[curr]) {
      message *m;
      mtypes key_press = KEY_PRESS;
      int soc = t->soc;
      char str[2];
      str[0] = c;
      m = create_message(&key_press, &soc, freeze_ptr(str));
      write_message(m);
      free_message(m);
    }
      //write(tabs[curr]->soc, &c, 1);
//    fprintf(stderr, "Ooops! We don't handle that character yet. [%c] %d\n", c, (int)c);
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
  int *uisoc;
  int *res_soc;
  mtypes follow = REQ_URI_FOLLOW;
  mtypes res    = RES_URI;

  //spec of ui_soc() needs some thought
  uisoc = ui_soc();

  if (*uisoc > 0) {
    // Send message to the UI process
    r = create_message(&follow, uisoc, m->m_content);
    write_message(r);
    free_message(r);
  }

  // Retrieve contents of the URI stored in m->m_content
  content = wget(mutable_strdup(m->m_content));
  if (! content) return;

  // Send the result back
  res_soc = malloc(sizeof(*res_soc));
  *res_soc = m->m_fd;
  r = create_message(&res, res_soc, freeze_ptr(content));
  write_message(r);
  free(content);
  free_message(r);
}

void
handle_display_shm(message *m)
{
  int *uisoc;
  message *r;
  mtypes dshm = K2G_DISPLAY_SHM;

  uisoc = ui_soc();

  if (*uisoc > 0) {
    r = create_message(&dshm, uisoc, m->m_content);
    write_message(r);
    free_message(r);
  }
}

void
handle_set_cookie(KERNEL_TABS tabs, message * ReadMsgPtr m)
{
  int tab_idx;
  char *tmp;
  SoupCookie    *soup_cookie;
  struct tab *t;

  if (!m->m_content) {
    return;
  }

  for (tab_idx = 0; tab_idx < MAX_NUM_TABS; tab_idx++) {
    if (!(t = tabs[tab_idx]))
      continue;

    if (t->soc == m->m_fd) {
      soup_cookie = soup_cookie_parse(m->m_content,NULL);

      if (soup_cookie && check_cookie_domain(soup_cookie->domain, t->tab_origin)) {
        if (!table) {
          table = init_table();
        }
        add_cookie(table, soup_cookie);
        /* free_cookie(c); */
      } else {
        fprintf(stderr, "SET-COOKIE FAILED %s\n", m->m_content);
      }
      return;
    }
  }
}

char * 
get_the_cookies(cookie_list ** ARRAY START table, char * ARRAY sink_domain, char * ARRAY path)
{
  char **domains = NULL, *domain = NULL;
  cookie_list_f *new_l = NULL, *l = NULL, *curr_l = NULL;

  domains = gettable_domains(sink_domain);

  while (domain = *domains) {

    new_l = get_cookies(table, domain, path);

    if (l == NULL) {
      l      = new_l;
      curr_l = new_l;
    } else {
      while (curr_l->cl_next) {
        curr_l = curr_l->cl_next;
      }
      curr_l->cl_next = new_l;
    }
    domains++;
  }

  if (l) {
    return serialize_cookie_list(sink_domain, l);
  }

  return NULL;
}

void
handle_get_cookie(KERNEL_TABS tabs, message * ReadMsgPtr m)
{
  int w;
  int tab_idx;
  int *res_soc;
  get_cookie *c;
  cookie_list *l = NULL, *curr_l = NULL, *new_l = NULL;
  struct tab *t;
  char **domains;
  char *gc_domain = NULL;
  char *domain = NULL;
  char *serial = NULL;
  char *result = NULL;
  char *tmp;
  mtypes getc = RES_COOKIE;
  int free_result = 1;
  res_soc = malloc(sizeof(*res_soc));
 *res_soc = m->m_fd;

  c = parse_get_cookie(m->m_content);

  if (!c) return;

  for (tab_idx = 0; tab_idx < MAX_NUM_TABS; tab_idx++) {
    if (!(t = tabs[tab_idx]))
      continue;

    if (t->soc == m->m_fd) {
      gc_domain = c->gc_domain;
      if (check_cookie_domain(gc_domain, t->tab_origin)) {
        if (table) {
          result = get_the_cookies(table, gc_domain, c->gc_path);
          if (result) {
            m = create_message(&getc, res_soc, result);
            write_message(m);
            free_message(m);
            free(result);
            free(res_soc);
            return;
          }
        }
      } else {
        printf("GET-COOKIE FAILED %s %s\n", gc_domain, c->gc_path);
      }
      m = create_message(&getc, res_soc, empty_string(t->tab_origin));
      write_message(m);
      free_message(m);
      free(res_soc);
      return;
    }
  }
  free(res_soc);
  return;
}

void
handle_req_socket(KERNEL_TABS tabs, message * ReadMsgPtr m)
{
  int soc;
  struct hostent *host_info;
  //  struct sockaddr_in addr;
  req_socket *req;
  long int host_addr = 0;
  mtypes res_socket = RES_SOCKET;
  int *res_soc;
  char port[10];
  char *buf = malloc(4);
  struct addrinfo *res, *addr, hints;

  req = parse_req_socket(m->m_content);

  if (!req) {
    fprintf(stderr, "handle_req_socket: failed to parse request");
    return;
  }

//  hints.ai_socktype = req->r_type;
//  hints.ai_family   = req->r_family;

  snprintf(port, sizeof(port), "%d", req->r_port);
  if (getaddrinfo(req->r_host, port, NULL,/* &hints, */ &res) < 0) {
    perror("getaddrinfo()");
    return;
  }

  for (addr = res; addr != NULL; addr = addr->ai_next) {
    if((soc = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) < 0) {
      perror("handle_req_socket: socket()");
      exit(1);
      return; /* csolve */
    }
    sprintf(buf, "%d", soc);
    if (connect(soc, addr->ai_addr, addr->ai_addrlen) < 0) {
      close(soc);
      continue; /* csolve */
    }

    res_soc = malloc(sizeof(*res_soc));
    *res_soc = m->m_fd;
    m = create_message(&res_socket, res_soc, freeze_ptr(buf));
    write_message(m);
    free_message(m);
    break;
  }

  free_req_socket(req);
  free(buf);
}

void
process_message(KERNEL_TABS tabs, int tab_idx, message *m) CHECK_TYPE
{
  message *r;

  printf("K: process message: tab %d, type %d\n", tab_idx, m->m_type);
  
  if (!(0 <= tab_idx && tab_idx < MAX_NUM_TABS)) {
    return;
  } //then

  if (m->m_content == NULL) {
    return; //error
  } //then

  switch (m->m_type) {
  case NAVIGATE:
    tab_kill(tabs, tab_idx, SIGINT);
    init_tab_process(tabs, tab_idx, m->m_content);
    break;
  case REQ_URI_FOLLOW:
    if (tab_idx < 0 || tab_idx >= MAX_NUM_TABS) return; //error
    if (m->m_content == NULL) return; //error
    handle_req_uri_follow(m);
    break;
  case SET_COOKIE:
    handle_set_cookie(tabs, m);
    break;
  case GET_COOKIE:
    handle_get_cookie(tabs, m);
    break;
  case DISPLAY_SHM:
    if (tab_idx == curr) {
      handle_display_shm(m);
    }
    break;
  case REQ_SOCKET:
    if (m->m_content == NULL) return; //error
    handle_req_socket(tabs, m);
    break;
  default:
    printf("Uhoh! We don't process that message type!\n");
    return;
  }
}
