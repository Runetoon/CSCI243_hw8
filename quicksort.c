//
//
// C Version: c99
// File: quicksort.c
// quicksort.c is a program that cann quicksort via recursion or threads
//
// @author sfr8408: Sam Rizzico
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

typedef struct{

	int *less;
	int *same;
	int *more;
	size_t less_size;
	size_t same_size;
	size_t more_size;

} Partition;

typedef struct{

	int *data;
	size_t size;
	int *result;
	int threads;

} ThreadData;

/*
 * This function partitions the data when necessary, it does so based on the standard rules of quicksort. Should only be used by sorting functions.
 * @param pivot, the pivot value
 * @param *data, the data thats being partitioned
 * @param size, the size of the int array thats being partitioned
 * @returns, the Partitioned data set
 */
Partition partition(int pivot, int *data, size_t size){

	Partition p;
	p.less = malloc(size * sizeof(int));
	p.same = malloc(size * sizeof(int));
	p.more = malloc(size * sizeof(int));
	p.less_size = p.same_size = p.more_size = 0;

	for (size_t i= 0; i <size; i++){
		if (data[i]<pivot)
			p.less[p.less_size++] = data[i];
		else if (data[i]>pivot)
			p.more[p.more_size++] = data[i];
		else
			p.same[p.same_size++] = data[i];
	}
	return p;
}

/*
 * This is the recursive function that uses the quicksort algorithm
 * @param *data, the data thats being sorted
 * @param size, the size of the int array thats being sorted
 * @returns, the sorted data set
 */
int *quicksort(size_t size, int *data){

	if (size<= 1){
		int *base = malloc(size * sizeof(int));
		for (size_t i =0; i<size; i++)
			base[i] = data[i];
		return base;
	}

	int pivot = data[0];
	Partition p = partition(pivot,data,size);
	int *left = quicksort(p.less_size, p.less);
	int *right = quicksort(p.more_size,p.more);
	int *result = malloc(size * sizeof(int));
	size_t idx = 0;

	memcpy(result +idx, left, p.less_size * sizeof(int));
	idx += p.less_size;
	memcpy(result +idx, p.same, p.same_size * sizeof(int));
	idx += p.same_size;
	memcpy(result +idx, right, p.more_size * sizeof(int));

	free(left);
	free(right);
	free(p.less);
	free(p.same);
	free(p.more);

	return result;

}

/*
 * This is the threaded function that uses the quicksort algorithm.
 * @param *args, the argument that contains a struct with all the values quicksort needs to run,. This includes the data, amd size of the data.
 */
void *quicksort_threaded(void *args){

	ThreadData *t = (ThreadData *)args;
	if (t->size<= 1){
		t->result = malloc(t->size * sizeof(int));
		for (size_t i = 0; i< t->size; i++){
			t->result[i] = t->data[i];
		}
		return NULL;
	}
	int pivot = t->data[0];
	Partition p = partition(pivot, t->data, t->size);
	pthread_t left_thread, right_thread;
	ThreadData left = { p.less, p.less_size,NULL, 0};
	ThreadData right = { p.more, p.more_size,NULL, 0};

	t->threads += 2;
	pthread_create(&left_thread, NULL,quicksort_threaded, &left);
	pthread_create(&right_thread, NULL,quicksort_threaded, &right);
	pthread_join(left_thread,NULL);
	pthread_join(right_thread, NULL);
	t->result = malloc(t->size * sizeof(int));
	size_t idx = 0;

	memcpy(t->result+idx,left.result, left.size * sizeof(int));
	idx += left.size;

	memcpy(t->result +idx,p.same,p.same_size * sizeof(int));
	idx += p.same_size;

	memcpy(t->result+idx, right.result, right.size * sizeof(int));

	free(left.result);
	free(right.result);
	free(p.less);
	free(p.same);
	free(p.more);
	return NULL;
}



int main(int argc, char **argv){

	int print_flag = 0;
	char *filename;

	if (argc == 2){
		filename = argv[1];
	} 
	else if (argc == 3 && strcmp(argv[1], "-p") == 0){
		print_flag = 1;
		filename = argv[2];
	} 
	else{
		fprintf(stderr, "usage: %s [-p] file_of_integers\n", argv[0]);
		return 1;
	}
	FILE *fp = fopen(filename, "r");

	if (!fp){
		perror("fopen");
		return 1;
	}
	int capacity = 100;
	int size = 0;
	int *data = malloc(capacity * sizeof(int));

	while (fscanf(fp, "%d", &data[size]) == 1){
		size++;
		if (size >= capacity){
			capacity *= 2;
			data = realloc(data, capacity * sizeof(int));
		}
	}

	fclose(fp);
	if (print_flag){
		printf("Unsorted list before non-threaded quicksort: ");
		for (int i = 0; i < size; i++){

			printf("%d", data[i]);
			if (i != size - 1) printf(", ");
		}
		printf("\n");
	}

	clock_t start = clock();
	int *sorted1 = quicksort(size, data);
	clock_t end = clock();
	double t1 = (double)(end - start) / CLOCKS_PER_SEC;
	printf("Non-threaded time: %f\n", t1);

	if (print_flag){
		printf("Resulting list: ");
		for (int i = 0; i < size; i++){
			printf("%d", sorted1[i]);
			if (i != size - 1) printf(", ");
		}
		printf("\n");
	}

	if (print_flag){
		printf("Unsorted list before threaded quicksort: ");
		for (int i = 0; i < size; i++){

			printf("%d", data[i]);
			if (i != size - 1) printf(", ");
		}
		printf("\n");
	}

	ThreadData t;
	t.data = data;
	t.size = size;
	t.result = NULL;
	t.threads = 0;
	start = clock();
	pthread_t thread;
	pthread_create(&thread, NULL, quicksort_threaded, &t);
	pthread_join(thread, NULL);
	end = clock();
	double t2 = (double)(end - start) / CLOCKS_PER_SEC;
	printf("Threaded time: %f\n", t2);
	printf("Threads spawned: %d\n", t.threads);

	if (print_flag){

		printf("Resulting list: ");
		for (int i = 0; i < size; i++){
			printf("%d", t.result[i]);
			if (i != size - 1) printf(", ");
		}
		printf("\n");
	}
	free(data);
	free(sorted1);
	free(t.result);
	return 0;

}
