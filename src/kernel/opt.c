#include <unistd.h>
#include "opt.h"
#include <stdio.h>



struct opt_args opt;


void parse_options(int argc, char *argv[]) {
    int c;
    
    opt.use_length_encoding = 1;
    opt.use_shm = 1;
    opt.use_kcookies = 0;
    
    while ( (c = getopt(argc, argv, "lmk")) != -1 ) {
        switch (c) {
            case 'l':
                opt.use_length_encoding = 1;
                break;
            case 'm':
                opt.use_shm = 1;
                break;
            case 'k':
                opt.use_kcookies = 1;
                break;
        }
    }
    
    //printf("opt options parsed:\n");
    //printf("-l: %d\n-m: %d\n-k: %d\n", opt.use_length_encoding, opt.use_shm, opt.use_kcookies);
}
