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
#include "headers.h"
#include "network.h"

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

    char buf[256];

    int i = 0;
    for(i = 0; i < 10; ++i) {
        memset(buf, 0, 256);
        int sbytes = send(sockfd, (void*) buf, 256, 0);
        if (sbytes != 256) {
            puts("Send error");
            break;
        }
        printf("Sended %d\n", buf[0]);
        int rbytes = recv(sockfd,(void*) buf, 256, MSG_WAITALL);
        if (rbytes != 256) {
            puts("Recv error");
            break;
        }
        printf("Received %d\n", buf[0]);
        sleep(1);
    }

    memset(buf, 0, 256);
    set_command(buf, C_SEND_FILE);
    int sbytes = send(sockfd, (void*) buf, 256, 0);
    if (sbytes != 256) {
        puts("Send error");
    } else {
        if (EXIT_SUCCESS == n_send_file(SEND_FILENAME, sockfd)) {
            puts("FILE SENDED");
        } else {
            puts("FILE SEND ERROR!");
        }
    }
    
    
    if (close(sockfd)) {
        int myerr = errno;
        printf("Client: close server socket: %s [%d]\n", strerror(myerr), myerr);
    }
}
