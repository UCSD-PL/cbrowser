#include "opt.h"
#include "str.h"
#include "msg.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


// from str.h for urlparse
extern char scheme[SCHEME_SIZE];
extern char netloc[NETLOC_SIZE];

// from opt.h for parse_options
extern struct opt_args opt;


struct whitelist_entry {
    char origin[16];
    int num_approved;
    char approved[4][24];
};

#define WHITELIST_LEN 7
struct whitelist_entry whitelist[WHITELIST_LEN] =
    { {"facebook.com",  1, {"fbcdn.net"}},
      {"youtube.com",   1, {"ytimg.com"}},
      {"yahoo.com",     1, {"yimg.com"}},
      {"wikipedia.org", 1, {"wikimedia.org"}},
      {"twitter.com",   1, {"twimg.com"}},
      {"blogger.com",   4, {"google.com", "blogspot.com", "gstatic.com", "googleusercontent.com"}},
      {"ebay.com",      3, {"ebaystatic.com", "ebayimg.com", "ebayrtm.com"}} };


// info for this tab
struct tab_info {
    char *tab_origin; //a command line arg -> don't need to allocate space
    int soc; //socket
};

struct tab_info tab;


///////////////
// Tab class //
///////////////

/////////////////
// is_sub_origin
//  return:
//      -> 1 if uri is a subdomain of origin
//      -> 0 otherwise
int is_sub_origin(char *origin, char *uri) {
    urlparse(uri);
    return endswith(netloc, origin);
}

/////////////////////
// find_in_whitelist
//  return:
//      -> the index in the whitelist of origin
//      -> -1 otherwise
int find_in_whitelist(char *origin) {
    int i;
    for (i=0; i<WHITELIST_LEN; i++) {
        if (strcmp(origin, whitelist[i].origin) == 0) {
            return i;
        }
    }
    return -1;
}

////////////////////
// is_sub_whitelist
//  return:
//      -> 1 if uri is_sub_origin of one of origin's whitelisted origins
//      -> 0 otherwise
//  Note: returns 0 even if uri is_sub_origin of origin
//      -> use is_sub_origin to determine this
int is_sub_whitelist(char *origin, char *uri) {
    int i, w;
    urlparse(uri);
    if ( (w = find_in_whitelist(origin)) >= 0 ) {
        for (i=0; i<whitelist[w].num_approved; i++) {
            if (is_sub_origin(whitelist[w].approved[i], uri)) {
                return 1;
            }
        }
    }
    return 0;
}




void write_message(message *m) {
    write_message_soc(tab.soc, m);
}

void read_message(message *m) {
    read_message_soc(tab.soc, m);
}



int main(int argc, char *argv[]) {
    if (argc > 4)
        parse_options(argc-3, &(argv[3]));
    
    tab.soc = atoi(argv[1]);
    tab.tab_origin = argv[2];
    char *url = argv[3];
    
//     printf("tab options parsed:\n");
//     printf("-l: %d\n-m: %d\n-k: %d\n", opt.use_length_encoding, opt.use_shm, opt.use_kcookies);
//     
//     printf("soc: %d\n", tab.soc);
//     printf("origin: %s\n", tab.tab_origin);
//     printf("url: %s\n", url);
    
    //phony navigate test
    
    message nav;
    nav.type = NAVIGATE;
    if (strcmp(url, "None") != 0)
        strncpy(nav.uri, url, MAX_URI_LEN);
    else
        strncpy(nav.uri, "www.google.com", MAX_URI_LEN);
    write_message(&nav);
    
    //printf("writing message to navigate to: %s\n", url);
    
    //while (1)
    //    sleep(5);
    
    printf("tab is exiting!\n");
}