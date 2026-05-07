#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h>

using namespace std;

// Generate random array
void generateArray(vector<int>& arr, int size) {
    arr.resize(size);
    for (int i = 0; i < size; i++)
        arr[i] = rand() % 10000;
}

// Sequential Bubble Sort
long long bss = 0;
void bubbleSortSequential(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            bss++; // count comparison
            if (arr[j] > arr[j+1])
                swap(arr[j], arr[j+1]);
        }
    }
}

// Parallel Bubble Sort (Odd-Even)
long long bsp = 0;
int threadsBubble = 0;

void bubbleSortParallel(vector<int>& arr) {
    int n = arr.size();

    #pragma omp parallel
    {
        #pragma omp single
        threadsBubble = omp_get_num_threads();
    }

    for (int i = 0; i < n; i++) {
        #pragma omp parallel for
        for (int j = (i % 2); j < n - 1; j += 2) {
            #pragma omp atomic
            bsp++;

            if (arr[j] > arr[j+1])
                swap(arr[j], arr[j+1]);
        }
    }
}


// Merge
long long mergeOps = 0;

void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> temp(right - left + 1);
    int i = left, j = mid+1, k = 0;

    while (i <= mid && j <= right) {
        mergeOps++;
        temp[k++] = (arr[i] < arr[j]) ? arr[i++] : arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];

    for (int i = 0; i < k; i++)
        arr[left + i] = temp[i];
}

// Sequential Merge Sort
long long mss = 0;

void mergeSortSequential(vector<int>& arr, int left, int right) {
    if (left < right) {
        mss++;
        int mid = (left + right)/2;
        mergeSortSequential(arr, left, mid);
        mergeSortSequential(arr, mid+1, right);
        merge(arr, left, mid, right);
    }
}

// Parallel Merge Sort
long long msp = 0;
int threadsMerge = 0;

void mergeSortParallel(vector<int>& arr, int left, int right, int depth = 0) {
    if (left < right) {
        msp++;
        int mid = (left + right)/2;

        if (depth == 0) {
            #pragma omp parallel
            {
                #pragma omp single
                threadsMerge = omp_get_num_threads();
            }
        }

        if (depth < 4) {
            #pragma omp parallel sections
            {
                #pragma omp section
                mergeSortParallel(arr, left, mid, depth+1);

                #pragma omp section
                mergeSortParallel(arr, mid+1, right, depth+1);
            }
        } else {
            mergeSortSequential(arr, left, mid);
            mergeSortSequential(arr, mid+1, right);
        }

        merge(arr, left, mid, right);
    }
}


int main() {
    const int SIZE = 5000;
    vector<int> original;
    generateArray(original, SIZE);

    vector<int> a1 = original, a2 = original;
    vector<int> a3 = original, a4 = original;

    double t1, t2;

    // Sequential Bubble
    t1 = omp_get_wtime();
    bubbleSortSequential(a1);
    t2 = omp_get_wtime();
    cout << "\nSequential Bubble Sort Time: " << t2 - t1 << "s";
    cout << "\nOperations: " << bss << endl;

    // Parallel Bubble
    t1 = omp_get_wtime();
    bubbleSortParallel(a2);
    t2 = omp_get_wtime();
    cout << "\nParallel Bubble Sort Time: " << t2 - t1 << "s";
    cout << "\nOperations: " << bsp;
    cout << "\nThreads Used: " << threadsBubble << endl;

    // Sequential Merge
    t1 = omp_get_wtime();
    mergeSortSequential(a3, 0, SIZE-1);
    t2 = omp_get_wtime();
    cout << "\nSequential Merge Sort Time: " << t2 - t1 << "s";
    cout << "\nOperations: " << (mss + mergeOps) << endl;

    // Parallel Merge
    mergeOps = 0; // reset
    t1 = omp_get_wtime();
    mergeSortParallel(a4, 0, SIZE-1);
    t2 = omp_get_wtime();
    cout << "\nParallel Merge Sort Time: " << t2 - t1 << "s";
    cout << "\nOperations: " << (msp + mergeOps);
    cout << "\nThreads Used: " << threadsMerge << endl;

    return 0;
}