//Grzegorz Bielecki 288388

#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "icmp_send.h"
#include "icmp_receive.h"

struct sockaddr_in get_recipient(char* _receiver_ip){
    struct sockaddr_in recipient;
    bzero (&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    int out = inet_pton(AF_INET, _receiver_ip, &recipient.sin_addr);
    if (out == 1){
        return recipient;
    }
    if(out == 0){
        fprintf(stderr, "IP does not contain a \
                         character string representing a valid network address in the \
                         ipv4 address family\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Error parsing ip, pls check input and try again\n");
    exit(EXIT_FAILURE);
}

int send_pings(int _sockfd, struct sockaddr_in _recipient, int _ttl, int _number_of_pings){
    int out, iter=0;
    for (int i = _number_of_pings; i > 0; i--){
        out = send_ping(_sockfd, _recipient, _ttl, (_ttl - 1)*_number_of_pings + iter);
        if (out == EXIT_FAILURE && iter<_number_of_pings*2){
            i++;
        }
        iter++;
    }
    return 0;
}

void print_ips(char ip_addresses[][20], int count){
    int a[count];
    memset(a, 0, sizeof(a[0])*count);

    for(int j = 0; j < count; j++){
        if(a[j] == 0) {
            printf("%s ", ip_addresses[j]);
            for(int i = j; i < count; i++){
                if(!strcmp(ip_addresses[j], ip_addresses[i])){
                    a[i] = -1;
                }
            }
        }
    }
}

int receive_pings(int _sockfd, int _ttl, int _number_of_pings, long avr_time){
    u_int8_t buffer[IP_MAXPACKET];
    struct timeval tv; tv.tv_sec = 1; tv.tv_usec = 0;
    char sender_ip_str[_number_of_pings][20];
    avr_time = 0;
    int is_avr_time_avaible = 0;
    memset(sender_ip_str, 0, sizeof(char) * _number_of_pings * 20);
    int get_atleast_one = 0, get_to_dest_host = 0;
    for(int i = 0; i < _number_of_pings; i++){
        int out = receive_with_ttl(_sockfd, _ttl, &tv, _number_of_pings, buffer, sender_ip_str[i]);
        switch (out) {
        case 0:
            //doszlismy do podanego hosta
            get_to_dest_host = 1;
            get_atleast_one += 1;
            avr_time += tv.tv_usec || tv.tv_sec ? (1000000 - tv.tv_usec) / 1000 : 0;
            break;
        case 2:
            //dostalismy odpowiedz od routera na drodze do hosta
            avr_time += tv.tv_usec || tv.tv_sec ? (1000000 - tv.tv_usec) / 1000 : 0;
            get_atleast_one += 1;
            break;
        default:
            //jesli nie ustawiamy czas tego jednego pakietu na -1
            is_avr_time_avaible = -1;
            break;
        }
    }
    avr_time = avr_time/_number_of_pings;

    if(get_atleast_one){
        print_ips(sender_ip_str, _number_of_pings);

        if(is_avr_time_avaible == -1) {
            printf("???\n");
        }
        else {
            printf("%ldms\n", avr_time);
        }
    } else {
        printf("*\n");
    }

    if(get_to_dest_host) {
        return 0;
    }
    return 1;
}

int traceroute(char* _receiver_ip){
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    struct sockaddr_in recipient;
    int send_out, recv_out;
    long time=0;

    recipient = get_recipient(_receiver_ip);
    for (int ttl=1; ttl <= 30; ttl++){
        send_out = send_pings(sockfd, recipient, ttl, 3);
        if (send_out!=0){
            fprintf(stderr, "Error when sending packet\n");
            return EXIT_FAILURE;
        }
        recv_out = receive_pings(sockfd, ttl, 3, time);
        if (recv_out==0){
            return 0;//dotarliśmy do hosta
        }
    }
    return EXIT_FAILURE;
}

int main(int argc, char** argv){
    if (argc != 2){
        fprintf(stderr, "Get %d arguments, only one is requied\n", argc);
        exit(EXIT_FAILURE);
    }

    return traceroute(argv[1]);
}