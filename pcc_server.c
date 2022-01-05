#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

uint32_t pcc_total[127] = {0};
uint32_t pcc_tmp[127] = {0};
short flageSIGINT = 0;
short isProcessing = 0;

void zeroPccTmp() {
    int i;
    for(i = 0; i < 127; i++) pcc_tmp[i] = 0;
}

void pccTotalPrint() {
    int i;
    for(i = 32; i <= 126; i++) {
        if(pcc_total[i] != 0) {
            printf("char '%c' : %u times\n", (char)i, pcc_total[i]);
        }
    }
}

void updatePccTotal() {
    int i;
    for(i = 0; i < 127; i++) {

    }
}

void SIGINT_handler() {
    if(!isProcessing) {
        pccTotalPrint();
        exit(0);
    }
    flageSIGINT = 1;
}

int sendingData(int sockfd, int notWritten, char *buff) {
    int bytesWrite, totalSent;
    totalSent = 0;
    while(notWritten > 0) {
        bytesWrite = write(sockfd, buff+totalSent, notWritten); 
        /*error has occurred*/
        if(bytesWrite < 0) {
            /*"real" error*/
            if((errno != ETIMEDOUT) && (errno != ECONNRESET) && (errno != EPIPE)) {
                return 0;
            }
            /*TCP connection error*/
            else {
                return -1;
            }
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
        /*possible errors*/
        if(bytesRead <= 0) { 
            /*error has occurred*/
            if(bytesRead < 0) {
                /*"real" error*/
                if((errno != ETIMEDOUT) && (errno != ECONNRESET) && (errno != EPIPE)) {
                    return 0;
                }

                /*TCP connection error*/
                else{
                    return -1;
                }
            }
            
            /*bytesRead == 0*/
            else {
                /*we didnt finish to read all data from client*/
                if(expected != totalRead) {
                    return -1;
                }   
            } 
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
        if (32 <= clientBuff[i] && clientBuff[i] <= 126) {
            pcc_tmp[(int)clientBuff[i]]++;
            C++;
        }
    }
    return C;
}

int main(int argc, char** argv) {
    uint16_t port;
    uint32_t N, C, intBuff;
    int listenfd, connfd, retVal = 1;
    char *inBuff, *clientBuff;
    struct sockaddr_in serv_addr; //storage size of serv_addr/peer isnt known
    struct sigaction sa;

    /*SIGINT handler initialize*/
    sa.sa_handler = &SIGINT_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask); //checkk
    if( (sigaction(SIGINT, &sa, NULL)) != 0){ //implicit declaretion
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

    memset(&serv_addr, 0, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);  
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 

    /*reusing the port*/
    if( (setsockopt(listfd, SOL_SOCKET, SO_REUSEADDR, &retVal, sizeof(int))) < 0 ) {
        perror("setsockopt faield\n");
        exit(1);
    }

    /*binding socket and listening to incoming TCP connections*/
    if( 0 != bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) ) {
        perror("bind failed\n");
        exit(1);
    }

    if( 0 != listen(listenfd, 10) ) {
        perror("listen failed\n");
        exit(1);
    }

    while(1) {
       if( (connfd = accept(listenfd, NULL, NULL)) < 0 ) {
           perror("accept failed\n");
           exit(1);
       }

       isProcessing = 1; //checkkk location

       /*reading N from clien*/ 
        inBuff = (char*)&intBuff;
        retVal = readingData(connfd, 4, inBuff);
        if(retVal == 0) {
            perror("read from socket failed\n"); //check about errno cases
            exit(1);
        }
        N = ntohl(intBuff); 

        /*reading file data from client*/
        clientBuff = (char*)malloc(N); 
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

        /*updating pcc_total only if we didn't get SIGINT in the while loop*/
        if(!flageSIGINT) {
            updatePccTotal();
        }

        zeroPccTmp();
        free(clientBuff);
        close(connfd);  
    }
}