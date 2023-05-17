#ifndef MYUTILS_H
#define MYUTILS_H

#include "utils.h"
#define debug 0

// You can modify these functions as you need

/**
 * Print all elements in the array (space separation)
 *
 * @param arr   integer array
 * @param line  size of elements in the array
 * @return None
 */
void printArray(int arr[], int size);

/**
 * Write sorted numbers to a file
 *
 * @param myID  process's ID which is a part of output filename
 * @param arr   integer array storing sorted numbers
 * @param size  size of element in the array
 * @return None
 */
void writeSortedResultToFile(char* myID, int arr[], int size);

void quickSort(int arr[], int low, int high);
// Helper functions for quickSort()
int partition(int arr[], int low, int high);

void merge(char* myID, int depth, int nChild);
// Helper functions and struct for merge()
struct MinArr {
    int *arr;
    int *min;
    int *end;
};
int left(int idx); // Returns index of left child node in the binary heap
int right(int idx); // Returns index of right child node in the binary heap
void buildHeap(struct MinArr arrs[], int size); // Constructs a min heap of arrs[] comparing the value at arrs[].min
void heapify(struct MinArr arrs[], int size, int idx); // Checks that the node at the index satisfies the min heap properties, adjusts accordingly
int extractMin(struct MinArr arrs[], int *size); // Extracts the smallest element from arrs[]

#endif //MYUTILS_H
