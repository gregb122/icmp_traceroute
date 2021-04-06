#Grzegorz Bielecki 288388

CFLAGS = -std=gnu17 -Wall -Wextra

traceroute: $(obj)
	gcc $(CFLAGS) -c -o traceroute.o traceroute.c
	gcc $(CFLAGS) -c -o icmp_checksum.o icmp_checksum.c
	gcc $(CFLAGS) -c -o icmp_receive.o icmp_receive.c
	gcc $(CFLAGS) -c -o icmp_send.o icmp_send.c
	gcc $(CFLAGS) -o traceroute icmp_checksum.o icmp_receive.o icmp_send.o traceroute.o

.PHONY: clean distclean
clean:
	rm -f *.o

distclean:
	rm  -f traceroute