#include "functions.h" 

int my_read(int sockfd, void* buf, int bufsz) {
    struct pollfd client;
    client.fd = sockfd;
    client.events = POLLIN;

    int ret = poll(&client, 1, TIMEOUT); //timeout 3500ms = 3.5s

    if (-1 == ret) {
        int myerr = errno;
        printf("Server: Thread: Poll: %s [%d]\n", strerror(myerr), myerr); fflush(stdout);
    } else if(0 == ret) { //timeout
        
    } else {
        if (client.revents & POLLIN) {
            int bytes = recv(sockfd, buf, bufsz, MSG_WAITALL);
            if (-1 == bytes) {
                int myerr = errno;
                printf("Server: Thread: recv: %s [%d]\n", strerror(myerr), myerr);
            } else if (bytes < bufsz) { //We don't receive all data
                //Hmm... We don't get all data
                ret = 0;
            } else { //All nice
                ret = bytes;
            }
        }
    }
    return ret;
}

int my_write(int sockfd, void* buf, int bufsz) {
    struct pollfd client;
    client.fd = sockfd;
    client.events = POLLOUT;

    int ret = poll(&client, 1, TIMEOUT);

    if (-1 == ret) {
        int myerr = errno;
        printf("Server: Thread: Poll: %s [%d]\n", strerror(myerr), myerr);
        
    } else if(0 == ret) { //timeout
        
    } else {
        if (client.revents & POLLOUT) {
            int bytes = send(sockfd, buf, bufsz, 0);
            if (-1 == bytes) {
                int myerr = errno;
                printf("Server: Thread: send: %s [%d]\n", strerror(myerr), myerr);
            } else if (bytes < bufsz) { //We don't receive all data
                //Hmm... We don't get all data
                ret = 0;
            } else { //All nice
                ret = bytes;
            }
        }
    }
    return ret;
}

int send_recv(int sockfd, void* buf, int bufsz) {
    int i = 0;
    for(i = 0; i < REPEAT_CNT; ++i) {
        int write_bytes = my_write(sockfd, buf, bufsz);
        if (write_bytes == -1) {  //error
            return WRITE_ERROR;
        } else if (write_bytes != bufsz) {
            return WRITE_FINISH_ERROR;
        }

        int answer = 0;
        int read_bytes = my_read(sockfd, (void*) &answer, sizeof(answer));
        if (read_bytes == -1) {  //error
            return WRITE_ANSW_ERROR;
        } else if (read_bytes != sizeof(answer)) {
            return WRITE_READ_ERROR;
        } else {
            if (answer == ERROR) {
                return WRITE_SERVER_ERROR;
            } else if (answer == DATA_OK) {
                return 0;
            }
        }
    }
    return WRITE_REPEAT_ERROR;
}

int recv_send(int sockfd, void* buf, int bufsz) {
    int i = 0;
    for(i = 0; i < REPEAT_CNT; ++i) {
        int read_bytes = my_read(sockfd, buf, bufsz);
        int answer = 0;
        if (read_bytes == -1) {  //error
            answer = ERROR;
        } else if (read_bytes != bufsz) {
            answer = REPEAT;
        } else {
            answer = DATA_OK;
        }

        int write_bytes = my_write(sockfd, (void*) &answer, sizeof(answer));

        if (write_bytes == -1) {  //error
            return READ_ANSW_ERROR;
        } else if (write_bytes != sizeof(answer)) {
            return READ_WRITE_ERROR;
        } else {
            if (answer == ERROR) {
                return READ_ERROR;
            } else if (answer == DATA_OK) {
                return 0;
            }
        }
    }
    return READ_REPEAT_ERROR;
}
