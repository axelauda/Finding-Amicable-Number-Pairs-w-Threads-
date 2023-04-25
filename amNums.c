// Alexander Lauderdale
// CS 370 1002
// Assignment 3

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <getopt.h>
#include <string.h>

// This struct holds the thread count and upper limit requested by user.
struct {
	int threads;
	long int limit;
} args;

// Function prototypes for getting arguments, thread function, and printing array.
int getArgs(int argc, char *argv[]);
void * findAmNums();
void printNums();

// Global variables used for the threads to manipulate.
pthread_mutex_t myLock;
long int gloCtr = 0;
int pairArr[250][2] = {{-1}};
int arrCtr = 0;

// Main function
int main(int argc, char *argv[]) {

	unsigned long int mtxErr = pthread_mutex_init(&myLock, NULL);
	unsigned long int threadErr;
// If the mutex init failed, program ends.
	if (mtxErr != 0) {
		printf("Mutex initialization failed.\n");
		exit(0);
	}
// Arguments are parsed from command line and stored in args struct.
	int checkArgs;
	checkArgs = getArgs(argc, argv);
// If any arguments failed, program exits.
	if (checkArgs == -1) {
		exit(0);
	}

// Make the amount of threads requested by user and store in dynamic threadPool.
	pthread_t * threadPool;
	threadPool = malloc(sizeof(pthread_t[args.threads]));

// Print out the header and some data of the program.
	printf("CS 370 - Project #3\nAmicable Numbers Program\n\n");
	printf("Thread Count:   %i \nNumbers Limit:  %li \n\n", args.threads, args.limit);

// Print waiting statement
	printf("Please wait. Running...\n\n");
	
// Thread id holds the id number of the current thread.
	long int thdId[args.threads];

// The array is used so that each thread is passed a unique value from thdId.	
	for (long int i = 0; i < args.threads; i++) {
		thdId[i] = i;
	}

// Until we've reached all numbers up to limit, while loop loops.
	while (gloCtr <= args.limit) {

// Threads are created to check for amicable numbers in parallel.
		for (int i = 0; i < args.threads; i++) {
// The threads are passed a unique value in thdId that serves as their individual "thread number".
			threadErr = pthread_create(&threadPool[i], NULL, &findAmNums, (void *) &thdId[i]);
			if (threadErr != 0) {
				printf("Thread failed to create.\n");
			}
		}

// At the end, the threads are joined back.
		for (int i = 0; i < args.threads; i++) {
			pthread_join(threadPool[i], NULL);
		}
// After all threads are joined, the global counter increments by how many threads were completed.
		gloCtr += args.threads;
	}
// Deallocate any dynamic memory that was used for the thread pool.
	free(threadPool);
	
// Print out the result and then exit.
	printf("Amicable Numbers\n");
	printNums();
// The program ends.
	exit(0);
}

// Function to obtain the arguments from command line.
int getArgs(int argc, char *argv[]) {

	char o;
	char* ptr = NULL;
	int gotThread = 0;
	int gotLimit = 0;

// Print how to use program if no options were written.
	if (argc == 1) {
		printf("Usage: ./amNums -t <threadCount> -l <limitValue>\n");
		return -1;
	}
// Use switch statement to get all options and numbers from command line.
	while ((o = getopt(argc, argv, "t:l:")) != -1) {
		switch(o) {

// The number of threads after option "-t" is stored in args struct.
			case 't':
				args.threads = strtol(optarg, &ptr, 10);
				if (*ptr) {
					printf("Error, invalid thread count value.\n");
					return -1;
				}
				gotThread = 1;
				break;
// The upper limit after option "-l" is stored in args struct.
			case 'l':
				args.limit = strtol(optarg, &ptr, 10);
				if (*ptr) {
					printf("Error, invalid limit value.\n");
					return -1;
				}
				gotLimit = 1;
				break;
		}
	}
// If no thread was retrieved, then the thread specifier was incorrect.
	if (!gotThread) {
		printf("Error, invalid thread count specifier.\n");
		return -1;
// If no limit was retrieved, then the limit specifier was incorrect.
	} else if (!gotLimit) {
		printf("Error, invalid limit value specifier.\n");
		return -1;
	}
// Check if threads are within acceptable range, otherwise error.
	if (args.threads < 1 || args.threads > 64) {
		printf("Error, thread count must be >= 1 and <= 64.\n");
		return -1;
	}
// Check if upper limit is 100 or above, otherwise error.
	if (args.limit < 100) {
		printf("Error, limit must be > 64.\n");
		return -1;
	}
// If all arguments were successful, return a 0;
	return 0;
}

// Algorithm used in threads to find amicable numbers.
void * findAmNums(void * thdNum) {

// Thread id is passed to the function.
	long int * tId = (long int *) (thdNum);
	long int oldNum;
	long int num;
	int remaining;
	int divSum = 0;

// Current number to be checked by this thread is based on gloCtr plus its id.
	num = gloCtr + *tId;
// This ensures that no thread does the same number, and gloCtr isn't updated until after threads join.	

// If the number is greater than the args limit, then no reason to check past limit.
	if (num > args.limit) {
		return NULL;
	}
	
// Algorithm checks to efficiently find all the divisors of some number.
	remaining = num;
	for (int i = 1; i <= remaining; ++i) {
// If remaining equals i, then it's already in the sum, so break out of loop.
		if (remaining == i) {
			break;
		}

// If i is a divisor of num, both i and num/i, which is also a divisor, is added to sum.
		if (num % i == 0) {
			divSum += i;

// The remaining values to be checked for divisibility of num are shrunk to save time.
			remaining = num/i;

// If i times itself makes num, then we don't add it to wtice to the sum.
			if (i != (num/i) && (num/i) != num) {
				divSum += num/i;
			}
		}
	}

// We repeat the algorithm above, accept we check if the result above also
// results in the original num.
    oldNum = num;
    num = divSum;
	remaining = divSum;
	divSum = 0;
	for (int i = 1; i <= remaining; ++i) {
		if (remaining == i) {
			break;
		}
		if (num % i == 0) {
			divSum += i;
			remaining = num/i;
			if (i != (num/i) && (num/i) != num) {
				divSum += num/i;
			}
		}
	}

// If amicable number is found and it's not a repeat, we initiate a lock and 
// add it to the amicable num array.
	if (oldNum == divSum && oldNum < num) {
		pthread_mutex_lock(&myLock);
			pairArr[arrCtr][1] = oldNum;
			pairArr[arrCtr][2] = num;
			arrCtr++;
		pthread_mutex_unlock(&myLock);
	}

// Thread returns.
	return NULL;
}

// Function prints out the contents of amicable num array.
void printNums() {

// All pairs stored in the array are outputted to screen.
	for (int i = 0; i < arrCtr; i++) {
		printf("%*d", 12, pairArr[i][1]);
		printf("%*d\n", 12, pairArr[i][2]);
	}

// The count of pairs found are also outputted to screen.
	printf("\n");
	printf("Count of amicable number pairs from 1 to %li is %i\n", args.limit, arrCtr);
}
