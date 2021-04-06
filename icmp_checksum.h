#pragma once

#include <sys/types.h>

u_int16_t compute_icmp_checksum(const void* buff, int length);