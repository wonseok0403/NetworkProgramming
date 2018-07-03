#include<stdio.h>
#include<string.h>

#ifndef __linux__
    #include<winsock2.h>
#else   
    #include<netinet/in.h>
    #include<arpa/inet.h>
#endif
#include"../ethernet.h"
#include"../arp.h"
#include"../ip.h"
