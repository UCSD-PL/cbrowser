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

#define SINGLETON(_type)                                                      \
  ((DEREF([V]) : int) = _type) =>                                             \
    && [TAGSET([V]) = Set_sng([(DEREF([V + 4]) : int)]);                      \
        TAGSET([(DEREF([V + 4]) : int)]) = Set_sng([(DEREF([V + 4]) : int)]); \
        TAGSET([DEREF([V + 8])]) = Set_sng([(DEREF([V + 4]) : int)])]

#define REQ_URI_FOLLOW_MSG_TYPE SINGLETON(2)
#define RES_URI_MSG_TYPE SINGLETON(7)

#define MSG_POLICY                                                      \
  REF(&& [TAGSET([V])              = TAGSET([(DEREF([V + 4]) : int)]);  \
          TAGSET([DEREF([V + 8])]) = TAGSET([(DEREF([V + 4]) : int)])])

typedef struct {
  mtypes FINAL type;
  int FINAL src_fd;
  char NULLTERMSTR * NNSTRINGPTR NNSTART LOC(L) FINAL content;
} message;

#define msg_start(_l) START INST(L,_l) VALIDPTR ROOM_FOR(message)

#define writable_msg(soc)                                               \
  REF(|| [? Set_emp([Tags(soc)]);                                       \
          && [? Set_emp([Tags(V)]); ? Set_emp([FieldTags(V,8)])];       \
          && [Tags(V) = Tags(soc); FieldTags(V,8) = Tags(soc)]])

/* The following should all take the union of content, fd, and type's tags.
   Or intersection? */
message*
msg_start(S)
TagsEq(V, content)
TagsEq((Field(V,4) : int), content)
TagsEq(Field(V,8), content)
REF((DEREF([V]) : int) = type)
REF((DEREF([V + 4]) : int) = fd)
create_msg(mtypes type, int fd, char NULLTERMSTR * NNSTRINGPTR NNSTART LOC(S) content) OKEXTERN;

void write_message_soc(int soc,
                       message FINAL * writable_msg(soc) m) OKEXTERN;

message*
msg_start(L)
REF((DEREF([V + 4]) : int) = soc)
MSG_POLICY
read_message_soc(int soc) OKEXTERN;

#endif
