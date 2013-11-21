#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "functions.h"

struct thread_conf {
    int id;
    int client_sockfd;
    int iret;
    pthread_t thread;
    pthread_mutex_t lock;
    int status;
};

int server_run = 1;

void server(int port);

void *print_message_function(void *ptr);

void sig_handler(int signum) {
    printf("I get %d signal, I will stop\n", signum);
    //signal(signum, SIG_DFL);
    //exit(3);
    server_run = 0;
}

int main(int argc, char **argv) {
    int port = argc > 1 ? atoi(argv[1]) : 9378;
    if (SIG_ERR == signal(SIGINT, sig_handler)) {
        int myerr = errno;
        printf("Server: Set sock opt: %s [%d]\n", strerror(myerr), myerr);
        exit(5);
    }
    server(port);
    return EXIT_SUCCESS;
}

void *print_message_function(void *ptr) {
    struct thread_conf *my_conf = (struct thread_conf *) ptr;
    int sockfd = my_conf->client_sockfd;
    int flag = 1;
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int))) {
        int myerr = errno;
        printf("Server: Thread #%d: Set sock opt: %s [%d]\n", my_conf->id, strerror(myerr), myerr);
    }

    int cnt = 0;
    if (!recv_send(sockfd, (void*) &cnt, sizeof(cnt))) {
        int i = 0;
        for(i = 0; i < cnt; ++i) {
            //Receive file #i
            char buf[DATA_SIZE];
            memset(buf, 0, DATA_SIZE);
            if (!recv_send(sockfd, (void*) buf, DATA_SIZE)) {
                char filename[DATA_SIZE] = RECV_DIR;
                strcat(filename, buf);
                FILE *fp = fopen(filename, "w");
                if (fp != NULL) {
                    int sz = 0;
                    if (!recv_send(sockfd, (void*) &sz, sizeof(sz))) {
                        int break_recv = 0;
                        for(;sz > 0; sz -= DATA_SIZE) {
                            int portion = MIN(DATA_SIZE, sz);
                            printf("Recive %d portion of file\n", portion);

                            if (recv_send(sockfd, (void*) buf, portion)) {
                                fclose(fp);
                                break_recv = 1;
                                break;
                            }
                            
                            if (portion != fwrite(buf, sizeof(char), portion, fp)) {
                                fclose(fp);
                                break_recv = 1;
                                break;
                            }
                        }
                        if (break_recv) {
                            break;
                        }
                    } else {
                        fclose(fp);
                        break;
                    }
                } else {
                    break;
                }
                fclose(fp);
            } else {
                break;
            }
        }
    }

    flag = 0;
	if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int))) {
        int myerr = errno;
        printf("Server: Thread #%d: Set sock opt: %s [%d]\n", my_conf->id, strerror(myerr), myerr);
    }

	if (close(sockfd)) {
        int myerr = errno;
        printf("Server: Thread #%d: Close socket: %s [%d]\n", my_conf->id, strerror(myerr), myerr);
    }

    pthread_mutex_lock(&(my_conf->lock));
    my_conf->status = 0;
    pthread_mutex_unlock(&(my_conf->lock));
}


void server(int port) {
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);
 
    if (bind(server_sockfd, (struct sockaddr *) &server_address, sizeof(server_address))) {
        int myerr = errno;
        printf("Server: Bind: %s [%d]\n", strerror(myerr), myerr);
        exit(2);
    } else {
        printf("bind is ok\n");
    }

    int flag = 1;
    if (setsockopt(server_sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int))) {
        int myerr = errno;
        printf("Server: Set sock opt: %s [%d]\n", strerror(myerr), myerr);
    }

    if (listen(server_sockfd, 5)) {
        int myerr = errno;
        printf("Server: Listen: %s [%d]\n", strerror(myerr), myerr);
        exit(3);
    } else {
        printf("listen is ok\n");
    }

    struct thread_conf threads[THREAD_COUNT];
    int i = 0;
    for(i = 0; i < THREAD_COUNT; ++i) {
        threads[i].status = -1;
        pthread_mutex_init(&(threads[i].lock), NULL);
    }
    
    for(i = 0; (i < THREAD_COUNT) && server_run; i = (i + 1) % THREAD_COUNT) {
        pthread_mutex_lock(&(threads[i].lock));
        if (!threads[i].status) { //if tread finish
            pthread_join(threads[i].thread, NULL);
            printf("Thread #%d returns: %d\n", i, threads[i].iret);
            pthread_mutex_unlock(&(threads[i].lock));
        } else if (threads[i].status > 0) { //if tread still busy
            pthread_mutex_unlock(&(threads[i].lock));
            continue;
        } else { // if thread only initialised
            pthread_mutex_unlock(&(threads[i].lock));
        }

        printf("server waiting for connection\n");
        
        struct sockaddr_in client_address;
        int client_len = sizeof(client_address);

        threads[i].client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);

        printf("server crating thread #%d\n", i);
        threads[i].status = 1;
        threads[i].iret = pthread_create(&(threads[i].thread), NULL, print_message_function, (void*) &(threads[i]));
    }

    for(i = 0; i < THREAD_COUNT; ++i) {
        pthread_join(threads[i].thread, NULL);
        printf("Thread #%d returns: %d\n", i, threads[i].iret);
    }

    flag = 0;
    if (setsockopt(server_sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int))) {
        int myerr = errno;
        printf("Server: Set sock opt: %s [%d]\n", strerror(myerr), myerr);
    }

    if (close(server_sockfd)) {
        int myerr = errno;
        printf("Server: close server socket: %s [%d]\n", strerror(myerr), myerr);
    }
}
