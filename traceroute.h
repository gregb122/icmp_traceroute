#pragma once

struct sockaddr_in get_recipient(char* _receiver_ip);
int send_pings(int _sockfd, struct sockaddr_in _recipient, int _ttl, int _number_of_pings);
long avrg(long* numbers, int count);
void print_ips(long* numbers, char* ip_addresses, int count);
int receive_pings(int _sockfd, int _ttl, int _number_of_pings);
int traceroute(char* _receiver_ip);