<!--test machine: csel-kh4250-03.cselabs.umn.edu (ssh)
*   group number: G19
*   name: Timothy Anderson, Luc Rubin, Hank Hanson
*   x500: and09985, rubin206, hans7186 -->

# Group Number 19

## Names and x500 addresses
Timothy Anderson - and09985 \
Luc Rubin - rubin206 \
Hank Hanson - hans7186 

## Individual contributions
We worked on it all together by using GitHub. For example, we mostly did the work on this project on our own, and we used GitHub to push and pull all the work we did. 
At the end, we chose what pieces we wanted to use and put everything together. We thought it was a good idea to work on the project by ourselves 
to the best of our ability so that we can understand everything about this project instead of just assigning portions and working on that one portion.

## Leaf node sorting algorithm used
Quick Sort

## Assumptions
*	Errors from fork() and wait() are ignored as well as the exit status of a child process because an error will be produced during merge when the intermediate files are not found. 
	All intermediate files will be present if all the child processes executed successfully.

*	fscanf() is used for reading from the input file and intermediate files, instead of the provided getLineFromFile(). 
	This was done to avoid storing the data in a temporary buffer, and it assumed that the input file follows the correct format, regardless of errors in the data itself.

*	Only the leaf node process will construct an array from the data in the input file, internal node processes contruct their data from the intermidate files.

*	Only the master process will check for assumption violations

## Compile
	> make clean
	> make

## Execution
	> make run1
	> make run2 
	...
	> make run9