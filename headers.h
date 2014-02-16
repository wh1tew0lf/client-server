
#pragma once
#ifndef H_HEADERS_H
#define H_HEADERS_H

struct thread_conf {
    int id;
    int sockfd;
    int iret;
    pthread_t thread;
    pthread_mutex_t lock;
    int status;
};


#endif /*H_HEADERS_H*/
