/*
 * author : Bhushan Jagtap
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


/*
 * This is link list implimentation : FOR TESTING USE TESTING STUB test_link_list
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
 *  simply initialize link list
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
	struct list_element	*temp;
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

void list_print(struct list_head *h) {
	struct list_element	*t;
	printf("Size of list : %d\n", h->size);
	t = h->next;
	while(t != NULL) {
		printf("%d %d\n", t->key, t->value);
		t = t->next;
	}
}
