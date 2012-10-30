#ifndef KERNEL_H
#define KERNEL_H

#include "msg.h"
#define kstr char NULLTERMSTR * NNSTRINGPTR NNSTART

void klog(char *str);
char *get_uri_follow(char *uri);

// Piped Process
void init_piped_process(const char *procfile,
                        char  *args[],
                        pid_t *proc,
                        int   *soc);
void write_message(message *m);
message *read_message(int fd);


// UI Process
void init_ui_process();
void kill_ui(int signal);

// Tab Process
void add_kargv(char *args[], int pos);
void init_tab_process(int tab_idx, char *init_url);
void get_trusted_origin_suffix(int tab_idx);
void render(int tab_idx);
void kill_tab(int tab_idx, int signal);

int  current_tab();
int  num_tabs();
int  tab_fd(int tab_idx);
char *tab_title(int tab_idx);

// Kernel
void add_tab();
void switch_tab(int tab_idx);
void print_tab_title(int tab_idx);
void print_text_display();
int get_tab_idx(char ascii);
void process_input_char(char c);

#endif
