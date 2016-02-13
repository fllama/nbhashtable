#include "NBHashTable.h"

NBHashTable::NBHashTable(int ks) {
	int i;
	
	kSize = ks;
	bounds = new int[kSize];
	buckets = new int[kSize];
	for(i = 0; i < kSize; i++) initProbeBound(i);
	
	// TODO: Allocate table for actual data
}

NBHashTable::~NBHashTable() {
	delete bounds;
	delete buckets;
}

// The threadid is probably not necessary
// But I just wanted to print something out
void NBHashTable::put(NBType n) {
	printStream.lock();
	printf("Putting %d!\n", n);
	printStream.unlock();
}

// bool NBHashTable::contains(int n);
// int NBHashTable::size();
// void NBHashTable::remove(int n);


// Hash

int NBHashTable::hash(int value) {
	return (value % kSize);
}


// Bucket
// h is the hash value
// bucket()
int* NBHashTable::getBucketValue(int startIndex, int probeJumps) {
	return &buckets[(startIndex + (probeJumps * (probeJumps + 1)) /2) % kSize];
}

bool NBHashTable::doesBucketContainCollision(int startIndex, int probeJumps) {
	return (getBucketValue(startIndex,probeJumps) != 0 && hash(*getBucketValue(startIndex,probeJumps)) == startIndex);
}


// Bounds

void NBHashTable::initProbeBound(int startIndex) {
	bounds[startIndex] = 0;
}

int NBHashTable::getProbeBound(int startIndex) {
	return bounds[startIndex];
}

void NBHashTable::conditionallyRaiseBound(int startIndex, int probeJumps) {
	bounds[startIndex] = (startIndex > probeJumps) ? startIndex : probeJumps;
}

void NBHashTable::conditionallyLowerBound(int startIndex, int probeJumps) {
	if (probeJumps > 0) {
		int i = probeJumps - 1;
		while (i > 0 && -doesBucketContainCollision(startIndex,probeJumps)) {
			i--;
		}
		bounds[startIndex] = i;
	}
}


// Public
