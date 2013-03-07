#include "msg.h"
//#include "opt.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

extern struct opt_args opt;

#define MSG_CHUNK_SIZE 64
ssize_t
read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
    struct msghdr   msg;
    struct iovec    iov[1];
    ssize_t         n;
    int             newfd;
    char buf[4096];

    union {
      struct cmsghdr    cm;
      char              control[CMSG_SPACE(sizeof(int))];
    } control_un;
    struct cmsghdr  *cmptr;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    iov[0].iov_base = buf;
    iov[0].iov_len = sizeof(buf);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    fprintf(stderr, "recving...\n");
    if ( (n = recvmsg(fd, &msg, 0)) <= 0)
        return(n);

    if ( (cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
        cmptr->cmsg_len == CMSG_LEN(sizeof(int))) {
      if (cmptr->cmsg_level != SOL_SOCKET) {
        fprintf(stderr, "control level != SOL_SOCKET");
        exit(1);
      }
      if (cmptr->cmsg_type != SCM_RIGHTS) {
        fprintf(stderr, "control type != SCM_RIGHTS");
        exit(1);
      }
        *recvfd = *((int *) CMSG_DATA(cmptr));
    } else
        *recvfd = -1;       /* descriptor was not passed */
    fprintf(stderr, "recv %d=%x\n",*recvfd, *recvfd);
    return(n);
}
/* end read_fd */

ssize_t
write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
    struct msghdr   msg;
    struct iovec    iov[1];
    char buf[1];

    union {
      struct cmsghdr    cm;
      char              control[CMSG_SPACE(sizeof(int))];
    } control_un;
    struct cmsghdr  *cmptr;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);

    cmptr = CMSG_FIRSTHDR(&msg);
    cmptr->cmsg_len = CMSG_LEN(sizeof(int));
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    *((int *) CMSG_DATA(cmptr)) = sendfd;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    if (nbytes < 1) {
      iov[0].iov_base = buf;
      iov[0].iov_len = sizeof(buf);
    } else {
      iov[0].iov_base = ptr;
      iov[0].iov_len = nbytes;
    }
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    fprintf(stderr, "sending %d=%x\n",sendfd, sendfd);
    return(sendmsg(fd, &msg, 0));
}
/* end write_fd */



get_cookie *
parse_get_cookie(char *get_cookie_str)
{
  get_cookie *req;
  char *s;

  req = malloc(sizeof(*req));

  s = strdup(get_cookie_str);
  req->gc_scheme   = strdup(strtok(s, ";"));
  req->gc_domain   = strdup(strtok(NULL, ";"));
  req->gc_path     = strdup(strtok(NULL, ";"));
  req->gc_httpOnly = atoi(strtok(NULL, ";"));

  free(s);
  return req;
}

req_socket *
parse_req_socket(char *req_socket_str)
{
  req_socket *req;
  char *str = strdup(req_socket_str);

  req = malloc(sizeof(*req));

  assert(req_socket_str);
  fprintf(stderr, "parse_req_socket: parsing(%s)\n", req_socket_str);

  req->r_host     = strtok(str, ":");
  req->r_port     = atoi(strtok(NULL,":"));
  req->r_family   = atoi(strtok(NULL,":"));
  req->r_type     = atoi(strtok(NULL,":"));
  req->r_protocol = atoi(strtok(NULL,":"));

  fprintf(stderr, "parse_req_socket: %s -> %s:%d:%d:%d:%d\n",
          req_socket_str,
          req->r_host, req->r_port, req->r_family, req->r_type, req->r_protocol);

  if (req->r_host
      && req->r_port >= 0
      && req->r_family >= 0
      && req->r_type >= 0
      && req->r_protocol >= 0) {
    return req;
  }
  return NULL;
}

message *
create_req_socket(char *hostname, int port, int family, int type, int protocol)
{
  message *m;
  char *str;

  if (asprintf(&str, "%s:%d:%d:%d:%d",
               hostname, port, family, type, protocol) < 0) {
    fprintf(stderr, "create_req_socket() Failed to create socket request string\n");
    exit(1);
  }

  m            = malloc(sizeof(*m));
  m->m_type    = REQ_SOCKET;
  m->m_content = str;

  return m;
}

char *
get_cookie_str(char *scheme, char *host, char *path, int for_http)
{
  char *str;
  if (asprintf(&str, "%s;%s;%s;%d", scheme, host, path, for_http) < 0) {
    fprintf(stderr, "get_cookie_str() Failed to create cookie request string\n");
    exit(1);
  }

  return str;
}

