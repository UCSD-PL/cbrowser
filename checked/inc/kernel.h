#ifndef KERNEL_H
#define KERNEL_H

#include "constants.h"
#include "msg.h"
#include "tab.h"

#define kstr char NULLTERMSTR * NNSTRINGPTR NNSTART


// Kernel
int  current_tab() OKEXTERN;
int  num_tabs() OKEXTERN;
void process_input_char(KERNEL_TABS tabs, char c) OKEXTERN;
void process_message(KERNEL_TABS tabs,
                     int tab_idx,
                     message * START VALIDPTR ROOM_FOR(message) READ_MSG_T m) OKEXTERN;

#endif
