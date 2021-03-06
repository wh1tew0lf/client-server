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
#include <fcntl.h>
#include "errmacro.h"
#include "headers.h"

int server_run = 1;

void server(int port, int tcnt);
void *client_thread(void *ptr);
void sig_handler(int signum);

void pack_heart_comm(char * buf);
void pack_set_dev_comm(char * buf);
void pack_get_dev_comm(char * buf);
void pack_set_over_comm(char * buf);
void pack_get_over_comm(char * buf);

void unpack_heart_comm(char * buf);
void unpack_set_dev_comm(char * buf);
void unpack_get_dev_comm(char * buf);
void unpack_set_over_comm(char * buf);
void unpack_get_over_comm(char * buf);



int main(int argc, char **argv) {
    int port = argc > 1 ? atoi(argv[1]) : DEF_PORT;
    int tcnt = argc > 2 ? atoi(argv[2]) : DEF_THREAD_CNT;
    if (SIG_ERR == signal(SIGINT, sig_handler)) {
        MY_ERROR1("Init sighandler");
        exit(5);
    }
    server(port, tcnt);
    return EXIT_SUCCESS;
}

void sig_handler(int signum) {
    printf("I get %d signal, I will stop\n", signum);
    server_run = 0;
}

void server(int port, int tcnt) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);
 
    if (bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address))) {
        MY_ERROR1("Bind");
        exit(2);
    } else {
        printf("Bind is ok\n");
    }

    if (listen(sockfd, 5)) {
        MY_ERROR1("Listen");
        exit(3);
    } else {
        printf("Listen is ok\n");
    }

    struct thread_conf *threads = (struct thread_conf *) malloc(tcnt * sizeof(struct thread_conf));
    int i = 0;
    for(i = 0; i < tcnt; ++i) {
        threads[i].status = -1;
        pthread_mutex_init(&(threads[i].lock), NULL);
    }
    
    for(i = 0; (i < tcnt) && server_run; i = (i + 1) % tcnt) {
        pthread_mutex_lock(&(threads[i].lock));
        if (!threads[i].status) {                                      //if tread finish
            pthread_join(threads[i].thread, NULL);
            printf("Thread #%d returns: %d\n", i, threads[i].iret);
            pthread_mutex_unlock(&(threads[i].lock));
        } else if (threads[i].status > 0) {                            //if tread still busy
            pthread_mutex_unlock(&(threads[i].lock));
            continue;
        } else {                                                       // if thread only initialised
            pthread_mutex_unlock(&(threads[i].lock));
        }

        printf("server waiting for connection\n");
        
        int pret = 0;
        while(server_run) {
            struct pollfd serv;
            serv.fd = sockfd;
            serv.events = POLLIN;
            pret = poll(&serv, 1, SRV_TIMEOUT);

            if (-1 == pret) {
                MY_ERROR1("Poll");
                break;
            } else if(0 == pret) { //timeout
                continue;
            } else {
                break;
            }
        }
        if ((-1 == pret) || !server_run) {
            break;
        }

        struct sockaddr_in client_address;
        int client_len = sizeof(client_address);
        threads[i].sockfd = accept(sockfd, (struct sockaddr *) &client_address, &client_len);

        printf("server crating thread #%d\n", i);
        threads[i].status = 1;
        threads[i].iret = pthread_create(&(threads[i].thread), NULL, client_thread, (void*) &(threads[i]));
    }

    for(i = 0; i < tcnt; ++i) {
        pthread_join(threads[i].thread, NULL);
        printf("Thread #%d returns: %d\n", i, threads[i].iret);
    }

    if (close(sockfd)) {
        MY_ERROR1("Close server socket");
    }
}

void *client_thread(void *ptr) {
    struct thread_conf *my_conf = (struct thread_conf *) ptr;
    int sockfd = my_conf->sockfd;

    int command_loop = 1;
    int skip = 0;

    while(command_loop && !skip) {
        struct pollfd client;
        client.fd = sockfd;
        client.events = POLLIN;

        int ret = poll(&client, 1, TIMEOUT);
        int parse_comm = 0;
        char com[COMM_BUF_LEN];
        memset(com, 0, COMM_BUF_LEN);
        
        if (-1 == ret) {
            MY_ERROR1("Poll");
            skip = 1;
        } else if(0 == ret) {
            continue;
        } else {
            int bytes = 0;
            if (client.revents & POLLIN) {
                bytes = recv(sockfd, com, COMM_BUF_LEN, MSG_WAITALL);
                if (-1 == bytes) {
                    MY_ERROR1("Recv");
                    skip = 2;
                } else if (bytes < COMM_BUF_LEN) { 
                    MY_ERROR1("Recv, not full data");
                    skip = 3;
                } else {
                    //All nice
                    parse_comm = 1;
                }
            }
            ret = bytes;
        }

        if (parse_comm) {
            switch(com[0]) {
            case COMM_HEART_BEAT:
                printf("Headbeat command\n");
                break;
            default:
                MY_ERROR2("Incorrect command");
                skip = 1;
            } //switch(com[0])
        } //if (parse_comm)
    } //while(command_loop && !skip)



    /*char filename[] = SEND_FILENAME;
    
    off64_t size = -1;
    FILE *fs = fopen(filename, "r");
    if (fs != NULL) {
        fseek(fs, 0L, SEEK_END);
        size = ftello64(fs);
        fseek(fs, 0L, SEEK_SET);

        printf("File size: %ld[%ld]\n", size, sizeof(size));

        send(sockfd, &size, sizeof(size), 0);
        
        for(;size > 0; size -= MIN(DATA_SIZE, size)) {
            char buf[DATA_SIZE];
            memset(buf, 0, DATA_SIZE);
            unsigned int portion = MIN(DATA_SIZE, size);
            
            //printf("Send %d portion of file\n", portion);
                             
            if (portion != fread(buf, sizeof(char), portion, fs)) {
                MY_ERROR1("Can't read portion of file");
                break;
            }
            
            int rsnd = send(sockfd, (void*) buf, portion, 0);
            
            if (portion != rsnd) {
                MY_ERROR1("Can't send portion of file");
                break;
            }
        }
    
        if (fclose(fs)) {
            MY_ERROR1("Can't send portion of file");
        }
    } else {
        MY_ERROR1("Open file");
    }*/
    
	if (close(sockfd)) {
        MY_ERROR1("Close socket");
    }

    pthread_mutex_lock(&(my_conf->lock));
    my_conf->status = 0;
    pthread_mutex_unlock(&(my_conf->lock));
    pthread_exit(&skip);
}

void pack_heart_comm(char * buf) {
    memset(buf, 0, COMM_BUF_LEN);
    buf[0] = COMM_HEART_BEAT;
}

