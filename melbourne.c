#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>

long get_ele_count(char *name) {
	FILE *fp;
	long sz;

	fp = fopen(name, "rb");
	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);
	fclose(fp);
	return (sz / sizeof(int));
}


void read_file(char *name) {
	int count, num;
	FILE *fp;

	fp = fopen(name, "wb");
	if(fp == NULL) {
		perror("read_file");
		exit(1);
	}

	printf("Enter Number of element:\n");
	scanf("%d", &count);

	printf("Enter numbers:\n");
	while(count > 0) {
		scanf("%d", &num);
		fwrite(&num, sizeof(int), 1, fp);
		count--;
	}
	fclose(fp);
}


void print_file(char *name) {
	long count;
	int num;
	FILE *fp;

	fp = fopen(name, "rb");
	if (fp) {
		while(fread(&num, sizeof(int), 1, fp)) {
			printf(" %d", num);
		}
	} else {
		printf("ERROR printing file");
	}
	printf("\n");
	fclose(fp);
}

/*
 * Fill array sequentially and then randomly swap elements
 */

void fill_rho(int array[], int n) {
	int i;
	for(i = 0; i < n; i++) {
		array[i] = i;
	}
	if (n > 1)
	{
		for (i = 0; i < n - 1; i++) {
			size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
			int t = array[j];
			array[j] = array[i];
			array[i] = t;
		}
	}
	printf("\nrho :");
	for(i = 0; i < n; i++) {
		printf("%d ", array[i]);
	}
	printf("\n");
}

void melbourn_shuffle(char *input, char *temp, int rho[], char *output) {
	FILE	*ifp, *ofp, *tfp;
	int	input_size, i;
	long	buckets;

	input_size =  get_ele_count(input);
	ifp = fopen(input, "rb");
	ofp = fopen(output, "wb");
	tfp = fopen(temp, "wb");

	buckets = sqrt(input_size);

	for(i = 0; i < buckets; i++) {
		printf("WORKING on bucket : %d", i);
	}

	fclose(ifp);
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
	fill_rho(rho,  input_size);
	free(rho);
}
