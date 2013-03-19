#ifndef MSG_H
#define MSG_H

#include <csolve.h>
#include <stdio.h>
#include "str.h"
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

#define MsgPtrDomain       MsgPtrF(Domain)
#define MsgFdDomain        MsgFdF(Domain)
#define MsgContentDomain   MsgContentF(Domain)

#define MSG_TYPE(type) ((DEREF([V]) : int) = type)
#define MessageDomainEq(d)  \
  && [ MsgPtrDomain = d     \
     ; MsgContentDomain = d \
      
#define SAME_DOMAIN(_domain)                    \
  && [MsgPtrDomain = _domain;                   \
      MsgContentDomain = _domain]               \
  
#define ReadMsgPolicy                           \
  REF(&& [MsgPtrDomain = MsgFdDomain;           \
          MsgContentDomain = MsgFdDomain])

#define ReqSocketPolicy(soc) \
  && [MSG_TYPE(8); MsgContentDomain = MsgFdDomain]

#define WriteMsgPolicy(soc)                                             \
  REF(MSG_TYPE(12) => ? COOKIE_DOMAIN_GET([DOMAIN([(DEREF([soc]) : int)]);MsgContentDomain]))

#define WriteMsgPtr(s)       MemSafe WriteMsgPolicy(s)

#define ReadMsgPtr           MemSafe ReadMsgPolicy
#define ReadMsgPtrFrom(_s)   ReadMsgPtr REF(MsgFd = _s)

#define ParsedGetCookie(str) NullOrSafe                               \
                             NNREF(DOMAIN([V]) = DOMAIN([DEREF([V])]))

message* ReadMsgPtrFrom(soc) 
read_message_soc(int soc) OKEXTERN;


void
check_ok_set_cookie(message FINAL * REF(Domain(Field(V,4) : int) = Domain(Field(V,8)))) OKEXTERN;


get_cookie FINAL * ParsedGetCookie(get_cookie_str)
parse_get_cookie(char FINAL Immutable get_cookie_str) OKEXTERN;

message *
create_get_cookie(char *scheme, char *host, char *path, int for_http);

req_socket FINAL * NullOrSafe
parse_req_socket(char FINAL Immutable req_socket_str) OKEXTERN;

message *
create_req_socket(char *hostname, int port, int family, int type, int protocol);

#define CreateMessage(_type,_fd,_c)                      \
  MemSafe                                                \
  REF(MsgType          = (DEREF([_type]):int))            \
  REF(MsgFd            = (DEREF([_fd]):int))              \
  REF(MsgFdDomain      = Domain((DEREF([_fd]) : int)))    \
  REF(MsgContentDomain = Domain(_c)) 


message INST(L,L) FINAL * CreateMessage(type,fd,c)
create_message(mtypes FINAL *type, int FINAL * VALIDPTR HASROOM fd, char FINAL NImmutable LOC(L) c) OKEXTERN;

void
write_message(message FINAL * WriteMsgPtr(V+4) m) OKEXTERN;

/* Not really final... */
void free_message(message FINAL *m)       OKEXTERN;
void free_req_socket(req_socket FINAL *r) OKEXTERN;
void free_get_cookie(get_cookie FINAL *g) OKEXTERN;
#endif
