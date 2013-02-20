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

typedef struct {
  mtypes         FINAL m_type;
  int            FINAL m_fd;
  char Immutable FINAL m_content;
} message;

typedef struct {
  char Immutable START REF(DOMAIN([V]) = THE_STRING([V])) FINAL gc_domain;
  char Immutable gc_scheme;
  char Immutable gc_path;
  int            gc_httpOnly;
} get_cookie;

typedef struct {
  char Immutable FINAL START SIZE_GE(1) r_host;
  int                  r_port;
  int                  r_family;
  int                  r_type;
  int                  r_protocol;
} req_socket;

#define MsgPtr     V
#define MsgType    (Field(V, 0) : int)
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

#define MessageTagsEq(s) \
  && [ MsgPtrTags        = s                      \
     ; MsgContentPtrTags = s                      \
     ; MsgContentTags    = s                      \
     ] 

#define MessageDomainEq(d)  \
  && [ MsgPtrDomain = d     \
     ; MsgContentDomain = d \
      
#define SAME_DOMAIN(_domain)                    \
  && [MsgPtrDomain = _domain;                   \
      MsgContentDomain = _domain]               \
  
#define ReadMsgPolicy                                         \
  REF(&& [MsgPtrTags        = MsgFdTags;                      \
          MsgPtrDomain      = MsgFdDomain;                    \
          MsgContentTags    = MsgFdTags;                      \
          MsgContentPtrTags = MsgFdTags;                      \
          MsgContentDomain  = MsgFdDomain])

#define ReqSocketPolicy(soc) \
  && [MSG_TYPE(8); MsgContentDomain = MsgFdDomain]

#define WriteMsgPolicy(soc)                                                \
  REF(|| [&& [MSG_TYPE(0); ? Set_emp([Tags(soc)])];                     \
          && [MSG_TYPE(2); TAB_PRIVATE_MSG(Tags(soc))];                 \
          && [MSG_TYPE(2); ? Set_emp([Tags(soc)])];                     \
          && [MSG_TYPE(4)];                                             \
          && [MSG_TYPE(7); TAB_PRIVATE_MSG(Tags(soc))];                 \
          && [MSG_TYPE(8); TAB_PRIVATE_MSG(Tags(soc))];                  \
          && [MSG_TYPE(12); ? COOKIE_DOMAIN_GET([DOMAIN([(DEREF([soc]) : int)]);MsgContentDomain])]])

              /* MsgPtrDomain = MsgFdDomain;                               \ */
              /* MsgContentDomain = MsgFdDomain];                          \ */
#define WriteMsgPtr(s)       MemSafe WriteMsgPolicy(s)

#define ReadMsgPtr           MemSafe ReadMsgPolicy
#define ReadMsgPtrFrom(_s)   ReadMsgPtr REF(MsgFd = _s)

#define ParsedGetCookie(str) NullOrSafe                               \
                             NNREF(TAGSET([V]) = TAGSET([str]))       \
                             NNREF(DOMAIN([V]) = DOMAIN([DEREF([V])]))

message* ReadMsgPtrFrom(soc) 
read_message_soc(int soc) OKEXTERN;

/* void assert_read_msg_t(message FINAL * READ_MSG_T m) OKEXTERN; */

void
check_ok_set_cookie(message FINAL * REF(Domain(Field(V,4) : int) = Domain(Field(V,8)))) OKEXTERN;

/* struct get_cookie FINAL * NNSTART NNVALIDPTR NNROOM_FOR(struct get_cookie) NNREF(TAGSET([V]) = TAGSET([get_cookie_str])) NNREF(DOMAIN([V]) = DOMAIN([DEREF([V])])) */

get_cookie FINAL * ParsedGetCookie(get_cookie_str)
parse_get_cookie(char FINAL Immutable get_cookie_str) OKEXTERN;

message *
create_get_cookie(char *scheme, char *host, char *path, int for_http);

req_socket FINAL * NullOrSafe NNREF(TAGSET([V]) = TAGSET([req_socket_str]))
parse_req_socket(char FINAL Immutable req_socket_str) OKEXTERN;

message *
create_req_socket(char *hostname, int port, int family, int type, int protocol);

#define CreateMessage(_type,_fd,_c)                      \
  MemSafe                                                \
  REF(MsgType          = (DEREF([_type]):int))            \
  REF(MsgFd            = (DEREF([_fd]):int))              \
  REF(MsgPtrTags       = Tags(_type))                     \
  REF(MsgFdTags        = Tags(_fd))                       \
  REF(MsgContentTags   = Tags(_c))                        \
  REF(MsgFdDomain      = Domain((DEREF([_fd]) : int)))    \
  REF(MsgContentDomain = Domain(_c)) 

//add once i can ref it? REF(MsgContentPtrTags = Tags(c))              \

message INST(L,L) FINAL * CreateMessage(type,fd,c)
create_message(mtypes FINAL *type, int FINAL * VALIDPTR HASROOM fd, char FINAL NImmutable LOC(L) c) OKEXTERN;

void
write_message(message FINAL * WriteMsgPtr(V+4) m) OKEXTERN;

message * LOC(L)
START
REF((Field(V,0): int) = (Field(m,0) : int))
REF(Tags(V) = Set_cup([Tags(m); Tags(s)]))
REF(Tags(V + 4) = Set_cup([Tags(m + 4); Tags(s)]))
REF(Tags(V + 8) = Set_cup([Tags(m + 8); Tags(s)]))
REF(Tags(Field(V,8)) = Set_cup([Tags(Field(m,8)); Tags(s)]))
REF(BLOCK_BEGIN([V]) = BLOCK_BEGIN([m]))
tags_xfer_msg(int s, message FINAL * LOC(L) START m) OKEXTERN;

/* Not really final... */
void free_message(message FINAL *m)       OKEXTERN;
void free_req_socket(req_socket FINAL *r) OKEXTERN;
void free_get_cookie(get_cookie FINAL *g) OKEXTERN;
#endif
