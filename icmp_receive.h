#pragma once

#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>

int receive_with_ttl(int _sockfd, int _ttl, struct timeval* _tv, int _number_ofpings, uint8_t _buffer[],
                 char _sender_ip_str[]);