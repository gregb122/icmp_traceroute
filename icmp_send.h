#pragma once

#include <arpa/inet.h>

struct icmp create_header(pid_t _pid, int _seq);
void change_ttl(int ttl, int sockfd);
int8_t send_with_ttl(int _sockfd, struct icmp _header, struct sockaddr_in _recipient, int _ttl);
int8_t send_ping(int _sockfd, struct sockaddr_in _recipient,  int _ttl, int _seq_number);