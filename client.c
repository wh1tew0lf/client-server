#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "errmacro.h"
#include "headers.h"

void client(char *ip, int port);
 
int main(int argc, char **argv) {
    char ip[16] = "127.0.0.1";
    if (argc > 1) {
        memset(ip, 0, 16);
        strcpy(ip, argv[1]);
    }
    int port = argc > 2 ? atoi(argv[2]) : 9378;
    client(ip, port);
    return EXIT_SUCCESS;
}

void pack_heart_comm(char * buf) {
    memset(buf, 0, COMM_BUF_LEN);
    buf[0] = COMM_HEART_BEAT;
}

 
void client(char *ip, int port) {
    int sockfd = socket (AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr= inet_addr(ip);
    address.sin_port = htons(port);
    
    if (connect(sockfd, (struct sockaddr *) &address, sizeof(address))) {
        int myerr = errno;
        printf("Client: Connect: %s [%d]\n", strerror(myerr), myerr);
        exit(2);
    }

    char com[COMM_BUF_LEN];
    int i = 0;
    for(i = 0; i < 10; ++i) {
        pack_heart_comm(com);
        int rsnd = send(sockfd, (void*) com, COMM_BUF_LEN, 0);
            
        if (COMM_BUF_LEN != rsnd) {
            MY_ERROR1("Can't send command");
            break;
        }
        sleep(3);
    }


    /*char filename[] = RECV_FILENAME;
    
    off64_t size = -1;
    FILE *fs = fopen(filename, "w");
    if (fs != NULL) {
        recv(sockfd, &size, sizeof(size), MSG_WAITALL);
        
        for(;size > 0; size -= MIN(DATA_SIZE, size)) {
            char buf[DATA_SIZE];
            memset(buf, 0, DATA_SIZE);
            unsigned int portion = MIN(DATA_SIZE, size);
            
            //printf("Recv %d portion of file\n", portion);

            int rsz = recv(sockfd, (void*) buf, portion, MSG_WAITALL);
            
            if (portion != rsz) {
                int myerr = errno;
                printf("Can't recv portion of file %d: %s [%d]\n", rsz, strerror(myerr), myerr);
                break;
            }
                             
            if (portion != fwrite(buf, sizeof(char), portion, fs)) {
                int myerr = errno;
                printf("Can't write portion of file: %s [%d]\n", strerror(myerr), myerr);
                break;
            }
            
        }
    
        if (fclose(fs)) {
            int myerr = errno;
            printf("Server: close file file: %s [%d]\n", strerror(myerr), myerr);
        }
    } else {
        int myerr = errno;
        printf("Server: open file: %s [%d]\n", strerror(myerr), myerr);
    }

    send(sockfd, &size, sizeof(size), 0);
    */
    if (close(sockfd)) {
        int myerr = errno;
        printf("Client: close server socket: %s [%d]\n", strerror(myerr), myerr);
    }
}
