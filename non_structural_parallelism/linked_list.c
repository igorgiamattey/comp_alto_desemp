#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct __list {
	int value;
	struct __list * next;
} list;

void process (list * ptr) {
	printf("Waiting %d seconds...\n", ptr->value);
	sleep(ptr->value);
}

list * push_front (list * l, int v) {
	list * ptr = malloc(sizeof(list));
	ptr->value = v;
	ptr->next = l;

	return ptr;
}

int main() {
	
	list * l = NULL;
	l = push_front(l, 5);
	l = push_front(l, 4);
	l = push_front(l, 6);
	l = push_front(l, 1);
	l = push_front(l, 3);
	l = push_front(l, 8);
	l = push_front(l, 4);

	list * ptr = l;
	#pragma omp parallel
	{
		#pragma omp single
		{
			while (ptr != NULL) {
				#pragma omp task
				{
					process(ptr);
				}
				ptr = ptr->next;
			}
		}
	}
}