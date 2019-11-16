#include "logger.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>

static int level = 0; //logging level 0=INFO, 1=WARNING, 2=ERROR, 3=PANIC
static int std = 1;
int infof(const char *format, ...){
    va_list arg;
    va_start(arg, format);
    int done;
    if(level == 0){
        char * sufix = (char *)malloc(strlen(format) + 7);
        strcpy(sufix,"INFO: ");
		if(std)
        	done = vfprintf(stdout, strcat(strcat(sufix, format), "\n"), arg);
		else{
			openlog(NULL, LOG_CONS, LOG_USER);
			vsyslog(LOG_INFO, format, arg);
			closelog();
		}
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
        char * sufix = (char *)malloc(strlen(format) + 11);
        strcpy(sufix,"WARNNING: ");
		if(std)
        	done = vfprintf(stdout, strcat(strcat(sufix, format), "\n"), arg);
		else{
			openlog(NULL, LOG_CONS, LOG_USER);
			vsyslog(LOG_WARNING, format, arg);
			closelog();
		}
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
        char * sufix = (char *)malloc(strlen(format) + 8);
        strcpy(sufix,"ERROR: ");
		if(std)
        	done = vfprintf(stdout, strcat(strcat(sufix, format), "\n"), arg);
		else{
			openlog(NULL, LOG_CONS, LOG_USER);
			vsyslog(LOG_ERR, format, arg);
			closelog();
		}
        free(sufix);
    }
    va_end (arg);
    return done;
}
int panicf(const char *format, ...){
    va_list arg;
    va_start(arg, format);
    int done;
    char * sufix = (char *)malloc(strlen(format) + 8);
    strcpy(sufix,"PANIC: ");
	if(std)
		done = vfprintf(stdout, strcat(strcat(sufix, format), "\n"), arg);
	else{
		openlog(NULL, LOG_CONS, LOG_USER);
		vsyslog(LOG_EMERG, format, arg);
		closelog();
	}
    free(sufix);
    va_end (arg);
    raise(SIGABRT);
    return done;
}
void setLevel(int l){
    level = l;
}

int initLogger(char *logType) {
    printf("Initializing Logger on: %s\n", logType);
	if(strcmp(logType, "stdout")==0 || strcmp(logType, "")==0)
		std = 1;
	else if(strcmp(logType, "syslog")==0){
		std = 0;
	}
	else{
		return -1;
	}
    return 0;
}
