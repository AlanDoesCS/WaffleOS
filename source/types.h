//
// Created by Alan on 12/07/2024.
//

#ifndef TYPES_H
#define TYPES_H

#define TRUE 1
#define FALSE 0

#define NULL ((void*)0)
typedef typeof(sizeof(0)) size_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef uint32_t uintptr_t;

#endif //TYPES_H
