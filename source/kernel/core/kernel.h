#pragma once

#include "idt.h"
#include "../drivers/keyboard.h"
#include "../drivers/mouse.h"
#include "../libs/string.h"
#include "../drivers/fat.h"
#include "../drivers/disk.h"
#include "../timers/timer.h"
#include "memory.h"

void execute_command(char* command);
void print_splash(void);
void cowsay(char* message);
void delay(int duration);
