#pragma once
#ifndef MY_NETWORK_H
#define MY_NETWORK_H

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include "errmacro.h"

//Macroses - constants section
#define DEF_PORT 9378
#define DEF_THREAD_CNT 2
#define DATA_SIZE 256

#define SEND_FILENAME "/home/wh1/1.pdf"
#define RECV_FILENAME "/home/wh1/2.pdf"

#define SERVER_POLL_TIMEOUT 3500

#define C_HEARTBEAT 0
#define C_SEND_FILE 1

//Macroses - functions sections
#define MIN(a,b) (((a)<(b))?(a):(b))
#define get_command(buf) buf[0]
#define set_command(buf, command) buf[0] = command 

//Variables section
struct thread_conf {
    int id;
    int sockfd;
    int iret;
    pthread_t thread;
    pthread_mutex_t lock;
    int status;
};

//Functions section

int n_send_file(char filename[], int sockfd);
int n_recv_file(char filename[], int sockfd);

#endif /* MY_NETWORK_H */