message *
create_get_cookie(char *scheme, char *host, char *path, int for_http)
{
  message *m = malloc(sizeof(*m));
  m->m_type    = GET_COOKIE;
  m->m_content = get_cookie_str(scheme, host, path, for_http); 

  return m;
}

message *
create_message(mtypes *type,
               int *fd,
               char NULLTERMSTR * NNSTRINGPTR NNSTART LOC(S) content)
{
  message *m = malloc(sizeof(*m));

  m->m_type = *type;
  m->m_fd   = *fd;
  m->m_content = NULL;
  if (content)
    m->m_content = strdup(content);

  return m;
}

void
recv_exact(int soc, int size, char *buf)
{
  //NOTE: buf needs to be at least size+1
  int r = read(soc, buf, size);
  buf[size] = '\0';
  if (r != size) printf("read %d bytes, not %d\n", r, size);
}

void
read_lstr(int soc, char **dst) 
{
  char *size_buf = malloc(5);
  uint32_t size;

  //Calculate size of content
  recv_exact(soc, 4, size_buf);
  size = ntohl(*(uint32_t *)size_buf);
  
  if (size > 0) {
    //Allocate a buffer
    *dst = malloc(size+1);

    if (*dst == NULL) exit(1); 

    recv_exact(soc, size, *dst);
  }
  free(size_buf);
}

void
read_message_len(int soc, message *m)
{
  char msg_id;
  read(soc, &msg_id, 1);
  
  char buf[9];
  int shmid, size;
  switch (msg_id) {
  case NAVIGATE:
  case REQ_URI_FOLLOW:
  case RENDER:
  case K2G_DISPLAY_SHM:
  case C2K_SET_COOKIE:
  case DISPLAY_SHM:
  case SET_COOKIE:
  case REQ_SOCKET:
  case RES_SOCKET:
  case GET_COOKIE:
    m->m_type = msg_id;
    m->m_content = NULL;
    if (msg_id == RES_SOCKET) {
      int fd;
      read(soc, buf, 4); //ditch size
      read_fd(soc, NULL, 0, &fd);
      asprintf(&m->m_content, "%d", fd);
    } else {
      read_lstr(soc, &m->m_content);
    }
    break;
  default:
    fprintf(stderr, "read_message_len() Unknown message: %d\n", (int)msg_id);
    m->m_type = -1;
    return;
  }
}

message *
read_message_soc(int soc)
{
  message *m = malloc(sizeof(*m));
  read_message_len(soc, m);
  m->m_fd = soc;
  return m;
}

void
write_message_len(int soc, message *m)
{
  char header[5];
  //determine the str to be sent
  char buf[9]; //used by some message types in payload
    
  //compute message len
  uint32_t len    = strlen(m->m_content);
  uint32_t be_len = htonl(len);

  char *lenstr = (char *)&be_len;
    
  //send message header
  // type
  header[0] = m->m_type;
  // size
  memcpy(&header[1], (void *)&be_len, 4);

  write(soc, header, 5);
  /* hack for sending file descriptors */
  if (m->m_type == RES_SOCKET) {
    write_fd(soc, NULL, 0, atoi(m->m_content));
  } else {
    write(soc, m->m_content, len);
  }
}

void
write_message(message *m)
{
  write_message_len(m->m_fd, m);
}

char *
payload(message *m, char *buf)
{
  switch (m->m_type) {
  case NAVIGATE:
  case REQ_URI_FOLLOW:
    return m->m_content;
  case RENDER:
  case RES_SOCKET:
    return "";
  case K2G_DISPLAY_SHM:
  case DISPLAY_SHM:
    return m->m_content;
    //sprintf(buf, "%d%d", m->shmid, m->size);
    //    return buf;
  case RES_URI:
    //sprintf(buf, "%p", m->m_content);
    //return buf;
    return m->m_content;
  case KEY_PRESS:
  case MOUSE_CLICK:
  default:
    return "Whoops! We can't send that kind of message yet.";
  }
}

message *
tags_xfer_msg(int s, message *m)
{
  return m;
}

void
free_message(message *m)
{
  if (m->m_content)
    free(m->m_content);
  m->m_content = NULL;
  free(m);
}

void free_req_socket(req_socket *r)
{
  free(r->r_host);
  free(r);
}
void free_get_cookie(get_cookie *g)
{
  assert(g->gc_domain);
  free(g->gc_domain);

  assert(g->gc_scheme);
  free(g->gc_scheme);

  assert(g->gc_path);
  free(g->gc_path);
  free(g);
}
