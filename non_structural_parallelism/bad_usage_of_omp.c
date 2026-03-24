/* 
	=== Bad Way to Do It ===
int main() {
	
	#pragma omp parallel for
	for (int i = 0; i < n; i++){

	}
	
	list * ptr = head;
	while (ptr != null) {
		process(ptr);
		ptr = ptr->next;
	}

	int size = 0;
	list * ptr = head;
	while (ptr != null) {
		ptr = ptr->next;
		size++;
	}

	list ** listvet = (list **) malloc(8*size);
	ptr = head;
	int i = 0;
	while (ptr != null){
		list[i] = ptr;
		i++;
		ptr = ptr->next;
	}

	#pragma omp parallel for
	for (int = 0; i < size; i++){
		process(listvet[i];)
	}

	free(listvet);

}
*/