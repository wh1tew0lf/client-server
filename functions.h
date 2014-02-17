#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LEN 256
#define BLOCK_LEN 512

int exec(char* cmd, char** output, int* outsize);
