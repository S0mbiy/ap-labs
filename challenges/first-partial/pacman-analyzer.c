#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define REPORT_FILE "packages_report.txt"

void analizeLog(char *logFile, char *report);

int main(int argc, char **argv) {

    if (argc < 1) {
	    printf("Usage:./pacman-analizer.o pacman.log\n");
	    return 1;
    }

    //analizeLog(argv[1], REPORT_FILE);
    analizeLog("pacman.txt", REPORT_FILE);

    return 0;
}

void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);
    int fd, cRead;
    char *c = (char *) calloc(512, sizeof(char));
     
    fd = open(logFile, O_RDONLY);
    if(fd < 0){
        printf("Couldn't open log file\n");
        return;
    }
    do{
        char *line = "";
        int i, eof = 0; 
        cRead = read(fd, c, 512);
        if(cRead==-1){
            printf("Couldn't read log file\n");
            return;
        }
        for(i = 0; c[i]!='\n'; i++){
            if(c[i] == EOF){
                eof = 1;
                break;
            }
            line[i] = c[i];
        }
        line[i]='\0';
        if(eof)
            break;
        printf("line is: %s, lseek moved: %d", line, i-512);
        int seek = lseek (fd, (i+1)-512, SEEK_CUR);
        if(seek==-1){
            printf("Couldn't read line\n");
            return;
        }
    } while(1);


    if (close(fd) < 0)  
    { 
        printf("Couldn't close file"); 
        return;
    }  

    printf("Report is generated at: [%s]\n", report);
}