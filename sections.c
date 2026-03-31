#include <stdio.h>
#include <unistd.h>
#include <omp.h>

void task (double n){
	int id = omp_get_thread_num();
	printf("Thread %d sleeping for %.2f seconds...\n", id, n);
	sleep(n);
	printf("Thread %d woke up\n", id);
}

int main() {

	#pragma omp parallel num_threads(4)
	{
		#pragma omp sections
		{
			#pragma omp section
			{
				task(1.0);
			}

			#pragma omp section
			{
				task(10.0);
			}

			#pragma omp section
			{
				task(5.0);
			}
			printf("END SECTION, %d\n", omp_get_thread_num());
		}
		printf("END SECTIONS\n");
	}
}