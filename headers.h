#pragma once
#ifndef H_HEADERS_H
#define H_HEADERS_H

#define DEF_PORT 9378
#define DEF_THREAD_CNT 2
#define DATA_SIZE 256

#define SEND_FILENAME "~/1.avi"
#define RECV_FILENAME "~/2.avi"

#define SRV_TIMEOUT 3500
#define TIMEOUT 3500

#define COMM_BUF_LEN 20

#define COMM_HEART_BEAT 0
#define COMM_SET_DEVIATION 1
#define COMM_GET_DEVIATION 2
#define COMM_SET_OVERLAP 3
#define COMM_GET_OVERLAP 4

#define MIN(a,b) (((a)<(b))?(a):(b))

struct thread_conf {
    int id;
    int sockfd;
    int iret;
    pthread_t thread;
    pthread_mutex_t lock;
    int status;
};


#endif /*H_HEADERS_H*/
