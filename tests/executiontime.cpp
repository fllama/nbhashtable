#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <thread>
#include <string>

// Finally, include our hash table
#include "NBHashTable.h"

#define NUM_TRIALS 500000
#define TABLE_SIZE 50
#define MAX_NUM    100 // from 1 to MAX_NUM
#define NUM_TESTS  4

#define INSERT_CHANCE  75
#define REMOVE_CHANCE  25
#define CONTAIN_CHANCE 0

int getRandomOperation();
int getRandomInt(int max);

using namespace std;

void generator(NBHashTable *ht, int tid);

int chances[3];

int main(int argc, char **argv) {
	
	// Create our time-keeping objects
	clock_t start, end;
	
	// Seed our C-Style random number generator
	srand(time(0));
	
	vector<thread> threads;
	int numThreads, i, ii;
	double totalTime = 0;
	
	chances[0] = INSERT_CHANCE;
	chances[1] = REMOVE_CHANCE;
	chances[2] = CONTAIN_CHANCE;
	
	// Establish number of threads
	if(argc < 2) {
		printf("Enter a number of threads: ");
		scanf("%d", &numThreads);
	} else {
		numThreads = stoi(argv[1]);
		
		// for each argument after numThreads adjust our chances accordingly
		for(i=2; i < argc; i++) {
			chances[i-2] = stoi(argv[i]);
		}
	}
	
	// If we notice that the chances are not even, terminate the program
	if(chances[0] + chances[1] + chances[2] != 100) {
		printf("Error, chances do not add up to 100%%: %d - %d - %d\n", chances[0], chances[1], chances[2]);
		return 1;
	}
	
	printf("Using %d threads, with...\n", numThreads);
	printf("\t%d%% insertion chance, \n", chances[0]);
	printf("\t%d%% deletion chance, \n", chances[1]);
	printf("\t%d%% contains chance, \n", chances[2]);
	
	// Perform this test multiple times, return the average time.
	for(ii=0; ii < NUM_TESTS; ii++) {
		
		NBHashTable *ht = new NBHashTable(TABLE_SIZE);
		
		start = clock();
		
		// Make a vector of threads
		for(i=0; i < numThreads; i++) {
			threads.push_back(thread(generator, ht, i));
		}
		
		// Join the thread we made to this main() thread, so
		// we're sure that the program wont end before it's done printing.
		for(i=0; i < numThreads; i++) threads[i].join();
		end = clock();
		
		double thisTime = (double) (end-start) / (double)CLOCKS_PER_SEC;
		
		printf("Test #%d took %lf seconds.\n", ii+1, thisTime);
		totalTime += thisTime;
		
		delete ht;
		threads.clear();
	}
	
	printf("Average time after %d tests: %lf\n", NUM_TESTS, totalTime / (double) NUM_TESTS);
	
	return 0;
}

void generator(NBHashTable *ht, int tid) {
	int i;
	
	// Each thread will do 500k operations.
	for(i=0; i < NUM_TRIALS; i++) {
		
		// Generate our random number
		int n = getRandomInt(100);
		
		// Get our random operation (depending on percent chance)
		// then perform the corresponding operation
		switch(getRandomOperation()) {
			case 1:
				ht->insert(n);
				break;
			case 2:
				ht->remove(n);
				break;
			case 3:
				ht->contains(n);
				break;
			default:
				;
		}
	}
}

// Returns some constant integer:
//   1 - enqueue operation
//   2 - delete operation
//   3 - contains operation
int getRandomOperation() {
	int percent = getRandomInt(100);
	if(percent <= chances[0]) return 1;
	else if (percent <= chances[1]) return 2;
	else return 3;
}

// Returns an integer in our range, from [1, max];
int getRandomInt(int max) {
	return (int) (((float) rand() / (float) RAND_MAX) * max + 1);
}