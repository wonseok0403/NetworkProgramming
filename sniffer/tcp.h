#ifndef TCP_H
#define TCP_H
#include<stdint.h>

typedef  uint32_t tcp_seq;

#ifndef __linux__
    #pragma pack(push, 1)
#endif
struct tcphdr{
    uint16_t th_sport;
    uint16_t th_dport;
    tcp_seq th_seq;
    tcp_seq th_ack;
    uint8_t th_x2:4;
    uint8_t th_off:4;
    uint8_t th_flags;

    #define TH_FIN  0x01
    #define TH_SYN  0x02
    #define TH_RST  0x04
    #define TH_PUSH 0x08
    #define TH_ACK 0x10
    #define TH_URG 0x20

    uint16_t th_win;
    uint16_t th_sum;
    uint16_t th_urp;
}
#ifndef __linux__  
    ;
    #pragma pack(pop)
#else
__attribute__((__packed__));
#endif
void dump_tcp_header(void *iphdr);
#endif