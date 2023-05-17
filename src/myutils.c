/*  test machine: csel-kh4250-03.cselabs.umn.edu (ssh)
*   group number: G19
*   name: Timothy Anderson, Luc Rubin, Hank Hanson
*   x500: and09985, rubin206, hans7186 */

#include "myutils.h"

void printArray(int arr[], int size) {
    int i;
    for (i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

void writeSortedResultToFile(char* myID, int arr[], int size) {
    char line[WriteBufferSize];
    char filepath[MaxFileNameLength];
    memset(line, '\0', WriteBufferSize);
    memset(filepath, '\0', MaxFileNameLength);
    sprintf(filepath, "output/%s.out", myID);

    int i;
    int idx = 0;
    idx += sprintf(&line[idx], "%d\n", size);
    for (i = 0; i < size; i++) {
        idx += sprintf(&line[idx], "%d\n", arr[i]);
    }
    writeLineToFile(filepath, line);    // Write data in the write buffer to the file
}

// Quick Sort or another leaf node sorting algorithm
int partition(int arr[], int low, int high) {
    int piv = arr[high-1];
    int i = low-1, j;
    for (j=low; j<high-1; j++) {
        if (arr[j] <= piv) {
            int tmp = arr[++i];
            arr[i] = arr[j];
            arr[j] = tmp;
        }
    }
    int tmp = arr[++i];
    arr[i] = arr[j];
    arr[j] = tmp;
    return i;
}

void quickSort(int arr[], int low, int high) {
    if (low < high-1){
        int splt = partition(arr, low, high);
        quickSort(arr, low, splt);
        quickSort(arr, splt, high);
    }
}

// Multiway Merge Sort with multiple data streams from intermediate files
/** This implementation of merge was inspired by an exercise problem from csci4041 Algorithmn and Data Structures.
    Keeping the sorted arrays in a min heap allows us to merge in O(n lg k) time, 
    where n is the total amount of numbers in the final array, and k is the amount of arrays being merged.
**/
int left(int idx) { return (idx+1)*2-1; }
int right(int idx) { return (idx+1)*2; }

void buildHeap(struct MinArr arrs[], int size) {
    int i;
    for (i=(size-1)/2; i>=0; i--) heapify(arrs, size, i);
}

void heapify(struct MinArr arrs[], int size, int idx) {
    int min_idx = idx;
    int l = left(idx), r = right(idx);
    if (l < size && arrs[l].min[0] < arrs[min_idx].min[0])
        min_idx = l;
    if (r < size && arrs[r].min[0] < arrs[min_idx].min[0])
        min_idx = r;
    if (min_idx != idx){
        struct MinArr tmp = arrs[idx];
        arrs[idx] = arrs[min_idx];
        arrs[min_idx] = tmp;
        heapify(arrs, size, min_idx);
    }
}

int extractMin(struct MinArr arrs[], int *size) {
    int min = arrs[0].min[0];
    arrs[0].min++;
    if (arrs[0].min >= arrs[0].end){ // Only removes arrs[i] from the heap if the min pointer reaches the end pointer
        free(arrs[0].arr);
        (*size)--;
        arrs[0] = arrs[*size];
    }
    heapify(arrs, *size, 0);
    return min;
}

void merge(char* myID, int depth, int nChild) {
    struct MinArr arrs[nChild];
    int finalSize = 0;
    
    FILE *fp;                                                  
    char inputFileName[MaxFileNameLength];
    memset(inputFileName, '\0', MaxFileNameLength);
    
    int i;
    for (i=0; i<nChild; i++) {       
        sprintf(inputFileName, "output/%s%d.out", strcmp(myID,"master")?myID:"", i+1);

        if ((fp = getFilePointer(inputFileName)) == NULL) {
            printf("ERROR: Process [%s] couldn't open file \"%s\"\n", myID, inputFileName);
            exit(EXIT_FAILURE);
        }

        int dataSize;
        if (fscanf(fp, "%d\n", &dataSize) == EOF) { // Read data size, as well as makes sure the file isn't empty
            printf("ERROR: Process [%s] failed to read data from file \"%s\"\n", myID, inputFileName);
            exit(EXIT_FAILURE);
        }

        arrs[i].arr = (int *)malloc(sizeof(int) * dataSize);
        arrs[i].min = arrs[i].arr; // Points towards the smallest unused element
        arrs[i].end = arrs[i].arr+dataSize; // Points at the end of the array

        int *p; // Read sorted data
        for (p=arrs[i].arr; p < arrs[i].end && fscanf(fp, "%d\n", p) != EOF; p++);
        
        finalSize += dataSize;      
        fclose(fp);
    }

    int arr[finalSize];
    int heapSize = nChild;

    buildHeap(arrs, heapSize);
    for (i=0; i<finalSize; i++) // Building the final merged array
        arr[i] = extractMin(arrs, &heapSize);

    writeSortedResultToFile(myID, arr, finalSize);
}
