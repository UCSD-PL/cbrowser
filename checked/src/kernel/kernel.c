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
#include "tags.h"
#include "wget.h"
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
render(KERNEL_TABS tabs, int tab_idx)
{
  message *m;
  mtypes render = RENDER;
  struct tab *t;
  int x;
  /* m = create_message(&render, tab_fd(tabs, tab_idx), ""); */
  if (t = tabs[tab_idx]) {
    x = t->soc;
    m = create_message(&render, &x, "");
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
    init_tab_process(tabs, curr, "None"); //TODO: arguments
    t = tabs[curr];
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
handle_req_uri_follow(int tagctx, message *m)
{
  message *r;
  char *content;
  int *uisoc;
  int *res_soc;
  int tags_0;
  int tags_1;
  mtypes follow = REQ_URI_FOLLOW;
  mtypes res    = RES_URI;

  //spec of ui_soc() needs some thought
  uisoc = ui_soc();

  tags_0 = tags_of_ptr(uisoc);
  if (*uisoc > 0) {
    // Send message to the UI process
    r = create_message(&follow, uisoc, m->m_content);
    r = tags_xfer_msg(tags_0, r);
    write_message(r);
    free_message(r);
  }

  // Retrieve contents of the URI stored in m->m_content
  content = wget(mutable_strdup(m->m_content));
  if (! content) return;
  content = immutable_strdup(content);
  content = tags_xfer_ptr(tags_0, content);

  tags_1 = tags_union(tags_0, tags_of_ptr(content));

  // Send the result back
  res_soc=malloc(sizeof(*res_soc));
  *res_soc = m->m_fd;
  res_soc = tags_xfer_ptr2(tags_of_ptr(&(m->m_fd)), res_soc);
  r = create_message(&res, res_soc, content);
  r = tags_xfer_msg(tags_1, r);
  write_message(r);
  free(content);
  free_message(r);
}

void
handle_display_shm(int tagctx, message *m)
{
  int *uisoc;
  int tags_0;
  message *r;
  mtypes dshm = K2G_DISPLAY_SHM;

  uisoc = ui_soc();

  tags_0 = tags_of_ptr(uisoc);
  if (*uisoc > 0) {
    r = create_message(&dshm, uisoc, m->m_content);
    r = tags_xfer_msg(tags_0, r);
    write_message(r);
    free_message(r);
  }
}

/* /\* /\\* void check_domain(char FINAL NULLTERMSTR ICHAR *STRINGPTR d, *\\/ *\/ */
/* /\* /\\*                   SoupCookie FINAL * REF(DOMAIN([V]) = THE_STRING([d]))) OKEXTERN; *\\/ *\/ */

void
handle_set_cookie(KERNEL_TABS tabs, message *m)
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

    if (t->soc == m->m_fd && soup_domain_matches_uri(c->gc_domain, uri)) {
      gc_domain = c->gc_domain;
      domains = gettable_domains(gc_domain);
      assert (domains);
      while (domain = *domains) {
        printf("trying %s...\n", domain);
        if (l) {
          new_l = get_cookies(domain, c->gc_path);
          new_l->cl_next = l;
          l = new_l;
        } else {
          l = get_cookies(domain, c->gc_path);
        }
        domains++;
      }
      if (l) {
        result = serialize_cookie_list(domain, l);
        m = create_message(&getc, res_soc, result);
        write_message(m);
        free_message(m);
        free(result);
        free(res_soc);
        return;
        /* Should free cookie list, too */
      }
      else {
        m = create_message(&getc, res_soc, "");
        write_message(m);
        free_message(m);
      }
    }
  }
  return;
}

void
handle_req_socket(KERNEL_TABS tabs, message *m)
{
  int soc;
  struct hostent *host_info;
  struct sockaddr_in addr;
  req_socket *req;
  long int host_addr = 0;
  mtypes res_socket = RES_SOCKET;
  int *res_soc;
  int tags_0;
  int tags_1;
  char *buf = malloc(4);

  req = parse_req_socket(m->m_content);
  tags_0 = tags_of_ptr(req);

  if (!req) {
    fprintf(stderr, "handle_req_socket: failed to parse request");
    return;
  }

  if((soc = socket(req->r_family, req->r_type, req->r_protocol)) < 0) {
    perror("handle_req_socket: socket()");
    exit(1);
    return; /* csolve */
  }

  host_info = gethostbyname(req->r_host);
  if (!host_info) { return; }
  memcpy(&host_addr, host_info->h_addr, host_info->h_length);
  addr.sin_addr.s_addr = host_addr;
  addr.sin_port = htons(req->r_port);
  addr.sin_family = req->r_family;

  if (connect(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("handle_req_socket: connect()");
    exit(1);
    return; /* csolve */
  }

  sprintf(buf, "%d", soc);
  buf = tags_xfer_ptrb(tags_0, buf);
  tags_1 = tags_of_ptr(buf);

  res_soc = malloc(sizeof(*res_soc));
  *res_soc = m->m_fd;
  res_soc = tags_xfer_ptr2(tags_of_ptr(&(m->m_fd)), res_soc);
  m = create_message(&res_socket, res_soc, immutable_strdup(buf));
  m = tags_xfer_msg(tags_0, m);
  write_message(m);
  /* free_message(m); */
  /* free_req_socket(req); */
}

void
process_message(KERNEL_TABS tabs, int tab_idx, message *m) CHECK_TYPE
{
  int tags_0;
  int tags_1;
  int tags_2;
  int tags_3;
  int tags_4;
  
  message *r;

  printf("K: process message: tab %d, type %d\n", tab_idx, m->m_type);
  
  tags_0 = tags_of_int(tab_idx);
  if (tab_idx == MAX_NUM_TABS) { //doubled as UI, clean this up.
    //tags_0
    return;
  } //then

  /* There's a control flow dependency on &m->m_type, so propagate
     that information to all data below */
  tags_1 = tags_union(tags_of_ptr(m->m_content), tags_0);
  if (m->m_content == NULL) {
    //tags_1

    return; //error
  } //then

  /* tags_2 = tags_union(tags_union(tags_of_int(m->m_type), tags_of_ptr(&m->m_type)), tags_1); */
  switch (m->m_type) {
  case REQ_URI_FOLLOW:
    if (tab_idx < 0 || tab_idx >= MAX_NUM_TABS) return; //error
    if (m->m_content == NULL) return; //error
    handle_req_uri_follow(tags_2, m);
    break;
  case SET_COOKIE:
    handle_set_cookie(tabs, m);
    break;
  case GET_COOKIE:
    handle_get_cookie(tabs, m);
    break;
  case DISPLAY_SHM:
    if (tab_idx == curr) {
      handle_display_shm(tags_2, m);
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
