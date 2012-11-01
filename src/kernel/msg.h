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

//#define MSG_POLICY REF(REQ_URI_FOLLOW_MSG_TYPE) REF(RES_URI_MSG_TYPE)

#define MSG_POLICY                                                      \
  REF(&& [TAGSET([V])              = TAGSET([(DEREF([V + 4]) : int)]);  \
          TAGSET([DEREF([V + 8])]) = TAGSET([(DEREF([V + 4]) : int)])])

typedef struct {
  mtypes FINAL type;
  int FINAL src_fd;
  char NULLTERMSTR * NNSTRINGPTR NNSTART LOC(L) FINAL content;
} message;

#define msg_start(_l) START INST(L,_l) VALIDPTR ROOM_FOR(message)

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
                       message FINAL * REF(|| [(? Set_emp([TAGS(soc)]));
                                               TAGS(Field(V, 8)) = TAGS(soc)]) m) OKEXTERN;

message*
msg_start(L)
REF((DEREF([V + 4]) : int) = soc)
MSG_POLICY
/* msg_start(L) //TagsEq(V, soc) TagsEq(Field(V, 4), soc) */
/* REF(TAGSET([V]) = Set_sng([soc]))//Set_cup([TAGSET([soc]); Set_sng([soc])])) */
/* REF(TAGSET([Field(V,4)]) = Set_sng([soc]))//Set_cup([TAGSET([soc]); Set_sng([soc])])) */
/* REF(TAGSET([Field(V,8)]) = Set_sng([soc]))//Set_cup([TAGSET([soc]); Set_sng([soc])])) */
read_message_soc(int soc) OKEXTERN;

void recv_exact(int soc, int size, char *buf);
void read_lstr(int soc, char **dst); // message *m);
void read_message_len(int soc, message *m) OKEXTERN;
void write_message_len(int soc, message *m) OKEXTERN;
char *payload(message *m, char *buf);


#endif
