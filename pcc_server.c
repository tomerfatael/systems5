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
    struct sockaddr_in serv_addr;
    
    if(argc != 2) {
        perror("invalid input\n");
        exit(1);
    }

    port = atoi(argv[1]);

}