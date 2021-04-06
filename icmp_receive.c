//Grzegorz Bielecki 288388

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "icmp_receive.h"

int receive_with_ttl(int _sockfd, int _ttl, struct timeval* _tv, uint8_t _buffer[],
                 char sender_ip_str[])
{
	if (_sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

	for (;;) {
		
		struct sockaddr_in 	sender;	
		socklen_t 			sender_len = sizeof(sender);

		fd_set descriptors;
		FD_ZERO (&descriptors);
		FD_SET (_sockfd, &descriptors);

		int ready = select (_sockfd+1, &descriptors, NULL, NULL, _tv);
		if(ready < 0){
			fprintf(stderr, "select error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (ready == 0){
			return 3; // error
		}

		ssize_t packet_len = recvfrom (_sockfd, _buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len);
		if (packet_len < 0) {
			fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
			return EXIT_FAILURE;
		}

		inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str[0]));

		struct ip* 			ip_header = (struct ip*) _buffer;
		ssize_t				ip_header_len = 4 * ip_header->ip_hl;

        struct icmp* icmp_header = (struct icmp*)(_buffer + ip_header_len);

        if(icmp_header->icmp_type == ICMP_TIMXCEED)
        {
            uint8_t* beg_of_orig_datagram = _buffer + ip_header_len + sizeof(icmp_header);
            ip_header     = (struct ip*)(beg_of_orig_datagram);
            ip_header_len = 4 * ip_header->ip_hl;
            icmp_header = (struct icmp*)(beg_of_orig_datagram + ip_header_len);
        }
        if(icmp_header->icmp_hun.ih_idseq.icd_id != htons(getpid()) || icmp_header->icmp_hun.ih_idseq.icd_seq != htons(_ttl))
        {
            continue;
        }

        if(icmp_header->icmp_type == ICMP_ECHOREPLY)
        {
            return 0; // gdy doszlismy do podanego adresu hosta
        }
        return 2; // dostalismy odpowiedz
    }

    return 1; // nie dotarlismy do podanego adresu hosta
}
