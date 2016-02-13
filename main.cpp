#include <cstdio>
#include <vector>
#include <thread>
#include <string>

// Finally, include our hash table
#include "NBHashTable.h"

using namespace std;

void generator(NBHashTable *ht, int tid);

int main(int argc, char **argv) {
	
	NBHashTable *ht = new NBHashTable(8);
	vector<thread> threads;
	int numThreads, i;
	
	// Establish number of threads
	if(argc < 2) {
		printf("Enter a number of threads: ");
		scanf("%d", &numThreads);
	} else {
		numThreads = stoi(argv[1]);
	}
	
	printf("Using %d threads...\n", numThreads);
	
	// Make a vector of threads
	for(i=0; i < numThreads; i++) {
		threads.push_back(thread(generator, ht, i));
	}
	
	// Join the thread we made to this main() thread, so
	// we're sure that the program wont end before it's done printing.
	for(i=0; i < numThreads; i++) threads[i].join();
	
	
	
	delete ht;
	return 0;
}

void generator(NBHashTable *ht, int tid) {
	int num = 1 << tid; // A neat way to represent 2^n :)
	
	// lock my hash table access
	ht->put(num);
	// unlock the lock
	
}
