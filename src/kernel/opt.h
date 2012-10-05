#ifndef OPT_H
#define OPT_H

struct opt_args {
    int use_length_encoding;
    int use_shm;
    int use_kcookies;
};

extern struct opt_args opt;


void parse_options(int argc, char *argv[]);


#endif