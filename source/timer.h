//
// Created by Alan on 14/07/2024.
//

#ifndef TIMER_H
#define TIMER_H

#include "types.h"

void init_pit();
void sleep_millis(uint32_t duration);
void sleep(uint32_t duration);
uint32_t get_seconds(void);
void get_systime_string(char* buffer);
void print_systime(void);

#endif //TIMER_H
