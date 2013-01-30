#ifndef MSG_H
#define MSG_H

#include <csolve.h>
#include <stdio.h>
#include "str.h"
#include "tags.h"
#include "constants.h"
#include "types.h"

typedef enum {
    K2G_DISPLAY_SHM,
    DISPLAY_SHM,
    REQ_URI_FOLLOW,
    REQ_SOCKET,
    RENDER,
    KEY_PRESS,
    MOUSE_CLICK,
    RES_URI,
    RES_SOCKET,
    NAVIGATE,
    GET_COOKIE,
    SET_COOKIE,
    RES_COOKIE,
    K2C_SET_COOKIE,
    K2C_GET_COOKIE,
    C2K_RES_COOKIE,
    K2T_SET_COOKIE,
    C2K_SET_COOKIE
} mtypes;

#define MsgPtr     V
#define MsgFd      (Field(V, 4) : int)
#define MsgContent Field(V, 8)

#define MsgPtrF(_f)      _f(MsgPtr)
#define MsgFdF(_f)       _f(MsgFd) 
#define MsgContentF(_f)  _f(MsgContent)

#define MsgPtrTags        MsgPtrF(Tags)
#define MsgFdTags         Tags(V + 4)
#define MsgContentTags    MsgContentF(Tags)
#define MsgContentPtrTags Tags(V + 8)

#define MsgPtrDomain       MsgPtrF(Domain)
#define MsgFdDomain        MsgFdF(Domain)
#define MsgContentDomain   MsgContentF(Domain)

#define MSG_TYPE(type) ((DEREF([V]) : int) = type)

#define TAB_PRIVATE_MSG(_tags)             \
  && [ MsgPtrTags = _tags;                 \
       MsgContentTags = _tags]          

#define SAME_DOMAIN(_domain)                    \
  && [MsgPtrDomain = _domain;                   \
      MsgContentDomain = _domain]               \
  
#define READ_MSG_T                                            \
  REF(&& [MsgPtrTags        = MsgFdTags;                       \
          MsgPtrDomain      = MsgFdDomain;                     \
          MsgContentTags    = MsgFdTags;                       \
          MsgContentPtrTags = MsgFdTags;                       \
          MsgContentDomain  = MsgFdDomain])

#define WRITE_MSG_T(soc)                                                \
  REF(|| [&& [MSG_TYPE(0); ? Set_emp([Tags(soc)])];                     \
          && [MSG_TYPE(2); TAB_PRIVATE_MSG(Tags(soc))];                 \
          && [MSG_TYPE(2); ? Set_emp([Tags(soc)])];                     \
          && [MSG_TYPE(4)];                                             \
          && [MSG_TYPE(7); TAB_PRIVATE_MSG(Tags(soc))];                 \
          && [MSG_TYPE(12); ? COOKIE_DOMAIN_GET([DOMAIN([(DEREF([soc]) : int)]); MsgContentDomain])]])

          /* (? Set_emp([Tags(MsgContent)]))])  */

typedef struct {
  mtypes                                          FINAL type;
  int                                             FINAL src_fd;
  char NULLTERMSTR ICHAR * START STRINGPTR LOC(L) /* REF(DOMAIN([V]) = DOMAIN([src_fd])) */ FINAL content;
} message;

#define SAFE_MESSAGE START VALIDPTR ROOM_FOR(message)

message * SAFE_MESSAGE
          READ_MSG_T
          REF((DEREF([V + 4]) : int) = soc)
read_message_soc(int soc) OKEXTERN;

struct get_cookie {
  char immutable_string REF(DOMAIN([V]) = THE_STRING([V])) FINAL domain;
  char immutable_string scheme;
  char immutable_string path;
  int  httpOnly;
};

struct req_socket {
  char immutable_string FINAL host;
  int port;
  int family;
  int type;
  int protocol;
};

void assert_read_msg_t(message FINAL * READ_MSG_T m) OKEXTERN;

void
check_ok_set_cookie(message FINAL * REF(Domain(Field(V,4) : int) = Domain(Field(V,8)))) OKEXTERN;

struct get_cookie FINAL * NNSTART NNVALIDPTR NNROOM_FOR(struct get_cookie) NNREF(TAGSET([V]) = TAGSET([get_cookie_str])) NNREF(DOMAIN([V]) = DOMAIN([DEREF([V])]))
parse_get_cookie(char NULLTERMSTR ICHAR FINAL * STRINGPTR get_cookie_str,
                 size_t n) OKEXTERN;

message *
create_get_cookie(char *scheme, char *host, char *path, int for_http);

struct req_socket FINAL *
parse_req_socket(char NULLTERMSTR ICHAR FINAL * STRINGPTR req_socket_str,
                 size_t n) OKEXTERN;

message *
create_req_socket(char *hostname, int port, int family, int type, int protocol);

message INST(L,L) FINAL * SAFE_MESSAGE
                          /* REF(Tags(V) = Set_cup([Tags(fd); Tags(c)])) */
                          REF(Tags(V)     = Tags(type))
                          REF(Tags(V + 4) = Tags(fd))
                          REF(Tags(V + 8) = Tags(c))
                          REF(Tags(Field(V,8)) = Tags(c))
//                          REF(Tags(Field(V,4) : int) = Tags(fd))
//REF(Tags(Field(V,8)) = Tags(c))
REF((Field(V,0): int) = (DEREF([type]) : int))
REF(Domain(Field(V,4) : int) = Domain((DEREF([fd]) : int)))
REF(Domain(Field(V,8)) = Domain(c))
REF((Field(V, 4) : int) = (DEREF([fd]) : int))
/* REF(THE_STRING([Field(V,8)]) = THE_STRING([c])) */
create_msg(mtypes FINAL * type, int FINAL * VALIDPTR ROOM_FOR(int) fd, char NULLTERMSTR ICHAR FINAL * NNSTRINGPTR LOC(L) c) OKEXTERN;

void write_message(message FINAL * WRITE_MSG_T(V + 4) m) OKEXTERN;

message * LOC(L)
START
REF((Field(V,0): int) = (Field(m,0) : int))
REF(Tags(V) = Set_cup([Tags(m); Tags(s)]))
REF(Tags(V + 4) = Set_cup([Tags(m + 4); Tags(s)]))
REF(Tags(V + 8) = Set_cup([Tags(m + 8); Tags(s)]))
REF(Tags(Field(V,8)) = Set_cup([Tags(Field(m,8)); Tags(s)]))
REF(BLOCK_BEGIN([V]) = BLOCK_BEGIN([m]))
tags_xfer_msg(int s, message FINAL * LOC(L) START m) OKEXTERN;


void free_message(message *m);

void free_req_socket(struct req_socket *r);
void free_get_cookie(struct get_cookie *g);
#endif
