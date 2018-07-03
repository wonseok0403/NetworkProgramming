#ifndef ETHERNET_H
#define ETHERNET_H

#include<stdint.h>

#define ETHERTYPE_IP  0x0800
#define ETHERTYPE_ARP 0x0806

#ifndef __linux__
    #pragma pack(push,1)
#endif
struct ether_header{
    uint8_t ether_dhost[6];
    uint8_t ether_shost[6];
    uint16_t ether_type;
}
#ifndef __linux__
    ;
    #pragma pack(pop)
#else
__attribute__((__packed__));
#endif

void dump_ether_header(struct ether_header *ether_header);
#endif