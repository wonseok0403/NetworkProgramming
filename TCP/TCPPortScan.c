#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

enum
{
    ARGV_CMD,
    ARGV_IP,
    ARGV_START_PORT,
    ARGV_END_PORT
};

int connect_timeout(int sock, struct sockaddr *addr, unsigned long len, int sec);

int main(int argc, char **argv)
{
    int sock;
    struct sockaddr_in addr;
    int port, start_port, end_port;

    if (argc != 4)
    {
        printf("usage : %s [ip][start port][end port]\n", argv[ARGV_CMD]);
        return 1;
    }
    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[ARGV_IP]);

    start_port = atoi(argv[ARGV_START_PORT]);
    end_port = atoi(argv[ARGV_END_PORT]);

    for (port = start_port; port <= end_port; port++)
    {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("socket ");
            return 1;
        }
        addr.sin_port = htons(port);

        if (connect_timeout(sock, (struct sockaddr *)&addr, sizeof(addr), 1) >= 0)
        {
            printf("%s:%d open \n", argv[ARGV_IP], port);
        }
        close(sock);
    }
    return 0;

}
int connect_timeout( int sock, struct sockaddr* addr, unsigned long len, int sec){
    fd_set fread, fwrite;
    struct timeval tv;
    int res;
    int sock_error = 0;
    unsigned int errsize;
    unsigned long nonblock = 1;

    if( fcntl (sock, F_SETFL, O_NONBLOCK) < 0){
        perror("fnctl ");
        return -1;
    }

    FD_ZERO(&fread);
    FD_ZERO(&fwrite);

    FD_SET(sock, &fread);
    FD_SET(sock, &fwrite);

    tv.tv_sec = 1;
    tv.tv_usec=0;

    if(connect(sock, addr, len)<0){
        if( errno != EINPROGRESS) return -1;
    }

    if((res = select(sock+1, &fread, &fwrite, NULL, &tv)) < 0){
        perror("select ");
        return -1;
    }

    if( res==0 ){
        return -1;
    }
    if( FD_ISSET(sock, &fread) || FD_ISSET(sock, &fwrite)){
        errsize = sizeof(sock_error);

        if(getsockopt(
            sock, SOL_SOCKET, SO_ERROR, (void *)&sock_error, &errsize)<0){
                perror("getsockopt");
                return -1;
            }

            if(!sock_error) {
                return 0;
            }
    }
    return -1;
}

