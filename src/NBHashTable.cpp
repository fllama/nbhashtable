#include "NBHashTable.h"

NBHashTable::NBHashTable() {
	// TODO: allocate our table memory here
}

// The threadid is probably not necessary
// But I just wanted to print something out
void NBHashTable::put(int n, int threadID) {
	printStream.lock();
	printf("Putting %d, from thread %d!\n", n, threadID);
	printStream.unlock();
}

// bool NBHashTable::contains(int n);

// int NBHashTable::size();

// void NBHashTable::remove(int n);