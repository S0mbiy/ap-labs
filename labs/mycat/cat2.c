#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/* filecopy:  copy file ifp to file ofp */
int filecopy(int ifp, int ofp)
{
    char c[1];
    if(read(ifp, c, 1) < 0)
        return 1;
    int l;
    while (1){
        if(write(ofp, c, 1) != 1)
            return 1;
        if((l = read(ifp, c, 1)) != 1){
            if (l == 0)
                break;
            return 1;
        }
    }
    return 0;
}

/* cat:  concatenate files, version 2 */
int main(int argc, char *argv[])
{
    int fp;
    int filecopy(int, int);
    char *prog = argv[0];   /* program name for errors */

    if (argc == 1){  /* no args; copy standard input */
        if (filecopy(0, 1) != 0){
            fprintf(stderr, "%s: operation failed\n", prog);
            return 4;
        }
    }
    else{
        while (--argc > 0){
            if ((fp = open(*++argv, O_RDONLY)) < 0) {
                fprintf(stderr, "%s: can′t open %s\n", prog, *argv);
                return 1;
            } else {
                if (filecopy(fp, 1) != 0){
                    fprintf(stderr, "%s: operation failed with file %s\n", prog, *argv);
                    return 4;
                }
                if (close(fp)==-1){
                    fprintf(stderr, "%s: can′t close %s\n", prog, *argv);
                    return 3;
                }
            }
        }
    }
    if (ferror(stdout)) {
        fprintf(stderr, "%s: error writing stdout\n", prog);
        return 2;
    }

    return 0;
}
