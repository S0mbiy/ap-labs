#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

static int level = 0; //logging level 0=INFO, 1=WARNING, 2=ERROR, 3=PANIC
int infof(const char *format, ...){
    va_list arg;
    va_start(arg, format);
    int done;
    if(level == 0){
        char * sufix = (char *)malloc(sizeof(format) + 7);
        strcpy(sufix,"INFO: ");
        done = vfprintf(stdout, strcat(strcat(sufix, format), "\n"), arg);
        free(sufix);
    }
    va_end (arg);
    return done;
}
int warnf(const char *format, ...){
    va_list arg;
    va_start(arg, format);
    int done;
    if(level < 2){
        char * sufix = (char *)malloc(sizeof(format) + 11);
        strcpy(sufix,"WARNNING: ");
        done = vfprintf(stdout, strcat(strcat(sufix, format), "\n"), arg);
        free(sufix);
    }
    va_end (arg);
    return done;
}
int errorf(const char *format, ...){
    va_list arg;
    va_start(arg, format);
    int done;
    if(level < 3){
        char * sufix = (char *)malloc(sizeof(format) + 8);
        strcpy(sufix,"ERROR: ");
        done = vfprintf(stderr, strcat(strcat(sufix, format), "\n"), arg);
        free(sufix);
    }
    va_end (arg);
    return done;
}
int panicf(const char *format, ...){
    va_list arg;
    va_start(arg, format);
    int done;
    char * sufix = (char *)malloc(sizeof(format) + 8);
    strcpy(sufix,"PANIC: ");
    done = vfprintf(stderr, strcat(strcat(sufix, format), "\n"), arg);
    free(sufix);
    va_end (arg);
    raise(SIGABRT);
    return done;
}
void setLevel(int l){
    level = l;
}
