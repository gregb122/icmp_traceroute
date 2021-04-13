//Grzegorz Bielecki 288388

#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


#include "icmp_checksum.h"
#include "icmp_send.h"

struct icmp create_header(pid_t _pid, int _seq){
    struct icmp header;
    header.icmp_type = ICMP_ECHO;
    header.icmp_code = 0;
    header.icmp_hun.ih_idseq.icd_id = _pid;
    header.icmp_hun.ih_idseq.icd_seq = _seq;
    header.icmp_cksum = 0;
    header.icmp_cksum = compute_icmp_checksum (
    (u_int16_t*)&header, sizeof(header));
    return header;
}

void change_ttl(int ttl, int sockfd){
    setsockopt (sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
}

int8_t send_with_ttl(int _sockfd, struct icmp _header, struct sockaddr_in _recipient, int _ttl){
    change_ttl(_ttl, _sockfd);
    ssize_t bytes_sent = sendto (
    _sockfd,
    &_header,
    sizeof(_header),
    0,
    (struct sockaddr*)&_recipient,
    sizeof(_recipient)
    );
    if (bytes_sent >= 0){
        return EXIT_SUCCESS;
    }
    fprintf(stderr, "Error sending a ping\n");
    return EXIT_FAILURE;
}

int8_t send_ping(int _sockfd, struct sockaddr_in _recipient,  int _ttl, int _seq_number){
    pid_t pid = getpid();
    struct icmp ping_header = create_header(pid, _seq_number);
    return send_with_ttl(_sockfd, ping_header, _recipient, _ttl);
}