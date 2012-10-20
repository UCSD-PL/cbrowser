#ifndef MSG_H
#define MSG_H

#include "str.h"
#include <stdio.h>
#include <csolve.h>


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
    mtypes type;
    char NULLTERMSTR * NNSTRINGPTR NNSTART content;
} message;


void create_navigate(message *m, char *s);
//void create_navigate(int soc, message *m);
void create_req_uri_follow(message *m, char *s);
void create_res_uri(message *m, char NULLTERMSTR * STRINGPTR content) OKEXTERN;
void create_render(message *m);
void create_display_shm(message *m, int shmid, int size);
void create_k2g_display_shm(message *m, int shmid, int size);
void create_get_cookie(message *m, void *cookie); 

void write_message_soc(int soc, message *m) OKEXTERN;
void read_message_soc(int soc, message *m) OKEXTERN;
void recv_exact(int soc, int size, char *buf);
void read_lstr(int soc, char **dst); // message *m);
void read_message_len(int soc, message *m) OKEXTERN;
void write_message_len(int soc, message *m) OKEXTERN;
char * payload(message *m, char *buf);


#endif
