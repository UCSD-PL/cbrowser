#include "msg.h"
#include "opt.h"

#include <sys/socket.h>
#include <stdint.h>
#include <stdio.h>
#include <endian.h>


extern struct opt_args opt;


#define MSG_CHUNK_SIZE 64


void
create_navigate(message *m, char *s)
{
  m->type = NAVIGATE;
  if (s)
    strncpy(m->uri, s, MAX_URI_LEN);
  //printf("create_navigate: 0x%x\n", m->uri);
  //read_lstr(soc, m->uri);
  //printf("create_navigate m.uri: %s\n", m->uri);
  //printf("create_navigate: 0x%x\n", m->uri);
}

void
create_req_uri_follow(message *m, char *s)
{
  m->type = REQ_URI_FOLLOW;
  if (s)
    strncpy(m->uri, s, MAX_URI_LEN);
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
  m->shmid = shmid;
  m->size = size;
}

void
create_k2g_display_shm(message *m, int shmid, int size)
{
  m->type = K2G_DISPLAY_SHM;
  m->shmid = shmid;
  m->size = size;
}

//void create_set_status(message *m, char *status) {
//    m->type = SET_STATUS;
//}


void
read_message_soc(int soc, message *m)
{
  //printf("read_message_soc\n");
  if (opt.use_length_encoding) {
    read_message_len(soc, m);
  }
}

void
write_message_soc(int soc, message *m)
{
  if (opt.use_length_encoding) {
    write_message_len(soc, m);
  }
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

void
read_lstr(int soc, char *dst) 
{
  //char dst[MAX_URI_LEN];
  //char *dst = m->uri;
  char size_buf[5];
  uint32_t size = 0;
  recv_exact(soc, 4, size_buf);
  size = be32toh(*(uint32_t *)size_buf);
  //printf("size_buf: %s\n", size_buf);
  //unpack the size
  sscanf(size_buf, "%x", &size);
  //printf("size to read: %d\n", size);
  if (size > 0) {
    recv_exact(soc, size, dst);
  }
  //printf("message received: '%s'\n", dst);
  //printf("read_lstr: 0x%x\n", dst);
  //strncpy(m->uri, dst, MAX_URI_LEN);
}

void
read_message_len(int soc, message *m)
{
  //char testm[4];
  //recv(soc, testm, 4, 0);
  //printf("message received: %s\n", testm);
  //return;
  
  
  char msg_id;
  //char buf[MAX_URI_LEN];
  
  //printf("read_message_len\n");
  
  // not sure if this is right
  //recv(soc, &msg_id, 1, 0); // get message id
  read(soc, &msg_id, 1);
  
  /* printf("got msg_id: %d\n", msg_id); */
  
  char buf[9];
  int shmid, size;
  switch (msg_id) {
  case NAVIGATE:
    //read_lstr(soc, buf);
    create_navigate(m, NULL);
    read_lstr(soc, m->uri);
    break;
  case REQ_URI_FOLLOW:
    create_req_uri_follow(m, NULL);
    read_lstr(soc, m->uri);
    break;
  case RENDER:
    create_render(m);
    read_lstr(soc, m->uri); //m->uri serves as a dummy str here
    break;
  /* case K2G_DISPLAY_SHM: */
  /*   read_lstr(soc, buf); */
  /*   sscanf(buf, "%d%d", &shmid, &size); */
  /*   create_k2g_display_shm(m, shmid, size); */
  /*   break; */
  /* case DISPLAY_SHM: */
  /*   read_lstr(soc, buf); */
  /*   sscanf(buf, "%d%d", &shmid, &size); */
  /*   create_display_shm(m, shmid, size); */
  /*   break; */
  case K2G_DISPLAY_SHM:
    m->type = K2G_DISPLAY_SHM;
    read_lstr(soc, m->content);
    break;
  case DISPLAY_SHM:
    m->type = DISPLAY_SHM;
    read_lstr(soc, m->content);
    break;
  case RES_URI:
    read_lstr(soc, buf);
    create_res_uri(m, NULL);
    sscanf(buf, "%p", &(m->content));
    break;
  case KEY_PRESS:
  case MOUSE_CLICK:
  case REQ_SOCKET:
  default:
    printf("Uhoh! We don't read that kind of message yet.\n");
    return;
  }
  //printf("read_message_len m.uri: %s\n", m->uri);
}

void
write_message_len(int soc, message *m)
{
  //char testm[4];
  //sprintf(testm, "%d", 17);
  //send(soc, testm, 4, 0);
  //return;
  char header[5];
  int i;
    
  //determine the str to be sent
  char buf[9]; //used by some message types in payload
  char *payld = payload(m, buf);
    
  //compute message len
  uint32_t len    = strlen(payld);
  uint32_t be_len = htobe32(len);

  char *lenstr = (char *)&be_len;
    
  //send message header
  // type
  header[0] = m->type;
  // size
  memcpy(&header[1], (void *)&be_len, 4);
    
  write(soc, header, 5);
  write(soc, payld, len);
    
  /*
  //send message body in chunks
  for (i=0; len-i>=MSG_CHUNK_SIZE; i+=MSG_CHUNK_SIZE) {
  send(soc, &(payld[i]), MSG_CHUNK_SIZE, 0);
  }
  //send remainder
  if (len-i > 0) {
  send(soc, &(payld[i]), len-i, 0);
  }
  */
  
  //printf("message type: %d\n", m->type);
  //printf("sent message:\n%s\n%s\n", header, payld);
}

char *
payload(message *m, char *buf)
{
  switch (m->type) {
  case NAVIGATE:
  case REQ_URI_FOLLOW:
    return m->uri;
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
