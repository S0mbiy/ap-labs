#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

//------------------------------------------------------------------------------
//----------Hash Table from The C Programming Language Book---------------------
//------------------------------------------------------------------------------

struct nlist {              /* table entry: */
    struct nlist *next;     /* next entry in chain */
    char *name;             /* defined name */
    char *i_date;
    char *u_date;
    char *updates;
    char *r_date;
};

#define HASHSIZE 50000

static struct nlist *hashtab[HASHSIZE]; /* pointer table */

/* hash: form hash value for string s */
unsigned hash(char *s)
{
    unsigned hashval;

    for(hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;

    return hashval % HASHSIZE;
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
    } else{
        free((void *) np->u_date);
    }
    char *updates = np->updates;
    int n_updates = atoi(updates);
    char updts[10];
    snprintf(updts, 10, "%d", n_updates+1);
    if ((np->updates = strdup(updts)) == NULL)
        return NULL;
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
    } else{
        free((void *) np->r_date);
    }
    char *updates = np->updates;
    int n_updates = atoi(updates);
    char updts[10];
    snprintf(updts, 10, "%d", n_updates+1);
    if ((np->updates = strdup(updts)) == NULL)
        return NULL;
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
    char* package = malloc(256 * sizeof(char));
    char* date = malloc(16 * sizeof(char));
    struct metapckg *p = {package, date};
	int j = 0, k = 0;
	for (i = 0; origin[i] != '\0'; i++){
        if(found){
            if(origin[i] != ' '){
                package[k++] = origin[i];
            }else{
                package[k] = '\0';
                p->name = package;
                p->date = date;
                return p;
            }
        }
        if(0<i<17){
            date[i-1]=origin[i];
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

void analizeLog(char *logFile, char *report);

int main(int argc, char **argv) {

    if (argc < 2) {
	    printf("Usage:./pacman-analizer.o pacman.log\n");
	    return 1;
    }

    analizeLog(argv[1], REPORT_FILE);

    return 0;
}

void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);
    int fd, j=0, fpos=0;
    int upgr = 0, inst = 0, rem = 0;
    long long int size = 512;
    char* line = malloc(size * sizeof(char) + 1);
    fd = open(logFile, O_RDONLY);
    if(fd < 0){
        printf("Couldn't open log file\n");
        return;
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
            printf("line is: %s\n", line);
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
    printf("Pacman Packages Report\n");
    printf("----------------------\n");
    printf("- Installed packages : %d\n", inst);
    printf("- Removed packages   : %d\n", rem);
    printf("- Upgraded packages  : %d\n", upgr);
    printf("- Current installed  : %d\n", inst-rem);
    printf("List of packages\n");
    printf("----------------\n");
    for (int i = 0; i < HASHSIZE; i++)
    {
        if (strcmp(hashtab[i]->name, "") != 0)
        {
            printf("- Package Name        : %s\n", hashtab[i]->name);
            printf("  - Install date      : %s\n", hashtab[i]->i_date);
            printf("  - Last update date  : %s\n", hashtab[i]->u_date);
            printf("  - How many updates  : %s\n", hashtab[i]->updates);
            printf("  - Removal date      : %s\n", hashtab[i]->r_date);
        }
    }

    if (close(fd) < 0)
    {
        printf("Couldn't close file");
        return;
    }

    printf("Report is generated at: [%s]\n", report);
}
