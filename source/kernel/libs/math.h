#pragma once
#include <float.h>
#include <stdint.h>

#define min(a,b)    ((a) < (b) ? (a) : (b))
#define max(a,b)    ((a) > (b) ? (a) : (b))

void init_fpu();
static inline void set_fpu_control_word(uint16_t control_word);
static inline uint16_t get_fpu_control_word();
