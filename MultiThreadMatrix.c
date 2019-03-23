#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int **A;
int **B;
int **C;
int *rows;
int row;
pthread_mutex_t lock; 

void *calculate(void *args) {
	int i;
	int *size = (int*) args;
	while(row != size[0]) {
		pthread_mutex_lock(&lock);
		i = row;
		row++;
		pthread_mutex_unlock(&lock);
		for(int j=0;j<size[0];j++) {
			for(int k=0;k<size[0];k++) {
				if(A[rows[i]][k] * B[k][j] == 1) {
					C[rows[i]][j] = 1; 
					break;
				}
			}
		}
	}	
	pthread_exit(0);
}

int main() {
	int n;
	int size;
	printf("Podaj rozmiar macierzy: ");
	scanf("%d",&size);
	printf("Podaj liczbe watkow: ");
	scanf("%d",&n);
	pthread_t t[n];
	int tsize[] = {size};
	A = (int **)malloc(size*sizeof(int*));
	B = (int **)malloc(size*sizeof(int*));
	C = (int **)malloc(size*sizeof(int*));
	rows = (int *)malloc(size*sizeof(int));
       	for(int j=0;j<size;j++) {
             	A[j] = (int*)malloc(size*sizeof(int));
		B[j] = (int*)malloc(size*sizeof(int));
		C[j] = (int*)malloc(size*sizeof(int));
	}
	srand(time(NULL));
	for(int i=0;i<size;i++) {
		rows[i] = i;
		for(int j=0;j<size;j++) {
			A[i][j] = rand()%2;
			B[i][j] = rand()%2;
			C[i][j] = 0;
		}
	}
	pthread_mutex_init(&lock,NULL);
	for(int i=0;i<n;i++) {
		pthread_create(&t[i],NULL,*calculate,(void *) tsize);
	}
	for(int i=0;i<n;i++) {
		pthread_join(t[i], NULL);
	}
	for(int i=0;i<size;i++) {
		for(int j=0;j<size;j++) {
			printf("%d ",A[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	for(int i=0;i<size;i++) {
		for(int j=0;j<size;j++) {
			printf("%d ",B[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	for(int i=0;i<size;i++) {
		for(int j=0;j<size;j++) {
			printf("%d ",C[i][j]);
		}
		printf("\n");
	}
	pthread_mutex_destroy(&lock);
	return 0;
}


// gcc -pthread -zad2.c
