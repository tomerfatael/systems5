#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

int main(int argc, char** argv) {
    uint16_t port;
    int listenfd;
    uint32_t pcc_total[127];
    uint32_t C;
    struct sockaddr_in serv_addr;

    if(argc != 2) {
        perror("invalid input\n");
        exit(1);
    }

    port = atoi(argv[1]);

    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed\n");
        exit(1);
    }

    memset(&serv_addr, 0, addrsize(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.port = htons(port); //check if htonl or htons whats the difference?
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    


}