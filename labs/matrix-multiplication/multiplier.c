#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "logger.h"
#include "multiplier.h"

unsigned NUM_BUFFERS;
long ARRAY_SIZE;
long L;
long * result;
pthread_mutex_t *mutexes;
long **buffers;

int main(int argc, char **argv){
	int res = -1;
	if (argc == 3 && strcmp(argv[1],"-n")==0 && (NUM_BUFFERS = atoi(argv[2])) > 1){
		if((NUM_BUFFERS%2)!=0){
			errorf("NUM_BUFFERS must be an even number");
			return 1;
		}
		long *mat1;
		mat1 = readMatrix("matA.dat");
		long *mat2;
		mat2 = readMatrix("matB.dat");
		multiply(mat1, mat2);
		res = saveResultMatrix(result);
		free(mat1);
		free(mat2);
		free(result);
		free(buffers);
		free(mutexes);
	}
	else{
		errorf("Usage: ./multipler -n <NUM_BUFFERS>");
	}
    return res;
}

long * readMatrix(char *filename){
	long *matrix = (long *)malloc(sizeof(long));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	fp = fopen(filename, "r");
	if (fp == NULL){
		errorf("File %s doesn't exist", filename);
		return NULL;
	}
	long i = 0;
	while ((read = getline(&line, &len, fp)) != -1) {
		if (line[read - 1] == '\n')
	    {
	        line[read - 1] = '\0';
	        --read;
	    }
		matrix = realloc(matrix, (i+1) * sizeof(long));
		*(matrix + i++) = atoi(line);
		// infof("%d", atoi(line));
   }
   fclose(fp);
   ARRAY_SIZE = i;
   L = (long)sqrt((long double)ARRAY_SIZE);
   return matrix;
}

long * getColumn(int col, long *matrix){
	long *matCol = (long *)malloc(L * sizeof(long));
	for(int i=0; i<L; i++){
		*(matCol + i) = *(matrix + (L * i) + col);
	}
	return matCol;
}

long * getRow(int row, long *matrix){
	long *matRow = (long *)malloc(L * sizeof(long));
	for(int i=0; i<L; i++){
		*(matRow + i) = *(matrix + (L * row) + i);
	}
	return matRow;
}

int getLock(){
	for(int b = 0; b<NUM_BUFFERS; b++){
		if (pthread_mutex_trylock((mutexes + b)) == 0){
			return b;
		}
	}
	return -1;
}

int releaseLock(int lock){
	if(pthread_mutex_unlock((mutexes + lock)) == 0){
		return 0;
	}
	return -1;
}

long dotProduct(long *vec1, long *vec2){
	long res = 0;
	for(int i = 0; i<L; i++){
		res += *(vec1 + i) * *(vec2 + i);
	}
	return res;
}

struct args {
	long l;
    int a;
    int b;
};

void cleanup_handler(void *arg){
	struct args* tmp = ((struct args*)arg);
    if(releaseLock(tmp->a))
    errorf("MUTEX couldn't be released");
    if(releaseLock(tmp->b))
    errorf("MUTEX couldn't be released");
}

void *storeResult(void *arg){
	struct args* tmp = ((struct args*)arg);
	*(result + tmp->l) = dotProduct(*(buffers + tmp->a), *(buffers + tmp->b));
	pthread_cleanup_push(cleanup_handler, arg);
	pthread_exit(NULL);
	pthread_cleanup_pop(1);
	return NULL;
}

void multiply(long *matA, long *matB){
	result = (long *)malloc(ARRAY_SIZE * sizeof(long));
	initBuffers();
    int rc;
	unsigned num_threads = NUM_BUFFERS/2;
	pthread_t threads[num_threads];
	int a, b;
	long * row;
	long * col;
	struct args **arg = (struct args **)malloc(num_threads * sizeof(struct args *));
	for(int j=0; j<num_threads; j++){
		*(arg + j) = (struct args *)malloc(sizeof(struct args));
	}
    for(long l=0; l<ARRAY_SIZE; l++){
		while((a = getLock())==-1);
		while((b = getLock())==-1);
		if(l>num_threads-1)
			pthread_join(threads[l%num_threads], NULL);
		row = getRow(l/L, matA);
		col = getColumn(l%L, matB);
		*(buffers + a) = row;
		*(buffers + b) = col;
		(*(arg + l%num_threads))->l = l;
		(*(arg + l%num_threads))->a = a;
		(*(arg + l%num_threads))->b = b;
        rc = pthread_create(&threads[l%num_threads], NULL, storeResult, (void *)(*(arg + l%num_threads)));
        if (rc){
        	errorf("ERROR in thread, return code from pthread_create() is %d\n", rc);
        }
		free(row);
		free(col);
    }

	for(int i=0; i<num_threads; i++)
	{
		pthread_join(threads[i], NULL);
	}
	for(int i=0; i<NUM_BUFFERS; i++)
	{
		pthread_mutex_destroy((mutexes + i));
	}

}

void initBuffers(){
	buffers = (long **)malloc(NUM_BUFFERS * sizeof(long *));
	mutexes = (pthread_mutex_t *)malloc(NUM_BUFFERS * sizeof(pthread_mutex_t));
	for(int b = 0; b<NUM_BUFFERS; b++){
		*(buffers + b) = (long *)malloc(L*sizeof(long));
		pthread_mutex_init((mutexes + b), NULL);
	}
}

int saveResultMatrix(long *result){
	FILE *fp = fopen("result.dat", "w");
	if (fp == NULL){
		errorf("Can't create result file");
		return -1;
	}
	for(int i = 0; i<ARRAY_SIZE; i++){
		fprintf(fp, "%ld\n", *(result + i));
    }
    fclose(fp);
    return 0;
}
