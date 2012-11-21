#ifndef MSG_H
#define MSG_H

#include <csolve.h>
#include <stdio.h>

#include "str.h"
#include "tags.h"


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

#define MsgPtrTags       MsgPtrF(Tags)
#define MsgFdTags        MsgFdF(Tags)
#define MsgContentTags   MsgContentF(Tags)

#define MsgPtrDomain       MsgPtrF(Domain)
#define MsgFdDomain        MsgFdF(Domain)
#define MsgContentDomain   MsgContentF(Domain)

#define MSG_TYPE(type) ((DEREF([V]) : int) = type)

#define TAB_PRIVATE_MSG(_tags)             \
    && [MsgPtrTags     = _tags;            \
        MsgContentTags = _tags]          

#define SAME_DOMAIN(_domain)                    \
  && [MsgPtrDomain = _domain;                   \
      MsgContentDomain = _domain]               \
  
#define READ_MSG_T                                  \
  REF(&& [MsgPtrTags       = MsgFdTags;               \
          MsgContentTags   = MsgFdTags;               \
          MsgContentDomain = MsgFdDomain])

#define WRITE_MSG_T(soc)                                            \
  REF(|| [&& [MSG_TYPE(0); ? Set_emp([Tags(soc)])];                 \
          && [MSG_TYPE(2); TAB_PRIVATE_MSG(Tags(soc))];             \
          && [MSG_TYPE(2); ? Set_emp([Tags(soc)])];                 \
          && [MSG_TYPE(4)];                                         \
          && [MSG_TYPE(7); TAB_PRIVATE_MSG(Tags(soc))];             \
         && [MSG_TYPE(13); DOMAIN([soc]) = MsgContentDomain];    \
          (? Set_emp([Tags(MsgContent)]))]) 

typedef struct {
  mtypes FINAL type;
  int FINAL src_fd;
  char NULLTERMSTR * STRINGPTR START LOC(L) FINAL content;
} message;

#define msg_start START VALIDPTR ROOM_FOR(message)

void
check_ok_set_cookie(message FINAL * REF(Domain(Field(V,4) : int) = Domain(Field(V,8)))) OKEXTERN;

message INST(L,L) FINAL *
msg_start
REF(Tags(V) = Set_cup([Tags(fd); Tags(c)]))
REF(Tags(Field(V,4) : int) = Tags(fd))
REF(Tags(Field(V,8)) = Tags(c))
REF((Field(V,0): int) = type)
REF(Domain(Field(V,4) : int) = Domain(fd))
REF(Domain(Field(V,8)) = Domain(c))
REF((Field(V, 4) : int) = fd)
/* REF(THE_STRING([Field(V,8)]) = THE_STRING([c])) */
create_msg(mtypes type, int fd, char FINAL NULLTERMSTR * NNSTRINGPTR NNSTART LOC(L) c) OKEXTERN;

void write_message_soc(int soc,
                       message FINAL * WRITE_MSG_T(soc) m) OKEXTERN;

message *
msg_start
REF((DEREF([V + 4]) : int) = soc)
REF(Tags(MsgFd) = Set_sng([soc]))
READ_MSG_T
read_message_soc(int soc) OKEXTERN;

#endif
