#include "functions.h"

/*int main(int argc, char **argv) {
    printf("Check shell...");
    if (system(NULL)) {
        puts("Ok");
    } else {
        puts("Fail");
        exit(EXIT_FAILURE);
    }

    char com[256];
    memset(com, 0, 256);
    while(fgets(com, 256, stdin) != NULL) {
        char *output;
        int outsize = 0;
    
        if (EXIT_SUCCESS == exec(com, &output, &outsize)) {
            printf("Out[%ld/%d]:\n %s\n", strlen(output), outsize, output);
            free(output);
        } else {
            puts("Fail");
        }
        memset(com, 0, 256);
    }
    
    return EXIT_SUCCESS;
}*/


int exec(char* cmd, char** output, int* outsize) {
    FILE* pipe = popen(cmd, "r");
    if (pipe != NULL) {
        int block = (*outsize > 0) ? *outsize : BLOCK_LEN;
        *outsize = block;
        *output = (char*) malloc(block * sizeof(char));
        if (*output != NULL) {
            memset(*output, 0, block);
            int pos = 0;
            char line[LINE_LEN];
            memset(line, 0, LINE_LEN);
            while(!feof(pipe)) {
                if(fgets(line, LINE_LEN, pipe) != NULL) {
                    int len = strlen(line);
                    if ((pos + len) > *outsize) {
                        char *tmp = *output;
                        *output = (char *)malloc((*outsize + block) * sizeof(char));
                        if (*output != NULL) {
                            memset(*output, 0, (*outsize + block));
                            memcpy(*output, tmp, *outsize);
                            *outsize = *outsize + block;
                            *outsize += block;
                        } else {
                            *output = tmp;
                            break;
                        }
                    }
                    char *current = *output + pos;
                    memcpy(current, line, len);
                    pos += len;
                }
            }
        }
        pclose(pipe);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
