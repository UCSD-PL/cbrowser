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
  /* m = create_message(&render, tab_fd(tabs, tab_idx), ""); */
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
  mtypes click = MOUSE_CLICK;
  int soc = tabs[tab_idx]->soc;
  message *m = create_message(&click, &soc, read_mouse());
  write_message(m);
  free_message(m);
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
  if (tab_idx >= 0) {
    switch_tab(tabs, tab_idx);
//  } else if (c == 'e' /*15*/) {   // F9
//    exit(0);
//    return;
  } else if (c == 'a' /*16*/) {   // F10
    add_tab(tabs);
  } else if (c == 17) {   // F11
    add_tab(tabs);
  } else if (c == 18) {   // F12
    //TODO: mouse_click();
    handle_mouse_click(tabs, curr);
    //    fprintf(stderr, "Ooops! We don't handle mouse clicks yet.\n");
  } else {
    //if (tabs[curr])
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
  res_soc=malloc(sizeof(*res_soc));
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

/* /\* /\\* void check_domain(char FINAL NULLTERMSTR ICHAR *STRINGPTR d, *\\/ *\/ */
/* /\* /\\*                   SoupCookie FINAL * REF(DOMAIN([V]) = THE_STRING([d]))) OKEXTERN; *\\/ *\/ */

void
handle_set_cookie(KERNEL_TABS tabs, message * ReadMsgPtr m) 
{
  int tab_idx;
  cookie *c;
  SoupURI       *uri;
  SoupCookie    *soup_cookie;
  struct tab *t;

  if (!m->m_content) {
    return;
  }

  for (tab_idx = 0; tab_idx < MAX_NUM_TABS; tab_idx++) {
    if (!(t = tabs[tab_idx]))
      continue;

    if (t->soc == m->m_fd) {
      uri = soup_uri_new(t->tab_origin);

      if (!uri) {
        exit(1);
        return;
      }

      soup_cookie = soup_cookie_parse(m->m_content, uri);
      if (soup_cookie) {
        c = malloc(sizeof(*c));
        c->c_domain = strdupi(t->tab_origin);
        c->c_cookie = soup_cookie;
        add_cookie(c);
        free_cookie(c);
      }
      soup_uri_free(uri);
      return;
    }
  }
}

void
handle_get_cookie(KERNEL_TABS tabs, message * ReadMsgPtr m)  
{
  int w;
  int tab_idx;
  int *res_soc;
  get_cookie *c;
  SoupURI    *uri;
  cookie_list *l = NULL, *new_l = NULL;
  struct tab *t;
  char **domains;
  char *gc_domain = NULL;
  char *domain = NULL;
  char *serial = NULL;
  char *result = NULL;
  char *tmp;
  cookie *test;
  mtypes getc = RES_COOKIE;
  int free_result = 1;
  res_soc = malloc(sizeof(*res_soc));
 *res_soc = m->m_fd;

  c = parse_get_cookie(m->m_content);

  if (!c) return;

  for (tab_idx = 0; tab_idx < MAX_NUM_TABS; tab_idx++) {
    if (!(t = tabs[tab_idx]))
      continue;

    uri = soup_uri_new(t->tab_origin);

    if (!uri) {
      exit(1);
      return;
    }

    if (t->soc == m->m_fd) {
      if (soup_domain_matches_uri(c->gc_domain, uri)) {
        gc_domain = c->gc_domain;
        domains = gettable_domains(gc_domain);
        assert (domains);
        while (domain = *domains) {
          if (l) {
            new_l = get_cookies(domain, c->gc_path);
            new_l->cl_next = l;
            l = new_l;
          } else {
            l = get_cookies(domain, c->gc_path);
          }
          domains++;
        }
      }
      if (l) {
        result = serialize_cookie_list(gc_domain, l);
        if (result) {
          m = create_message(&getc, res_soc, result);
          write_message(m);
          free_message(m);
          free(result);
        }
        free(res_soc);
        /* Should free cookie list, too */
        return;
      }
      else {
        m = create_message(&getc, res_soc, lift_domain(gc_domain,freeze_ptr("")));
        write_message(m);
        free_message(m);
        return;
      }
    }
  }
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

  hints.ai_socktype = req->r_type;
  hints.ai_family   = req->r_family;

  snprintf(port, sizeof(port), "%d", req->r_port);
  if (getaddrinfo(req->r_host, port, &hints, &res) < 0) {
    return;
  }

  for (addr = res; addr != NULL; addr = addr->ai_next) {
    if((soc = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) < 0) {
      perror("handle_req_socket: socket()");
      exit(1);
      return; /* csolve */
    }
    sprintf(buf, "%d", soc);
    if (connect(soc, res->ai_addr, res->ai_addrlen) < 0) {
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
  
  if (tab_idx == MAX_NUM_TABS) { //doubled as UI, clean this up.
    return;
  } //then

  if (m->m_content == NULL) {
    return; //error
  } //then

  switch (m->m_type) {
  case NAVIGATE:
    printf("NAVIGATE TO: (%s)\n",   m->m_content);
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
