//
// Created by Alan on 12/07/2024.
//

#ifndef DISK_H
#define DISK_H

#include "types.h"

uint8_t read_status(void);
void select_device(uint8_t device);
void init_disk(void);

#endif //DISK_H
