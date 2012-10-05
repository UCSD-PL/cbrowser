#ifndef KERNEL_H
#define KERNEL_H

#include "msg.h"


void klog(char *str);
char *get_uri_follow(char *uri);


// Piped Process
void init_piped_process(int tab_idx, const char *procfile, char * args[]);
void write_message(int tab_idx, message *m);
void read_message(int tab_idx, message *m);


// UI Process
void init_ui_process();


// Tab Process
void add_kargv(char *args[], int pos);
void init_tab_process(int tab_idx, char *init_url);
void get_trusted_origin_suffix(int tab_idx);
void render(int tab_idx);


// Kernel
void add_tab();
void switch_tab(int tab_idx);
void print_tab_title(int tab_idx);
void print_text_display();
int get_tab_idx(char ascii);
void process_input_char(char c);
void kexit();


void make_command_args_global(int argc, char *argv[]);


#endif
