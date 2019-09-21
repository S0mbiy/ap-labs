#include <stdio.h>

int mystrlen(char *str);
char *mystradd(char *origin, char *addition);
int mystrfind(char *origin, char *substr);

int main(int argc, char **argv) {
    printf("Initial Lenght      : %d\n", mystrlen(argv[1]));
	printf("New String          : %s\n", mystradd(argv[1], argv[2]));
	printf("SubString was found : %s\n", mystrfind(argv[1], argv[3])?"yes":"no");
	return 0;
}
