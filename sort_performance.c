/*
------------------------------------------------------------
ENVIRONMENT / HOW TO COMPILE AND RUN
------------------------------------------------------------

Language:
    C

Compiler:
    GCC (GNU Compiler Collection)

Linux / macOS:
    gcc -std=c11 -O0 sort_performance.c -o lab1
    ./lab1

Windows using MinGW GCC:
    gcc -std=c11 -O0 sort_performance.c -o lab1.exe
    lab1.exe

Notes:
    -O0 is used so the compiler does not heavily optimize the Bubble Sort
    and change the purpose of the experiment.

Purpose:
    Compare an unoptimized sorting algorithm (Bubble Sort)
    with an optimized sorting algorithm (Merge Sort).

Serial portion:
    Generate the random dataset in memory.

Measurements:
    T_unoptimized              = data generation + Bubble Sort
    T_processing_unoptimized   = Bubble Sort only

    T_optimized                = data generation + Merge Sort
    T_processing_optimized     = Merge Sort only

Amdahl's Law calculations:
    Measured Overall Speedup =
        T_unoptimized / T_optimized

    P =
        T_processing_unoptimized / T_unoptimized

    S =
        T_processing_unoptimized / T_processing_optimized

    Theoretical Overall Speedup =
        1 / ((1 - P) + (P / S))
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_SIZE 30000
#define RANDOM_SEED 12345

/* Generate random integers for the dataset. */
void generateData(int arr[], int size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        arr[i] = rand() % 100000;
    }
}

/* Unoptimized algorithm: Bubble Sort. */
void bubbleSort(int arr[], int size)
{
    int i;
    int j;
    int temp;

    for (i = 0; i < size - 1; i++)
    {
        for (j = 0; j < size - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

/* Merge two sorted portions of the array. */
void merge(int arr[], int left, int middle, int right)
{
    int n1 = middle - left + 1;
    int n2 = right - middle;

    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    int i;
    int j;
    int k;

    if (L == NULL || R == NULL)
    {
        printf("Memory allocation failed.\n");
        free(L);
        free(R);
        exit(1);
    }

    for (i = 0; i < n1; i++)
    {
        L[i] = arr[left + i];
    }

    for (j = 0; j < n2; j++)
    {
        R[j] = arr[middle + 1 + j];
    }

    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }

        k++;
    }

    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

/* Optimized algorithm: Merge Sort. */
void mergeSort(int arr[], int left, int right)
{
    int middle;

    if (left < right)
    {
        middle = left + (right - left) / 2;

        mergeSort(arr, left, middle);
        mergeSort(arr, middle + 1, right);

        merge(arr, left, middle, right);
    }
}

/* Check that the result is actually sorted correctly. */
int isSorted(const int arr[], int size)
{
    int i;

    for (i = 1; i < size; i++)
    {
        if (arr[i - 1] > arr[i])
        {
            return 0;
        }
    }

    return 1;
}

int main(void)
{
    int *data1;
    int *data2;

    clock_t totalStart1;
    clock_t processingStart1;
    clock_t processingEnd1;
    clock_t totalEnd1;

    clock_t totalStart2;
    clock_t processingStart2;
    clock_t processingEnd2;
    clock_t totalEnd2;

    double T_unoptimized;
    double T_processing_unoptimized;
    double T_optimized;
    double T_processing_optimized;

    double measuredSpeedup;
    double P;
    double S;
    double theoreticalSpeedup;

    data1 = (int *)malloc(DATA_SIZE * sizeof(int));
    data2 = (int *)malloc(DATA_SIZE * sizeof(int));

    if (data1 == NULL || data2 == NULL)
    {
        printf("Memory allocation failed.\n");
        free(data1);
        free(data2);
        return 1;
    }

    /*
     * UNOPTIMIZED VERSION
     *
     * Reset to the same fixed seed so both algorithms receive
     * the same sequence of pseudo-random values.
     */
    srand(RANDOM_SEED);

    totalStart1 = clock();

    generateData(data1, DATA_SIZE);

    processingStart1 = clock();

    bubbleSort(data1, DATA_SIZE);

    processingEnd1 = clock();

    totalEnd1 = clock();

    T_unoptimized =
        (double)(totalEnd1 - totalStart1) / CLOCKS_PER_SEC;

    T_processing_unoptimized =
        (double)(processingEnd1 - processingStart1) / CLOCKS_PER_SEC;

    /*
     * OPTIMIZED VERSION
     *
     * Use the exact same seed again. Therefore generateData()
     * creates the same sequence of values for Merge Sort.
     */
    srand(RANDOM_SEED);

    totalStart2 = clock();

    generateData(data2, DATA_SIZE);

    processingStart2 = clock();

    mergeSort(data2, 0, DATA_SIZE - 1);

    processingEnd2 = clock();

    totalEnd2 = clock();

    T_optimized =
        (double)(totalEnd2 - totalStart2) / CLOCKS_PER_SEC;

    T_processing_optimized =
        (double)(processingEnd2 - processingStart2) / CLOCKS_PER_SEC;

    /*
     * Calculate the values required for Amdahl's Law.
     */
    measuredSpeedup = T_unoptimized / T_optimized;

    P = T_processing_unoptimized / T_unoptimized;

    S = T_processing_unoptimized / T_processing_optimized;

    theoreticalSpeedup =
        1.0 / ((1.0 - P) + (P / S));

    /*
     * Print experiment results.
     */
    printf("============================================\n");
    printf("CSE 3320 Lab 1 - Amdahl's Law\n");
    printf("Bubble Sort vs. Merge Sort\n");
    printf("============================================\n");

    printf("Dataset size: %d integers\n", DATA_SIZE);
    printf("Random seed: %d\n\n", RANDOM_SEED);

    printf("UNOPTIMIZED VERSION (Bubble Sort)\n");
    printf("--------------------------------------------\n");
    printf("T_unoptimized = %.6f seconds\n",
           T_unoptimized);
    printf("T_processing_unoptimized = %.6f seconds\n\n",
           T_processing_unoptimized);

    printf("OPTIMIZED VERSION (Merge Sort)\n");
    printf("--------------------------------------------\n");
    printf("T_optimized = %.6f seconds\n",
           T_optimized);
    printf("T_processing_optimized = %.6f seconds\n\n",
           T_processing_optimized);

    printf("AMDahl's LAW RESULTS\n");
    printf("--------------------------------------------\n");
    printf("Measured Overall Speedup = %.6f\n",
           measuredSpeedup);
    printf("P = %.6f\n", P);
    printf("S = %.6f\n", S);
    printf("Theoretical Overall Speedup = %.6f\n\n",
           theoreticalSpeedup);

    printf("CORRECTNESS CHECK\n");
    printf("--------------------------------------------\n");
    printf("Bubble Sort result: %s\n",
           isSorted(data1, DATA_SIZE) ? "Sorted correctly" : "ERROR");
    printf("Merge Sort result:  %s\n",
           isSorted(data2, DATA_SIZE) ? "Sorted correctly" : "ERROR");

    free(data1);
    free(data2);

    return 0;
}
