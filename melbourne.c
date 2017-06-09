#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "link_list.h"

struct element {
	int key;
	int value;
};

/*
 * return number of elements in file
 */

long get_ele_count(char *name) {
	FILE *fp;
	long sz;

	fp = fopen(name, "rb");
	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);
	fclose(fp);
	return (sz / sizeof(struct element));
}

/*
 * read elements in file
 */

void read_file(char *name) {
	int		count;
	FILE		*fp;
	struct element	e;

	fp = fopen(name, "wb");
	if(fp == NULL) {
		perror("read_file");
		exit(1);
	}

	printf("Enter Number of element:\n");
	scanf("%d", &count);

	printf("Enter numbers:\n");
	while(count > 0) {
		scanf("%d %d", &e.key, &e.value);
		fwrite(&e, sizeof(struct element), 1, fp);
		count--;
	}
	fclose(fp);
}


void print_file(char *name) {
	long		count;
	struct element	e;
	FILE		*fp;

	fp = fopen(name, "rb");
	if (fp) {
		while(fread(&e, sizeof(struct element), 1, fp)) {
			printf("%d %d\t", e.key, e.value);
		}
	} else {
		printf("ERROR printing file");
	}
	printf("\n");
	fclose(fp);
}

/*
 * Fill array sequentially and then randomly swap elements within bucket
 */

void shuffle(int array[], int n) {
	int i;
	if(n > 0) {
		for (i = 0; i < n - 1; i++) {
			size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
			int t = array[j];
			array[j] = array[i];
			array[i] = t;
		}
	}
}

void fill_rho(int array[], int n, int size_of_bucket) {
	int i;
	for(i = 0; i < n; i++) {
		array[i] = i;
	}
	for(i = 0; i < n; i = i + size_of_bucket) {
		shuffle(&array[i], size_of_bucket);
	}
	printf("\nrho :\n");
	for(i = 0; i < n; i++) {
		if( i % size_of_bucket == 0)
			printf("  ");
		printf("%d\t", array[i]);
	}
	printf("\n");
}

/*
 * Funtion 	: melbourn_shuffle
 * Description 	: obliviously shuffles elements
 * Input 	:
 * 	input 	: name of input file
 * 	temp 	: name of temporary file to store value obliviously
 * 	rho	: permutation array
 * 	output	: name of output file
 * 	p	: constant value
 * Output :
 * 	it will shuffle contens according to rho and store it in output file.
 */

void melbourn_shuffle(char *input, char *temp, int rho[], char *output, int p) {
	FILE			*ifp, *ofp, *tfp;
	int			input_size, i, j, num, element_per_bucket, k;
	long			buckets;
	struct element		*bucketM, *t, *clean_rev_bucket, ele;
	struct list_head*	rev_bucket;
	int			max_elems, idT;
	int			write_location, list_k, list_v;

	input_size =  get_ele_count(input);

	ifp = fopen(input, "rb");
	tfp = fopen(temp, "wb");

	/* calculate various sizes */

	buckets = sqrt(input_size);				// step 1
	element_per_bucket = sqrt(input_size);			// step 2
	max_elems = p * log(input_size);

	bucketM = (struct element *) malloc (element_per_bucket * sizeof(struct element));	// allocate bucket to hold input from file
	rev_bucket = (struct list_head *) malloc (sizeof(struct list_head) * sqrt(input_size)); // allocate sqrt(input_size) number of link list heads to hold rev_buckets

	/*
	 * Distribution phase
	 */

	for(i = 0; i < buckets; i++) {									// setp 4
		for(k = 0; k < sqrt(input_size); k++) {							// Initializing link list
			list_init(&rev_bucket[k]);
		}
		fseek (ifp , i * sizeof(struct element) * element_per_bucket , SEEK_SET);		// step 5
		fread(bucketM, sizeof(struct element), element_per_bucket, ifp);			// step 5

		for(j = 0; j < element_per_bucket; j++) {						// step 7
			idT = rho[bucketM[j].key] / (int) sqrt(input_size);				// step 9
			list_add(&rev_bucket[idT], bucketM[j].key, bucketM[j].value);			// step 10
		}
		for(k = 0; k < buckets; k++) {
			if(rev_bucket[k].size > max_elems) {									// step 14
				printf("\nERROR : rho moves more than plog(n) elements from a bucket of I to a bucket of T");	// step 15
				break;
			}
			while(rev_bucket[k].size < max_elems) {
				list_add(&rev_bucket[k], -1, -1);					// step 18
			}

			/*
			 *  seek to approprite location ( i.e. within each bucket at ith position )
			 */

			write_location = k * sizeof(struct element) * max_elems * (int) sqrt(input_size) + i * max_elems * sizeof(struct element);	// step 20
			fseek (tfp , write_location, SEEK_SET);												// step 20

			/*
			 * DEBUG:
			 * You can print rev_buckets using list_print()
			 * printf("Printing rev buckets \n");
			 * list_print(&rev_bucket[k]);
			 */

			while(rev_bucket[k].size != 0 ) {												// step 20
				list_remove(&rev_bucket[k], &list_k, &list_v);
				ele.key = list_k;
				ele.value = list_v;

				/*
				 * Here we assume write adjust file pointer to next location
				 */

				fwrite(&ele, sizeof(struct element), 1, tfp);										// step 20
			}
		}
	}

	free(rev_bucket);
	fclose(ifp);
	fclose(tfp);

	/* 
	 * DEBUG:
	 * You can ise print_file() to print files contents
	 * printf("\n Printing TEMP file\n");
	 * print_file("tmp");
	 */

	/*
	 * Clean up phase
	 */

	tfp = fopen(temp, "rb");
	ofp = fopen(output, "wb");

	clean_rev_bucket = (struct element *) malloc (max_elems * sizeof(struct element) * (int) sqrt(input_size));
	for(i = 0; i < buckets; i++) {											// step 24
		fread(clean_rev_bucket, sizeof(struct element), max_elems * (int) sqrt(input_size), tfp);		// step 25
		for(j = 0; j <  max_elems * (int) sqrt(input_size); j++) {

			/*
			 * Skip dummy element identified by -1
			 */

			if(clean_rev_bucket[j].key == -1) {
				continue;										// step 27								
			}

			/*
			 * sort element within bucket according to rho
			 */

			idT = rho[clean_rev_bucket[j].key] % (int) sqrt(input_size);					// step 28
			bucketM[idT].key = clean_rev_bucket[j].key;
			bucketM[idT].value = clean_rev_bucket[j].value;
		}
		fwrite(bucketM, sizeof(struct element), element_per_bucket, ofp);					// step 29
	}

	free(bucketM);
	fclose(ofp);
	fclose(tfp);
}

int main(int argc, char **argv) {
	char *input = "input", *output = "output";
	int input_size;
	int *rho;

	read_file(input);
	input_size =  get_ele_count(input);
	printf("\nInput File\n");
	print_file(input);

	rho = (int *) malloc(sizeof(int) *  input_size);
	fill_rho(rho,  input_size, sqrt(input_size));

	melbourn_shuffle(input, "tmp", rho, output, 2);
	free(rho);

	printf("\nOutput File\n");
	print_file(output);


}
