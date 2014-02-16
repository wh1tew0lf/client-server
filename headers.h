#pragma once
#ifndef H_HEADERS_H
#define H_HEADERS_H

#define DEF_PORT 9378
#define DEF_THREAD_CNT 2
#define DATA_SIZE 256

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
