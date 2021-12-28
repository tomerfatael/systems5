#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

int sendingData(int sockfd, int notWritten, char *buff) {
    int bytesWrite, totalSent;
    totalSent = 0;
    while(notWritten > 0) {
        bytesWrite = write(sockfd, buff+totalSent, notWritten);
        if(bytesWrite < 0) {
            return 0;
        }
        totalSent += bytesWrite;
        notWritten -= bytesWrite;
    }
    return 1;
}

int readingData(int connfd, int notRead, char *buff) {
    int bytesRead, totalRead, expected;
    expected = notRead;
    totalRead = 0;
    while(notRead > 0) {
        bytesRead = read(connfd, buff+totalRead, notRead);
        if((bytesRead < 0) || (bytesRead == 0 && totalRead != expected)) { //check about errno maybe need to change
            return 0;
        }

        totalRead += bytesRead;
        notRead -= bytesRead;
    }
    return 1;
}

int main(int argc, char** argv) {
    uint16_t port;
    uint32_t N, C, intBuff;
    int listenfd, connfd, retVal;
    socklen_t addrSize = sizeof(struct sockaddr_in);
    char *NBuff, *inBuff;
    struct sockaddr_in serv_addr;
    struct sockaddr_in peer_addr;
    uint32_t pcc_total[127] = {0};

    if(argc != 2) {
        perror("invalid input\n");
        exit(1);
    }

    port = atoi(argv[1]);

    /*initializing socket*/
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed\n");
        exit(1);
    }

    memset(&serv_addr, 0, addrSize);
    serv_addr.sin_family = AF_INET;
    serv_addr.port = htons(port); //check if htonl or htons whats the difference?
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /*binding socket and listening to incoming TCP connections*/
    if( 0 != bind(listenfd, (struct sockaddr*) &serv_addr, addrSize) ) {
        perror("bind failed\n");
        exit(1);
    }

    if( 0 != listen(listenfd, 10) ) {
        perror("listen failed\n");
        exit(1);
    }

    /*reading N from client*/
    NBuff = (char*)&(intBuff);


    while(1) {
       if( (connfd = accept(listenfd, (struct sockaddr*) &peer_addr, &addrSize)) < 0 ) {
           perror("accept failed\n");
           exit(1);
       }





        

    }





}