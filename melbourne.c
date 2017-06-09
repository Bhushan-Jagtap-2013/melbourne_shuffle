#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


struct list_element;
struct list_head;
void list_init(struct list_head *h);
void list_add(struct list_head *h, int k, int v);
void list_remove(struct list_head *h, int *k, int *v);
void test_link_list();

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
			printf("%d %d\n", e.key, e.value);
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
	printf("\nrho :");
	for(i = 0; i < n; i++) {
		if( i % size_of_bucket == 0)
			printf("  ");
		printf("%d ", array[i]);
	}
	printf("\n");
}

void melbourn_shuffle(char *input, char *temp, int rho[], char *output, int p) {
	FILE		*ifp, *ofp, *tfp;
	int		input_size, i, j, num, element_per_bucket, k;
	long		buckets;
	struct element	*bucketM, *rev_bucket, *t, *clean_rev_bucket;
	int		max_elems, idT;
	int		write_location;

	input_size =  get_ele_count(input);

	ifp = fopen(input, "rb");
	tfp = fopen(temp, "wb");

	/* calculate various sizes */

	buckets = sqrt(input_size);				// step 1
	element_per_bucket = sqrt(input_size);			// step 2
	max_elems = p * log(input_size);

	bucketM = (struct element *) malloc (element_per_bucket * sizeof(struct element));
	rev_bucket = (struct element *) malloc (max_elems * sizeof(struct element) * sqrt(input_size)); // allocate sqrt(input_size) rev_bucket containing max_elems

	/*
	 * Distribution phase
	 */

	for(i = 0; i < buckets; i++) {								// setp 4
//		printf("\nWORKING on bucket : %d\n", i);
		memset(rev_bucket, 0, max_elems * sizeof(struct element) * sqrt(input_size));

		fseek (ifp , i * sizeof(struct element) * element_per_bucket , SEEK_SET);	// step 5
		fread(bucketM, sizeof(struct element), element_per_bucket, ifp);		// step 5

		for(j = 0; j < element_per_bucket; j++) {					// step 7
//			printf("\nIN LOOP %d %d", bucketM[j].key, bucketM[j].value);
			idT = rho[bucketM[j].key] % (int) sqrt(input_size);			// step 9
//			printf("idT : %d", idT);
			t = rev_bucket + (idT * max_elems);
			t->key = bucketM[j].key;
			t->value = bucketM[j].value;						// step 10
		}
//		for(k = 0 ; k < max_elems * sqrt(input_size); k++) {
//			if(k % max_elems == 0)
//				printf("\n");
//			printf("\nRev_bucket %d %d", rev_bucket[k].key, rev_bucket[k].value);
//		}
		for(k = 0; k < buckets; k++) {
			// fill dummy data if required, but its already a garbage value, so skipping this
			t = rev_bucket + (k * max_elems); 								// select each rev_bucket base address
			write_location = k * sizeof(struct element) * max_elems * (int) sqrt(input_size) + i * max_elems * sizeof(struct element);
//			printf("\nwrite location %d\n", write_location);
			fseek (tfp , write_location, SEEK_SET);								// seek to approprite location ( i.e. within each bucket at ith position )
			fwrite(t, sizeof(struct element), max_elems, tfp);						// write rev_bucket
		}
//		printf("\nprinting TMP\n");
//		print_file("tmp");
	}
	free(rev_bucket);
	fclose(ifp);
	fclose(tfp);

	/*
	 * Clean up phase
	 */

	tfp = fopen(temp, "rb");
	ofp = fopen(output, "wb");

	clean_rev_bucket = (struct element *) malloc (max_elems * sizeof(struct element) * (int) sqrt(input_size));
	for(i = 0; i < buckets; i++) {
		fread(clean_rev_bucket, sizeof(struct element), max_elems * (int) sqrt(input_size), tfp);
		for(j = 0; j < (int) sqrt(input_size); j++) {
			t = clean_rev_bucket + (j * max_elems);
			printf("Final -> %d %d\n", t->key, t-> value);
			idT = rho[t->key] % (int) sqrt(input_size);
			bucketM[idT].key = t->key;
			bucketM[idT].value = t->value;
		}
		fwrite(bucketM, sizeof(struct element), element_per_bucket, ofp);
		printf("\n");
	}


	free(bucketM);

	fclose(ofp);
	fclose(tfp);

	printf("\n Printing OUTPUT\n");
	print_file("output");

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
}

/*
 * This is link list implimentation : YOU CAN IGNORE THIS : FOR TESTING USE TESTING STUB test_link_list
 */

struct list_element {
	int		key;
	int		value;
	struct list_element *next;
};

struct list_head {
	int	size;
	struct list_element *next;
};


/*
 *  simply inisialize link list
 */

void list_init(struct list_head *h) {
	h->size = 0;
	h->next = NULL;
}

/*
 * add element in link list
 * Causion : this funtion assumes link list is initialized before using it.
 */

void list_add(struct list_head *h, int k, int v) {
	struct list_element *temp;
	temp = (struct list_element *) malloc (sizeof(struct list_element));

	// copy value to temp varialbe
	temp->key = k;
	temp->value = v;

	// attach temp variable to start of list
	temp->next = h->next;
	h->next = temp;


	// increment count
	h->size = h->size + 1;
}

/*
 * remove elemet from head of the list  and return its value in k and v
 */

void list_remove(struct list_head *h, int *k, int *v) {
	struct list_element *temp;
	if(h->size != 0) {

		// decrement count
		h->size = h->size - 1;

		// copy value
		*k = h->next->key;
		*v = h->next->value;
		
		// go to next value
		temp = h->next;
		h->next = h->next->next;
		free(temp);
	} else {
		printf("\nINVALIDE OPERATION : UNDERFLOW");	
	}
}

/*
 * funtion to test link list, just add/remove few values
 */

void test_link_list() {
	struct list_head	h;
	int			key, value;

	list_init(&h);


	list_remove(&h, &key, &value);  // expect underflow

	list_add(&h, 10, 10);
	list_add(&h, 20, 20);
	list_add(&h, 30, 30);

	list_remove(&h, &key, &value);
	printf("\nkey : %d, value : %d", key, value);

	list_remove(&h, &key, &value);
	printf("\nkey : %d, value : %d", key, value);

	list_remove(&h, &key, &value);
	printf("\nkey : %d, value : %d", key, value);


	list_remove(&h, &key, &value);  // expect underflow

	list_add(&h, 10, 10);
	list_add(&h, 20, 20);
	list_add(&h, 30, 30);

	list_remove(&h, &key, &value);
	printf("\nkey : %d, value : %d", key, value);

	list_remove(&h, &key, &value);
	printf("\nkey : %d, value : %d", key, value);

	list_remove(&h, &key, &value);
	printf("\nkey : %d, value : %d\n", key, value);
}

/* link list end */


