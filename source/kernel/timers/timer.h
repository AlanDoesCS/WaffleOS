//PIT
#pragma once

#include <stdint.h>
#include <stddef.h>

void init_pit();
void sleep_millis(uint32_t duration);
void sleep(uint32_t duration);
uint32_t get_seconds(void);
void get_systime_string(char* buffer);
void get_systime_millis(uint32_t* high, uint32_t* low);
void print_systime(void);
