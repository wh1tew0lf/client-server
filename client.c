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
#include "functions.h"
 
void client(char *ip, int port, char **files, int files_cnt);
 
int main(int argc, char **argv) {
    char ip[16] = "192.168.0.100";
    if (argc > 1) {
        memset(ip, 0, 16);
        strcpy(ip, argv[1]);
    }
    int port = argc > 2 ? atoi(argv[2]) : 9378;
    client(ip, port, &(argv[3]), argc - 3);
    return EXIT_SUCCESS;
}
 
void client(char *ip, int port, char **files, int files_cnt) {
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

    int flag = 1;
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int))) {
        int myerr = errno;
        printf("Client: Set sock opt: %s [%d]\n", strerror(myerr), myerr);
    }

    if (!send_recv(sockfd, (void*) &files_cnt, sizeof(files_cnt))) {
        int i = 0;
        for(i = 0; i < files_cnt; ++i) {
            //Send file #i
            char buf[DATA_SIZE];
            memset(buf, 0, DATA_SIZE);
            strcpy(buf, files[i]);
            printf("Send file #%d '%s'\n", i, buf);
            if (!send_recv(sockfd, (void*) buf, DATA_SIZE)) {
                
                FILE *fp = fopen(files[i], "r");
                if (fp != NULL) {
                    fseek(fp, 0L, SEEK_END);
                    int sz = ftell(fp);
                    fseek(fp, 0L, SEEK_SET);
                    
                    if (!send_recv(sockfd, (void*) &sz, sizeof(sz))) {
                        int break_send = 0;
                        for(;sz > 0; sz -= DATA_SIZE) {
                            int portion = MIN(DATA_SIZE, sz);
                            printf("Send %d portion of file\n", portion);
                            
                             
                            if (portion != fread(buf, sizeof(char), portion, fp)) {
                                fclose(fp);
                                break_send = 1;
                                break;
                            }

                            if (send_recv(sockfd, (void*) buf, portion)) {
                                fclose(fp);
                                break_send = 1;
                                break;
                            }
                        }
                        if (break_send) {
                            break;
                        }
                    } else {
                        fclose(fp);
                        break;
                    }
                    fclose(fp);
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    }
  
	flag = 0;
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int))) {
        int myerr = errno;
        printf("Client: Set sock opt: %s [%d]\n", strerror(myerr), myerr);
    }

    if (close(sockfd)) {
        int myerr = errno;
        printf("Client: close server socket: %s [%d]\n", strerror(myerr), myerr);
    }
}
