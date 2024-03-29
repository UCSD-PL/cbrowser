#ifndef STR_H
#define STR_H

#include <string.h>



// If this is changed, the following must also be updated:
//  -> kernel.c get_trusted_origin_suffix(...) scanf max size
#define MAX_ORIGIN_LEN 64

#define MAX_URI_LEN 2048



// endswith
//  Determine is the string str ends with suffix
//  return 1 if true, 0 if false
int endswith(char *str, char *suffix);




// used by urlparse
// If these constants get changed, urlparse must be similarly modified
#define SCHEME_SIZE 16
#define NETLOC_SIZE 1024
extern char scheme[SCHEME_SIZE];
extern char netloc[NETLOC_SIZE];

// urlparse
//  Puts the scheme and netloc of the url in global scheme and netloc vars
void urlparse(char *url);


#endif