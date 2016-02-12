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
void NBHashTable::put(NBType n, int threadID) {
	printStream.lock();
	printf("Putting %d, from thread %d!\n", n, threadID);
	printStream.unlock();
}

// bool NBHashTable::contains(int n);
// int NBHashTable::size();
// void NBHashTable::remove(int n);


// Hash

int NBHashTable::hash(int n) {
	return (n % kSize);
}


// Bucket

int* NBHashTable::bucket(int h, int index) {
	return &buckets[(h + (index * (index + 1)) /2) % kSize];
}

bool NBHashTable::doesBucketContainCollision(int h, int index) {
	return (bucket(h,index) != 0 && hash(*bucket(h,index)) == h);
}


// Bounds

void NBHashTable::initProbeBound(int h) {
	bounds[h] = 0;
}

int NBHashTable::getProbeBound(int h) {
	return bounds[h];
}

void NBHashTable::conditionallyRaiseBound(int h, int index) {
	bounds[h] = (h > index) ? h : index;
}

void NBHashTable::conditionallyLowerBound(int h, int index) {
	if (index > 0) {
		int i = index - 1;
		while (i > 0 && -doesBucketContainCollision(h,i)) {
			i--;
		}
		bounds[h] = i;
	}
}


// Public
