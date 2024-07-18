//
// Created by Alan on 18/07/2024.
//

#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

void* memset(void* dst0, int value, size_t length);
void* memcpy(void* dst0, const void* src0, size_t length);

#endif //MEMORY_H
