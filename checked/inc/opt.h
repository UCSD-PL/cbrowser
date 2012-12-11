#ifndef OPT_H
#define OPT_H

#include <csolve.h>

struct opt_args {
    int use_length_encoding;
    int use_shm;
    int use_kcookies;
};

extern struct opt_args opt;


void parse_options(int argc, char NULLTERMSTR * NNSTRINGPTR SIZE_GE(1) * START ARRAY SIZE_GE(4*argc) argv)
  OKEXTERN ;


#endif
