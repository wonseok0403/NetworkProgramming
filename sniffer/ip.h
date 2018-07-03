#ifndef IP_H
#define IP_H
#include<stdint.h>

#ifndef __linux__
    #include<winsock2.h>
    #pragma pack(push, 1)
#else
    #include <netinet/in.h>
#endif
struct ip{
    uint8_t ip_hl:4;
    uint8_t ip_v:4;
    uint8_t ip_tos;
    uint16_t ip_len;
    uint16_t ip_id;
    uint16_t ip_off;
    #define IP_RF 0x8000
    #define IP_DF 0x4000
    #define IP_MF 0x2000
    #define IP_OFFMASK 0x1 ff f
    uint8_t ip_ttl;
    uint8_t ip_p;
    uint16_t ip_sum;
    struct in_addr ip_src, ip_dst;
}
#ifndef __linux__
;
#pragma pack(pop)
#else
__attribute__((__packed__));
#endif
void dump_ip_header(struct ip *ip);

#endif