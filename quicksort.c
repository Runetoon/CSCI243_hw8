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


Partition partition(int pivot, const int *data, size_t size){

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

int *quicksort(size_t size, const int *data){

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


