#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int numcmp(char *, char *);


void copyArray(int n, void *A[], int start, int end, void *B[])
{
	int k;
    for(k = start; k < end; k++)
        B[k] = A[k];
}

void merge(int n, void *A[], int start, int mid, int end, void *B[], int (*comp) (void *, void *))
{

    int i = start, j = mid, k;
 
    for (k = start; k < end; k++) {
        if (i < mid && (j >= end || (*comp)(A[i], A[j]) <= 0)) {
            B[k] = A[i];
            i = i + 1;
        } else {
            B[k] = A[j];
            j = j + 1;
        }
    }
}

void split(int n, void *B[], int start, int end, void *A[], int (*comp) (void *, void *))
{
    if(end - start <= 1)                       
        return;                                 

    int mid = (end + start) / 2;              

    split(n, A, start, mid, B, comp);  
    split(n, A, mid, end, B, comp);  
    merge(n, B, start, mid, end, A, comp);
}

void MergeSort(int n, void *A[], void *B[], int (*comp) (void *, void *))
{
    copyArray(n, A, 0, n, B);       
    split(n, B, 0, n, A, comp);   
}

int main()
{
	char *A[]={"2","5","3","2","0"};
	int n = sizeof(A)/sizeof(A[0]);
	char *B[n];
	int numeric = 1;
	MergeSort(n, (void **)A, (void **)B, (int (*)(void*,void*))(numeric ? numcmp : strcmp));
    for (int i =0; i<n; i++)
    	printf("%s,", A[i]);
    printf("\n");
    return 0;
}

int numcmp(char *s1, char *s2)
{
	int v1, v2;
	v1 = atoi(s1);
	v2 = atoi(s2);
	if (v1 < v2)
		return -1;
	else if (v1 > v2)
		return 1;
	else
		return 0; 
}


