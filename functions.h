#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

#define DATA_SIZE 256
#define CRC_SIZE 32
#define THREAD_COUNT 2
#define TIMEOUT 3500
#define RECV_DIR "~/"

#define DATA_OK 1
#define REPEAT 0
#define ERROR -1 

//Errors
#define WRITE_ERROR -16
#define WRITE_FINISH_ERROR -17
#define WRITE_ANSW_ERROR -18
#define WRITE_READ_ERROR -19
#define WRITE_SERVER_ERROR -20
#define WRITE_REPEAT_ERROR -21

#define READ_ANSW_ERROR -22
#define READ_WRITE_ERROR -23
#define READ_ERROR -24
#define READ_REPEAT_ERROR -25

#define REPEAT_CNT 3

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

int my_read(int sockfd, void* buf, int bufsz);

int my_write(int sockfd, void* buf, int bufsz);

int send_recv(int sockfd, void* buf, int bufsz);

int recv_send(int sockfd, void* buf, int bufsz);

