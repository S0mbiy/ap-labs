#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <malloc.h>

//------------------------------------------------------------------------------
//----------Hash Table from The C Programming Language Book---------------------
//------------------------------------------------------------------------------

struct nlist {              /* table entry: */
    struct nlist *next;     /* next entry in chain */
    char *name;             /* defined name */
    char *i_date;
    char *u_date;
    int updates;
    char *r_date;
};

#define HASHSIZE 5000

static struct nlist *hashtab[HASHSIZE]; /* pointer table */

/* hash: form hash value for string s */
unsigned hash(char *s)
{
    long total = 0;
    char *ret;

    int len = mystrlen(s);

    for (int k = 0; k < len; k++)
        total += 11 * total + (int)s[k];

    total = total % HASHSIZE;

    if (total < 0)
        total += HASHSIZE;
    //printf("%s hash: %ld\n",s,total);
    return (int)total;
}


/* lookup: look for s in hashtab */

struct nlist *lookup(char *s)
{
    struct nlist *np;

    for (np = hashtab[hash(s)]; np != NULL; np = np->next)
        if (strcmp(s, np->name) == 0)
            return np;  /* found */
    return NULL;        /* not found */
}

/* install: put (name, defn) in hashtab */
struct nlist *installed(char *name, char *i_date)
{
    struct nlist *np;
    unsigned hashval;

    if ((np = lookup(name)) == NULL)  { /* not found */
        np = (struct nlist *) malloc(sizeof(*np));

        if (np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        hashval = hash(name);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    } else  /* already there */
        free((void *) np->i_date);

    if ((np->i_date = strdup(i_date)) == NULL)
        return NULL;
    if ((np->r_date = strdup((char *)"-")) == NULL)
        return NULL;
    if ((np->u_date = strdup((char *)"-")) == NULL)
        return NULL;

    return np;
}
struct nlist *updated(char *name, char *u_date)
{
    struct nlist *np;
    unsigned hashval;

    if ((np = lookup(name)) == NULL)  { /* not found */
        np = (struct nlist *) malloc(sizeof(*np));

        if (np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        hashval = hash(name);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    }
    int updates = np->updates + 1;
    np->updates = updates;
    if ((np->u_date = strdup(u_date)) == NULL)
        return NULL;
    if ((np->r_date = strdup((char *)"-")) == NULL)
        return NULL;

    return np;
}
struct nlist *removed(char *name, char *r_date)
{
    struct nlist *np;
    unsigned hashval;

    if ((np = lookup(name)) == NULL)  { /* not found */
        np = (struct nlist *) malloc(sizeof(*np));

        if (np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        hashval = hash(name);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    }
    int updates = np->updates + 1;
    np->updates = updates;
    if ((np->r_date = strdup(r_date)) == NULL)
        return NULL;

    return np;
}

struct nlist *undef(char *name) {
    struct nlist *found;

    found = lookup(name);

