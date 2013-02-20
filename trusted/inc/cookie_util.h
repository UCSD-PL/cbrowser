#ifndef BROWSER_COOKIE_UTIL
#define BROWSER_COOKIE_UTIL

#include <csolve.h>
#include <stdlib.h>
#include "constants.h"
#include "types.h"
#include <libsoup/soup-types.h>
#ifdef CIL
#define SoupDate int *
/* #define SoupURI  char * */
#endif
#include <libsoup/soup-uri.h>
#include <libsoup/soup-cookie.h>

typedef struct {
  char Immutable FINAL LOC(CD) c_domain;
  SoupCookie FINAL * FINAL REF(DOMAIN([V]) = THE_STRING([c_domain])) REF(&&[StructDom(0);StructDom(4);StructDom(8);StructDom(12)]) ROOM_FOR(SoupCookie) START VALIDPTR c_cookie;
} cookie;

void
free_cookie(cookie *c) OKEXTERN;

char NImmutable NNSTART
  NNREF((VVADDR + 8) < BLOCK_END([VVADDR]))
  NNREF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])]))
  NNREF(DOMAIN([V]) = THE_STRING([V])) FINAL
* ARRAY START VALIDPTR SIZE_GE(4) REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([DEREF([V])])]))
gettable_domains(char FINAL Immutable REF(DOMAIN([V]) = THE_STRING([V])) d) OKEXTERN;
#endif
