#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <signal.h>

uint32_t pcc_total[127] = {0};
short isProcessing = 0;

void SIGINT_handler() {
    int i;
    if(!isProcessing) return; //check about this condition
    for(i = 0; i < 127; i++) {
        if(pcc_total[i] != 0) {
            printf("char '%c' : %u times\n", (char)i, pcc_total[i]);
        }
    }
    exit(0);   
}

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

int countPrintableChars(int N, char *clientBuff) {
    int i, C;
    C = 0;
    for(i = 0; i < N; i++) {
        if(32 <= clientBuff[i] && clientBuff[i] <= 126) {
            pcc_total[(int)clientBuff[i]]++;
            C++;
        }
    }
    return C;
}

int main(int argc, char** argv) {
    uint16_t port;
    uint32_t N, C, intBuff;
    int listenfd, connfd, retVal;
    socklen_t addrSize = sizeof(struct sockaddr_in);
    char *inBuff, *clientBuff;
    struct sockaddr_in serv_addr;
    struct sockaddr_in peer_addr;
    struct sigaction sa;

    /*SIGINT handler initialize*/
    sa.sa_handler = &SIGINT_handler;
    sa.sa_flags = SA_RESTART;
    if( (sigactio(SIGINT, &sa, NULL)) != 0){
        perror("sigaction failed\n");
        exit(1);
    }

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
    serv_addr.port = htons(port); 
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

    while(1) {
        isProcessing = 1; //checkkk
       if( (connfd = accept(listenfd, (struct sockaddr*) &peer_addr, &addrSize)) < 0 ) {
           perror("accept failed\n");
           exit(1);
       }

       /*reading N from clien*/ //this should be in if statement to run only once not??
        inBuff = (char*)&intBuff;
        retVal = readingData(connfd, 4, inBuff);
        if(retVal == 0) {
            perror("read from socket failed\n"); //check about errno cases
            exit(1);
        }
        N = nthol(intBuff);

        /*reading file data from client*/
        clientBuff = (char*)malloc(N); //also this need to run only once???
        if(clientBuff == NULL) {
            perror("client buffer allocation failed\n");
            exit(1);
        }
        retVal = readingData(connfd, N, clientBuff);
        if(retVal == 0) {
            perror("read from socket failed\n"); //check about errno cases
            exit(1);
        }

        /*counting printable characters and C*/
        C = countPrintableChars(N, clientBuff);

        /*sending C back to client*/
        intBuff = htonl(C);
        inBuff = (char*)&intBuff; //check if it is OK maybe need to free allocated memory first
        retVal = sendingData(connfd, 4, inBuff);
        if(retVal == 0) {
            perror("sending C to cliend failed\n");
            exit(1);
        }

        free(clientBuff);
        close(connfd);
        

    }
}