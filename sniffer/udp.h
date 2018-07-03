#ifndef _UDP_H
#define _UDP_H
#include<stdint.h>

#ifndef __linux__
    #pragma pack(push, 1)
    #endif
struct udphdr{
    uint16_t uh_sport;
    uint16_t uh_dport;
    uint16_t uh_ulen;
    uint16_t uh_sum;
}
#ifndef __linux__ 
    ;
    #pragma pack(pop)
#else
__attribute__((__packed__));
#endif

void dump_udp_header(struct udphdr *udphdr);

#endif