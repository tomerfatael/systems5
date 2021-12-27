#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>


int main(int argc, char** argv) {
    uint16_t port;
    uint32_t N, htonlN, C;
    int sockfd, bytesWrite, notWritten, totalSent, bytesRead, totalRead;
    char *ip, *path *outBuff, *inBuff;
    FILE *file;
    struct sockaddr_in serv_addr;
    struct stat st;

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
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0) < 0) {
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
    fseek(file, 0L, SEEK_SET);

    /*allocating the sending buffer*/
    outBuff = (char*)malloc(N);
    if(outBuff == NULL) {
        perror("out buffer allocation failed\n");
        exit(1);
    }

    /*sending file size to server*/
    notWritten = 4;
    while(notWritten > 0) {
        bytesWrite = write(sockfd, ) //to completeeee
    }

    /*sending file to server*/
    notWritten = N; 
    totalSent = 0;
    while(notWritten > 0) {
        bytesWrite = write(sockfd, outBuff+totalSent, notWritten);
        if(bytesWrite < 0) {
            perror("writing to socket failed\n");
            exit(1);
        }
        totalSent += bytesWrite;
        notWritten -= bytesWrite;
    }

    /*reading C from server*/
    inBuff = (char*)malloc(N); //checkkkk
    if(inBuff == NULL) {
        perror("in buffer allocation failed\n");
        exit(1);
    }
    totalRead = 0;
    while(totalRead < 4) {
        bytesRead = read(sockfd, inBuff+totalRead, 4-totalRead);
        if(bytesRead < 0) {
            perror("read from socket failed\n");
            exit(1);
        }
        totalRead += bytesRead;
    }

    C = ///
    close();
    close();

    printf("# of printable characters: %u\n", C);
    exit(0);

}