// Source: https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part8/src/kernel/util/binary.h
#pragma once

#define FLAG_SET(x, flag) x |= (flag)
#define FLAG_UNSET(x, flag) x &= ~(flag)