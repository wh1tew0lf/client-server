#pragma once
#ifndef MY_ERRORS_H
#define MY_ERRORS_H 

#include <errno.h>

#ifndef MSTDERR
#define MSTDERR stderr
#endif

#define SHOWERROR 1

#define MY_ERROR1(text) if (SHOWERROR) {                                 \
        int myerr = errno;                                              \
        fprintf(MSTDERR, "%s:%d, *%s* %s '%s'[%d]\n",                   \
                __FILE__, __LINE__, __FUNCTION__, text, strerror(myerr), myerr); }

#define MY_ERROR2(text) if (SHOWERROR) {                                \
        fprintf(MSTDERR, "%s:%d, *%s* %s\n",                            \
                __FILE__, __LINE__, __FUNCTION__, text); }

#define MY_ERROR3(eprintf) if (SHOWERROR) {                             \
        fprintf(MSTDERR, "%s:%d, *%s* ",                                \
                __FILE__, __LINE__, __FUNCTION__);                      \
        eprintf;                                                        \
        fputs("\n", MSTDERR); }

#define MY_ERROR4(eprintf) if (SHOWERROR) {                             \
        int myerr = errno;                                              \
        fprintf(MSTDERR, "%s:%d, *%s* ",                                \
                __FILE__, __LINE__, __FUNCTION__);                      \
        eprintf;                                                        \
        fprintf(MSTDERR, " '%s'[%d]\n", strerror(myerr), myerr); }

#endif /* MY_ERRORS_H */
