#include "network.h"

int n_send_file(char filename[], int sockfd) {
    int status = EXIT_SUCCESS;
    off64_t size = -1;
    FILE *fs = fopen(filename, "r");
    if (fs != NULL) {
        fseek(fs, 0L, SEEK_END);
        size = ftello64(fs);
        fseek(fs, 0L, SEEK_SET);
        if (sizeof(size) == send(sockfd, &size, sizeof(size), 0)) {
            for(;size > 0; size -= MIN(DATA_SIZE, size)) {
                char buf[DATA_SIZE];
                memset(buf, 0, DATA_SIZE);
                unsigned int portion = MIN(DATA_SIZE, size);
                             
                if (portion != fread(buf, sizeof(char), portion, fs)) {
                    MY_ERROR1("Can't read portion of file");
                    status = EXIT_FAILURE;
                    break;
                }
            
                if (portion != send(sockfd, (void*) buf, portion, 0)) {
                    MY_ERROR1("Can't send portion of file");
                    status = EXIT_FAILURE;
                    break;
                }
            }
        }
        
    
        if (fclose(fs)) {
            MY_ERROR1("Close file");
            status = EXIT_FAILURE;
        }
    } else {
        MY_ERROR1("Open file");
        status = EXIT_FAILURE;
    }
    return status;
}

int n_recv_file(char filename[], int sockfd) {
    int status = EXIT_SUCCESS;
    off64_t size = -1;
    FILE *fs = fopen(filename, "w");
    if (fs != NULL) {
        if (sizeof(size) == recv(sockfd, &size, sizeof(size), MSG_WAITALL)) {
            for(;size > 0; size -= MIN(DATA_SIZE, size)) {
                char buf[DATA_SIZE];
                memset(buf, 0, DATA_SIZE);
                unsigned int portion = MIN(DATA_SIZE, size);
            
                if (portion != recv(sockfd, (void*) buf, portion, MSG_WAITALL)) {
                    MY_ERROR1("Can't receive portion of file");
                    status = EXIT_FAILURE;
                    break;
                }
                             
                if (portion != fwrite(buf, sizeof(char), portion, fs)) {
                    MY_ERROR1("Can't write portion of file");
                    status = EXIT_FAILURE;
                    break;
                }
            }
        }
        
        if (fclose(fs)) {
            MY_ERROR1("Close file");
            status = EXIT_FAILURE;
        }
    } else {
        MY_ERROR1("Open file");
        status = EXIT_FAILURE;
    }
    return status;
}

