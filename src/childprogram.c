/*  test machine: csel-kh4250-03.cselabs.umn.edu (ssh)
*   group number: G19
*   name: Timothy Anderson, Luc Rubin, Hank Hanson
*   x500: and09985, rubin206, hans7186 */

#include "myutils.h"

int main(int argc, char *argv[]) {
    
    if (argc < 7) {
        printf("Less number of arguments.\n");
        printf("./childProgram myDepth parentID startIdx endIdx myDataLen InputFileName\n");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    int myDepth = strtol(argv[1], NULL, 10);
    int myID = strtol(argv[2], NULL, 10);
    char* strMyID = argv[2];
    int myStartIdx = strtol(argv[3], NULL, 10);
    int myEndIdx = strtol(argv[4], NULL, 10);
    int myDataLen = strtol(argv[5], NULL, 10);
    char* inputFileName = argv[6];

    FILE *fp;
    if ((fp = getFilePointer(inputFileName)) == NULL) {
        printf("ERROR: Process [%d] couldn't open file \"%s\"\n", myID, inputFileName);
        exit(EXIT_FAILURE);
    }
    
    fscanf(fp, "%*d %*d\n"); // Skip the first line, no need to read in nData or depth

    // Find degree of next level
    int degree;
    int i; char c;
    for (i=0, c=' '; i < myDepth+1 && c != '\n' && fscanf(fp, "%d%c", &degree, &c) != EOF; i++); // Reads degrees until newline or next depth's degree is read
    if (i < myDepth+1) degree = 0; // If newline was hit before the next depth, we must be at a leaf node

    if (!degree) { // Degree of 0 means the process has no children and is a leaf node
        // Read input data (only needs to be done by leaf nodes)
        int *input = (int *)malloc(sizeof(int) * myDataLen);
        for (i=0; i < myStartIdx && fscanf(fp, "%*d\n") != EOF; i++); // Gets fp's offset to myStartIdx
        for (i=0; i < myDataLen && fscanf(fp, "%d\n", input+i) != EOF; i++); // Starts filling in the array

        quickSort(input, 0, myDataLen);
        writeSortedResultToFile(strMyID, input, myDataLen);
        printf("Process [%d] Quick Sort - Done\n", myID);

        fclose(fp);
        free(input);
    }
    else {
        // Spawn child processes and launch childProgram
        fclose(fp);
        for (i=0; i < degree && fork(); i++); // fork() will stop the loop for children processes, the value of i will be used for the ids of the child

        if (i != degree) { // Child process, i will only equal degree on the parent process
            int id = i + 1;
            int dataSize = myDataLen/degree;
            int startIdx = (dataSize*i)+myStartIdx;
            if (id == degree) // The last child assigned
                dataSize = myDataLen - (degree-1) * dataSize;
            int endIdx = startIdx+dataSize-1;
            printf("Parent [%d] - Spawn Child [%d, %d%d, %d, %d, %d]\n", myID, myDepth+1, myID, id, startIdx, endIdx, dataSize);

            // String array for the integer arguments
            char args[5][MaxDepth+1];
            sprintf(args[0], "%d", myDepth+1);
            sprintf(args[1], "%d%d", myID, id);
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

        merge(strMyID, myDepth, degree);
        printf("Process [%d] Merge Sort - Done\n", myID);
    }

    return EXIT_SUCCESS;
}
