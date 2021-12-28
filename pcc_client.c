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


int main(int argc, char** argv) {
    uint16_t port;
    uint32_t N, htonlN, C, intBuff;
    int sockfd, notWritten, bytesRead, totalRead, notRead, retVal;
    char *ip, *path, *NBuff, *outBuff, *inBuff;
    FILE *file;
    struct sockaddr_in serv_addr;

    if(argc != 4) {
        perror("invalid input\n");
        exit(1);
    }

    ip = argv[1];
    port = atoi(argv[2]);
    path = argv[3];

    /*open the input file*/
    file = fopen(path, "r");
    if(file == NULL) {
        perror("open file failed\n");
        exit(1);
    }

    /*initializing socket*/
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed\n");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.port = htons(port);
    if( (inet_pton(ip, &serv_addr.sin_addr) != 1) { //checkk
        perror("converting ip failed\n");
        exit(1);
    }

    /*connceting to server*/
    if( (connect(sockfd, (struct sock addr*) &serv_addr, sizeof(serv_addr))  < 0) {
        perror("connect failed\n");
        exit(1);
    }

    /*determine the file size*/
    fseek(file, 0L, SEEK_END);
    N = ftell(file);
    htonlN = htonl(N);
    fseek(file, 0L, SEEK_SET); //change to rewind

    /*allocating the sending buffer*/
    outBuff = (char*)malloc(N);
    if(outBuff == NULL) {
        perror("out buffer allocation failed\n");
        exit(1);
    }

    /*sending file size to server*/
    NBuff = (char*)&htonlN;
    retVal = sendingData(sockfd, 4, NBuff);
    if(retVal == 0) {
        perror("writing to socket failed\n");
        exit(1);
    }

    /*writing file to outBuff*/
    if( (fread(outBuff, 1, N, file)) != N ) {
        perror("read from file failed\n");
        exit(1);
    }

    /*sending file to server*/
    retVal = sendingData(sockfd, N, outBuff);
    if(retVal == 0) {
        perror("writing to socket failed\n");
        exit(1);
    }

    /*reading C from server*/
    inBuff = (char*)&(intBuff);

    totalRead = 0;
    notRead = 4;
    while(notRead > 0) {
        bytesRead = read(sockfd, inBuff+totalRead, notRead);
        if(bytesRead < 0) {
            perror("read from socket failed\n");
            exit(1);
        }
        totalRead += bytesRead;
        notRead -= bytesRead;
    }

    C = nthol(intBuff);
    close(file);
    close(sockfd);

    printf("# of printable characters: %u\n", C);
    exit(0);
}