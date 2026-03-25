#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TRESHOLD_SIZE 100000

void merge(int *arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int i = 0; i < n2; i++)
        R[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j])
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];

    free(L);
    free(R);
}

void merge_sort(int *arr, int left, int right) {
    if (left < right) {
        int mid = (left + right)/2;

        if (right - left < TRESHOLD_SIZE){
            merge_sort(arr, left, mid);
            merge_sort(arr, mid + 1, right);
        }
        else {
            #pragma omp task
            merge_sort(arr, left, mid);
            #pragma omp task
            merge_sort(arr, mid + 1, right);
        }
        #pragma omp taskwait
        merge(arr, left, mid, right);
    }
}

int partition(int *arr, int low, int high) {
    int pivot = arr[high]; // pivot simples

    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
        }
    }

    int tmp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = tmp;

    return i + 1;
}

void quick_sort(int *arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        #pragma omp task
        quick_sort(arr, low, pi - 1);
        #pragma omp task
        quick_sort(arr, pi + 1, high);
    }
}

double get_time() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

void fill_random(int *arr, int n) {
    for (int i = 0; i < n; i++)
        arr[i] = rand();
}

int check_sort(int *arr, int n){
    for (int i = 0; i < n-1; ++i){
        if (arr[i] > arr[i+1])
            return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <tamanho>\n", argv[0]);
        return 1;
    }
    
    int n = atoi(argv[1]);
    srand(42);

    int *base = (int *)malloc(n * sizeof(int));
    int *arr1 = (int *)malloc(n * sizeof(int));
    int *arr2 = (int *)malloc(n * sizeof(int));

    fill_random(base, n);

    memcpy(arr2, base, n * sizeof(int));
    memcpy(arr1, base, n * sizeof(int));

    // -------- Quick Sort --------
    
    double t1 = get_time();

    #pragma omp parallel
    {
         #pragma omp single
        {
            merge_sort(arr1, 0, n - 1);
        }
    }

    double t2 = get_time();
    printf("Merge Sort: %.6f s\n", t2 - t1);
    
    double t3 = get_time();
    
    #pragma omp parallel
    {
        #pragma omp single
        {
            quick_sort(arr2, 0, n - 1);
        }
    }

    double t4 = get_time();
    printf("Quick Sort: %.6f s\n", t4 - t3);
    
    printf("QS: ");
    if (check_sort(arr2, n))
        puts("=-=-=-=-=-= IT'S SORTED =-=-=-=-=-=");
    else
        puts("=-=-=-=-=-= IT AIN'T SORTED =-=-=-=-=-=");

    printf("MS: ");
    if (check_sort(arr1, n))
        puts("=-=-=-=-=-= IT'S SORTED =-=-=-=-=-=");
    else
        puts("=-=-=-=-=-= IT AIN'T SORTED =-=-=-=-=-=");

    
    printf("Tamanho: %d\n", n);
    
    
    
    /*
    for( int i = 0; i < n; i++ ) {
        printf("%d\n", arr2[i] );
    }*/


    free(base);
    free(arr1);
    free(arr2);

    return 0;
}
