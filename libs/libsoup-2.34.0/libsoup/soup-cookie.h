/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * Copyright 2007, 2008 Red Hat, Inc.
 */

#ifndef SOUP_COOKIE_H
#define SOUP_COOKIE_H 1

#include <libsoup/soup-types.h>
#include "csolve.h"

G_BEGIN_DECLS

struct _SoupCookie {
	char     NULLTERMSTR * name;
	char     NULLTERMSTR * value;
	char     FINAL NULLTERMSTR * domain;
	char     NULLTERMSTR * LOC(COOKIE_PATH) path;
	SoupDate *expires;
	gboolean  secure;
	gboolean  http_only;
};

#define StructDom(fld) DOMAIN([DEREF([V + fld])]) = DOMAIN([V])

GType soup_cookie_get_type (void);
#define SOUP_TYPE_COOKIE (soup_cookie_get_type())

#define SOUP_COOKIE_MAX_AGE_ONE_HOUR (60 * 60)
#define SOUP_COOKIE_MAX_AGE_ONE_DAY  (SOUP_COOKIE_MAX_AGE_ONE_HOUR * 24)
#define SOUP_COOKIE_MAX_AGE_ONE_WEEK (SOUP_COOKIE_MAX_AGE_ONE_DAY * 7)
#define SOUP_COOKIE_MAX_AGE_ONE_YEAR (SOUP_COOKIE_MAX_AGE_ONE_DAY * 365.2422)

SoupCookie *soup_cookie_new                     (const char  *name,
						 const char  *value,
						 const char  *domain,
						 const char  *path,
						 int          max_age);
SoupCookie *soup_kcookie_parse                   (const char  *header);

SoupCookie FINAL *
/* Refinemnts */
  NNSTART NNVALIDPTR NNROOM_FOR(SoupCookie)
  NNREF(&&[StructDom(0);StructDom(4);StructDom(8);StructDom(12)])
  NNREF(DOMAIN([V]) = DOMAIN([origin]))
  NNREF(TAGSET([V]) = Set_cup([TAGSET([header]);TAGSET([origin])]))
/* End Refinements */
soup_cookie_parse(const char  FINAL NULLTERMSTR MEMPRED(DOMAIN) * STRINGPTR header,
		  SoupURI     FINAL *origin)
  OKEXTERN;
SoupCookie FINAL * REF(DOMAIN([V]) = DOMAIN([cookie]))
                   REF(TAGSET([V]) = TAGSET([cookie])) 
                   REF(&&[StructDom(0);StructDom(4);StructDom(8);StructDom(12)])
                   START VALIDPTR ROOM_FOR(SoupCookie)
                   /* LOC(!C) */
	     soup_cookie_copy                    (SoupCookie  FINAL *
						  START VALIDPTR ROOM_FOR(SoupCookie) 
						  REF(&&[StructDom(0);StructDom(4);StructDom(8);StructDom(12)])
						  cookie) OKEXTERN;

const char *soup_cookie_get_name                (SoupCookie  *cookie);
void        soup_cookie_set_name                (SoupCookie  *cookie,
						 const char  *name);
const char *soup_cookie_get_value               (SoupCookie  *cookie);
void        soup_cookie_set_value               (SoupCookie  *cookie,
						 const char  *value);
const char *soup_cookie_get_domain              (SoupCookie  *cookie);
void        soup_cookie_set_domain              (SoupCookie  *cookie,
						 const char  *domain);
const char NULLTERMSTR * LOC(COOKIE_PATH)
soup_cookie_get_path    (SoupCookie  FINAL INST(COOKIE_PATH,COOKIE_PATH) *cookie) OKEXTERN;

void        soup_cookie_set_path                (SoupCookie  *cookie,
						 const char  *path);
void        soup_cookie_set_max_age             (SoupCookie  *cookie,
						 int          max_age);
SoupDate   *soup_cookie_get_expires             (SoupCookie  *cookie);
void        soup_cookie_set_expires             (SoupCookie  *cookie,
						 SoupDate    *expires);
gboolean    soup_cookie_get_secure              (SoupCookie  *cookie);
void        soup_cookie_set_secure              (SoupCookie  *cookie,
						 gboolean     secure);
gboolean    soup_cookie_get_http_only           (SoupCookie  *cookie);
void        soup_cookie_set_http_only           (SoupCookie  *cookie,
						 gboolean     http_only);

char       *soup_cookie_to_set_cookie_header    (SoupCookie  *cookie);


char       NULLTERMSTR MEMPRED(DOMAIN) *START STRINGPTR REF(DOMAIN([V]) = DOMAIN([cookie]))
soup_cookie_to_cookie_header        (SoupCookie  FINAL *cookie) OKEXTERN;

gboolean    soup_cookie_applies_to_uri          (SoupCookie  *cookie,
						 SoupURI     *uri);
gboolean    soup_cookie_equal                   (SoupCookie  *cookie1,
						 SoupCookie  *cookie2);

void        soup_cookie_free                    (SoupCookie  *cookie);

GSList     *soup_cookies_from_response          (SoupMessage *msg);
GSList     *soup_cookies_from_request           (SoupMessage *msg);

void        soup_cookies_to_response            (GSList      *cookies,
						 SoupMessage *msg);
void        soup_cookies_to_request             (GSList      *cookies,
						 SoupMessage *msg);

void        soup_cookies_free                   (GSList      *cookies);

char       *soup_cookies_to_cookie_header       (GSList      *cookies);

gboolean
REF(V != 0 => ? COOKIE_DOMAIN_GET([THE_STRING([host]);DOMAIN([cookie])]))
soup_cookie_domain_matches          (SoupCookie  FINAL *cookie,
				     const char  MEMPRED(DOMAIN) NULLTERMSTR FINAL *host) OKEXTERN;
gboolean
REF(V != 0 => ? COOKIE_DOMAIN_GET([THE_STRING([host]);DOMAIN([domain])]))
soup_domain_matches          (const char  MEMPRED(DOMAIN) NULLTERMSTR FINAL *STRINGPTR domain,
	                      const char  MEMPRED(DOMAIN) NULLTERMSTR FINAL *STRINGPTR host) OKEXTERN;

G_END_DECLS

#endif /* SOUP_COOKIE_H */
