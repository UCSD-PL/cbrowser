#include "str.h"
#include <stdio.h>


// for use with urlparse
char scheme[SCHEME_SIZE];
char netloc[NETLOC_SIZE];



int endswith(char *str, char *suffix) {
    int lenstr = strlen(str);
    int lensuffix = strlen(suffix);
    if ( lenstr >= lensuffix && (strcmp(&str[lenstr-lensuffix], suffix) == 0) ) {
        return 1;
    } else {
        return 0;
    }
}



void urlparse(char *url) {
    if ( strchr(url, ':') ) {
        sscanf(url, "%15[^:/]%*[:/]%1023[^/]", scheme, netloc);
    } else if ( url[0] == '/' ) {
        sscanf(url, "%*[/]%1023[^/]", netloc);
        strncpy(scheme, "", 1);
    } else {
        sscanf(url, "%1023[^/]", netloc);
        strncpy(scheme, "", 1);
    }
}