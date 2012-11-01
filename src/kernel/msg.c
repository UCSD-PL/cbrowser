#include "msg.h"
#include "opt.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//Sigh
#ifdef __APPLE__
  #define htobe32 htonl
  #define be32toh ntohl
#endif

extern struct opt_args opt;

#define MSG_CHUNK_SIZE 64

void
create_navigate(message *m, char *s)
{
  m->type = NAVIGATE;
  if (s)
    strncpy(m->content, s, MAX_URI_LEN);
}

void
create_req_uri_follow(message *m, char *s)
{
  m->type = REQ_URI_FOLLOW;
  m->content = s;
}

void
create_res_uri(message *m, char *content)
{
  m->type = RES_URI;
  m->content = content;
}

void
create_render(message *m)
{
  m->type = RENDER;
}

void
create_display_shm(message *m, int shmid, int size)
{
  m->type = DISPLAY_SHM;
  //  m->shmid = shmid;
  //m->size = size;
}

void
create_k2g_display_shm(message *m, int shmid, int size)
{
  m->type = K2G_DISPLAY_SHM;
  //m->shmid = shmid;
  //m->size = size;
}

//void create_set_status(message *m, char *status) {
//    m->type = SET_STATUS;
//}
message *
create_msg(mtypes type,
           int fd,
           char NULLTERMSTR * NNSTRINGPTR NNSTART LOC(S) content)
{
  message *m = malloc(sizeof(*m));

  m->type = type;
  m->src_fd = fd;
  m->content = content;

  return m;
}

void
read_lstr(int soc, char **dst) 
{
  char size_buf[5];
  uint32_t size;

  //Calculate size of content
  recv_exact(soc, 4, size_buf);
  size = be32toh(*(uint32_t *)size_buf);
  //  sscanf(size_buf, "%x", &size);
  
  if (size > 0) {
    //Allocate a buffer
    *dst = calloc(size, 1);

    if (*dst == NULL) exit(1); 

    recv_exact(soc, size, *dst);
  }
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
    m->type = msg_id;
    read_lstr(soc, &m->content);
    break;
  default:
    m->type = -1;
    return;
  }
  //printf("read_message_len m.uri: %s\n", m->uri);
}

message *
read_message_soc(int soc)
{
  message *m = malloc(sizeof(*m));
  //printf("read_message_soc\n");
  read_message_len(soc, m);
  m->src_fd = soc;
  return m;
}

void
write_message_len(int soc, message *m)
{
  char header[5];
  //determine the str to be sent
  char buf[9]; //used by some message types in payload
    
  //compute message len
  uint32_t len    = strlen(m->content);
  uint32_t be_len = htobe32(len);

  char *lenstr = (char *)&be_len;
    
  //send message header
  // type
  header[0] = m->type;
  // size
  memcpy(&header[1], (void *)&be_len, 4);
    
  write(soc, header, 5);
  write(soc, m->content, len);
}

void
write_message_soc(int soc, message *m)
{
  write_message_len(soc, m);
}

void
recv_exact(int soc, int size, char *buf)
{
  //NOTE: buf needs to be at least size+1
  
  /* printf("recv_exact %d\n", size); */
  int r = read(soc, buf, size);
  buf[size] = '\0';
  if (r != size) printf("read %d bytes, not %d\n", r, size);
  //return;
  
  /*int s = 0;
    while (s < size) {
    s = recv(soc, &(buf[s]), size-s, 0);
    //printf("s=%d\n", s);
    //perror("recv");
    //break;
    }
    buf[size] = '\0';
  */
}

char *
payload(message *m, char *buf)
{
  switch (m->type) {
  case NAVIGATE:
  case REQ_URI_FOLLOW:
    return m->content;
  case RENDER:
  case RES_SOCKET:
    return "";
  case K2G_DISPLAY_SHM:
  case DISPLAY_SHM:
    return m->content;
    //sprintf(buf, "%d%d", m->shmid, m->size);
    //    return buf;
  case RES_URI:
    //sprintf(buf, "%p", m->content);
    //return buf;
    return m->content;
  case KEY_PRESS:
  case MOUSE_CLICK:
  default:
    return "Whoops! We can't send that kind of message yet.";
  }
}
