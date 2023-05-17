/*  test machine: csel-kh4250-03.cselabs.umn.edu (ssh)
*   group number: G19
*   name: Timothy Anderson, Luc Rubin, Hank Hanson
*   x500: and09985, rubin206, hans7186 */

#include "myutils.h"

int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        printf("Less number of arguments.\n");
        printf("./master InputFileName\n");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    // Please keep this function call in the main.
    // This function initializes "output" folder
    // by deleting the folder and all its files,
    // and then creating a clean "output" folder.
    bookeepingCode();

    // Parse input file
    char inputFileName[MaxFileNameLength];
    memset(inputFileName, '\0', MaxFileNameLength);
    sprintf(inputFileName, "input/%s", argv[1]);

    FILE *fp;
    if ((fp = getFilePointer(inputFileName)) == NULL) { // Open a file and return file pointer to the file
        printf("ERROR: Process [master] couldn't open file \"%s\"\n", inputFileName);
        exit(EXIT_FAILURE);
    }   

    // Read the number of data and depth
    int nData;
    int depth = 0;
    
    fscanf(fp, "%d %d\n", &nData, &depth); // Using fscanf instead of storing a buffer line

    // Assumption error handling
    if (nData < 1 || nData > MaxDataSize) {
        printf("ERROR: Assumption violation: N. Data (%d) should be between 1 and %d.\n", nData, MaxDataSize);
        exit(EXIT_FAILURE);
    }
    if (depth > MaxDepth) {
        printf("ERROR: Assumption violation: Depth (%d) is greater than %d.\n", depth, MaxDepth);
        exit(EXIT_FAILURE);
    }

    // Read degrees of each level
    int degree = 0;

    char c;
    int leafs, d, i;
    for (i=0, leafs=1; i < depth && c != '\n' && fscanf(fp, "%d%c", &d, &c) != EOF; i++){
        if (i == 0) degree = d; // Saves the first degree for creating the first set of children
        
        // Assumption error handling
        if (d < 1 || d > MaxDegree) {
            printf("ERROR: Assumption violation: Degree (%d) of a depth (%d) should be between 1 and %d.\n", d, i, MaxDegree);
            exit(EXIT_FAILURE);
        }
        leafs *= d;
        if (leafs > nData) {
            printf("ERROR: Assumption violation: N. leaf nodes (%d) is greater than N. Data (%d).\n", leafs, nData);
            exit(EXIT_FAILURE);
        }
    }

    if (!degree) { // Degree of 0 means the process has no children and is a leaf node
        // Read input data (only needs to be done by leaf nodes)
        int *input = (int *)malloc(sizeof(int) * nData);
        int *p = input;
        for (p=input; fscanf(fp, "%d\n", p) != EOF; p++);

        quickSort(input, 0, nData);
        writeSortedResultToFile("master", input, nData);
        printf("Process [master] Quick Sort - Done\n"); 
        
        fclose(fp);
        free(input);
    }
    else {   
        // Spawn child processes and launch childProgram
        fclose(fp);
        for (i=0; i < degree && fork(); i++); // fork() will stop the loop for children processes, the value of i will be used for the ids of the child

        if (i != degree) { // Child process, i will only equal degree on the parent process
            int id = i + 1;
            int dataSize = nData/degree;
            int startIdx = dataSize*i;
            if (id == degree) // The last child assigned
                dataSize = nData - (degree-1) * dataSize;
            int endIdx = startIdx+dataSize-1;
            printf("Parent [master] - Spawn Child [%d, %d, %d, %d, %d]\n", 1, id, startIdx, endIdx, dataSize);

            // String array for the integer arguments
            char args[5][MaxDepth+1];
            sprintf(args[0], "%d", 1);
            sprintf(args[1], "%d", id);
            sprintf(args[2], "%d", startIdx);
            sprintf(args[3], "%d", endIdx);
            sprintf(args[4], "%d", dataSize);
            
            execl("childProgram", "childProgram", 
                args[0], args[1], args[2], args[3], args[4], inputFileName, NULL);
            printf("ERROR: process (%d) failed to execute \"childProgram\"", getpid());
            exit(EXIT_FAILURE);
        }
        
        // Wait for all child processes to terminate
        for (; i > 0; i--) wait(NULL);

        merge("master", 0, degree);
        printf("Process [master] Merge Sort - Done\n");    
    }

    return EXIT_SUCCESS;
}
