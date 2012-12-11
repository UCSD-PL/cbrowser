#ifndef KERNEL_H
#define KERNEL_H

#include "constants.h"
#include "msg.h"
#include "tab.h"

#define kstr char CSOLVE_DOMAIN_STR NULLTERMSTR * NNSTRINGPTR NNSTART

// Kernel
int  current_tab();
int  num_tabs();
void process_input_char(KERNEL_TABS tabs, char c);
void process_message(KERNEL_TABS tabs,
                     int tab_idx,
                     message * START VALIDPTR ROOM_FOR(message) READ_MSG_T m);

#endif
