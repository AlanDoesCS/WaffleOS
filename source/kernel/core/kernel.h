//
// Created by Alan on 12/07/2024.
//

# pragma once

#include "idt.h"
#include "../drivers/keyboard.h"
#include "../libs/string.h"
#include "../filesystems/filesystem.h"
#include "../drivers/disk.h"
#include "../timers/timer.h"
#include "memory.h"

void execute_command(char* command);
void print_splash(void);
void cowsay(char* message);
void delay(int duration);