    if (found == NULL) /* not found and nothing to do */
        return NULL;
    else {
        if (found->next != NULL) {
            found->next = found->next->next;
            found = found->next;
        } else {
            hashtab[hash(name)] = NULL;
            free((void *) found);
        }
    }
    return found;
}

//------------------------------------------------------------------------------
//---------------------------------My Code--------------------------------------
//------------------------------------------------------------------------------

#define REPORT_FILE "packages_report.txt"

struct metapckg {
    char *name;
    char *date;
};

int mystrlen(char *str){
	int i;
	for (i = 0; str[i] != '\0'; i++);
    return i;
}
struct metapckg *mystrfind(char *origin, char *substr){
	int i, found = 0;
    char* package = (char *)malloc(256 * sizeof(char));
    char* date = (char *)malloc(17 * sizeof(char));
    struct metapckg *p;
	int j = 0, k = 0;
    if(origin[0]!='['){
        return NULL;
    }
	for (i = 0; origin[i] != '\0'; i++){
        if(found){
            if(origin[i] != ' '){
                package[k++] = origin[i];
            }else{
                package[k] = '\0';
                p = (struct metapckg *) malloc(sizeof(*p));
                p->name = strdup(package);
                p->date = strdup(date);
                return p;
            }
        }
        if(0<i && i<17){
            date[i-1]=origin[i];
            date[16]='\0';
        }
		if(origin[i] == substr[j++]){
			if(j==mystrlen(substr)){
                found = 1;
			}
		}else{
			j = 0;
		}
	}
    return NULL;
}

int analizeLog(char *logFile, char *report);

int main(int argc, char **argv) {

    if (argc < 2) {
	    printf("Usage:./pacman-analizer.o pacman.log\n");
	    return 1;
    }

    return analizeLog(argv[1], REPORT_FILE);
}

int analizeLog(char *logFile, char *report) {
    int count = 0;
    printf("Generating Report from: [%s] log file\n", logFile);
    int fd, j=0, fpos=0;
    int upgr = 0, inst = 0, rem = 0;
    long long int size = 512;
    char* line = malloc(size * sizeof(char) + 1);
    fd = open(logFile, O_RDONLY);
    if(fd < 0){
        printf("Couldn't open log file\n");
        return 1;
    }
    do{
        int i, c_read, eof = 0;
        char c[513] = {};
        c_read = read(fd, c, 512);
        c[c_read] = '\0';
        if(c_read==-1){
            printf("Couldn't read log file\n");
            break;
        }
        for(i = 0; c[i]!='\n'; i++){
            if(i == 512){
                break;
            }else if(c[i] == '\0'){
                eof = 1;
                break;
            }
            line[j++] = c[i];
        }
        if(i == 512){
            size *= 2;
            char* tmp = realloc(line, size * sizeof(char) + 1);
            if (tmp)
            {
                line = tmp;
            }
        }else{
            line[j]='\0';
            count++;
            int seek = lseek (fd, fpos+=j+1, SEEK_SET);
            j = 0;
            if(seek==-1){
                printf("Couldn't read line\n");
                break;
            }
            struct metapckg *p = mystrfind(line, (char *)" installed ");
            if(p!=NULL){
                if(lookup(p->name)== NULL)
                    inst +=1;
                if(installed(p->name, p->date)==NULL){
                    printf("Error adding package info\n");
                    break;
                }
            }
            struct metapckg *p1 = mystrfind(line, (char *)" upgraded ");
            if(p1!=NULL){
                if(updated(p1->name, p1->date)==NULL){
                    printf("Error adding package info\n");
                    break;
                }else{
                    upgr+=1;
                }
            }
            struct metapckg *p2 = mystrfind(line, (char *)" removed ");
            if(p2!=NULL){
                if(strcmp(lookup(p2->name)->r_date, "-")== 0)
                    rem +=1;
                if(removed(p2->name, p2->date)==NULL){
                    printf("Error adding package info\n");
                    break;
                }
            }
            if(eof)
                break;
        }
    } while(1);
    FILE *fd_out;
    fd_out = fopen(report, "w");
    if(fd_out == NULL)
    {
       printf("Error Writting Output!");
       exit(1);
    }
    fprintf(fd_out,"Pacman Packages Report\n");
    fprintf(fd_out,"----------------------\n");
    fprintf(fd_out,"- Installed packages : %d\n", inst);
    fprintf(fd_out,"- Removed packages   : %d\n", rem);
    fprintf(fd_out,"- Upgraded packages  : %d\n", upgr);
    fprintf(fd_out,"- Current installed  : %d\n", inst-rem);
    fprintf(fd_out,"\n");
    fprintf(fd_out,"List of packages\n");
    fprintf(fd_out,"----------------\n");
    for (int i = 0; i < HASHSIZE; i++)
    {
        struct nlist *np;
        for (np = hashtab[i]; np != NULL; np = np->next){
            fprintf(fd_out,"- Package Name        : %s\n", np->name);
            fprintf(fd_out,"  - Install date      : %s\n", np->i_date);
            fprintf(fd_out,"  - Last update date  : %s\n", np->u_date);
            fprintf(fd_out,"  - How many updates  : %d\n", np->updates);
            fprintf(fd_out,"  - Removal date      : %s\n", np->r_date);
        }
    }

    if (close(fd) < 0)
    {
        printf("Couldn't close input file");
        return 1;
    }
    if (fclose(fd_out) < 0)
    {
        printf("Couldn't close output file");
        return 1;
    }

    printf("Report is generated at: [%s]\n", report);
    return 0;
}
