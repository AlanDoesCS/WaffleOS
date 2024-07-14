//
// Created by Alan on 14/07/2024.
//

#ifndef TIMER_H
#define TIMER_H

#include "types.h"

void init_pit();
void sleep_millis(uint32_t duration);
void sleep(uint32_t duration);

#endif //TIMER_H
