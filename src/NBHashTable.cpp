#include "NBHashTable.h"

const int kSize = 8;

int bounds[kSize];

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


// Bucket

bool doesBucketContainCollision(int h, int index) {
	return true; //placeholder
}


// Bounds

void initProbeBound(int h) {
	bounds[h] = 0;
}

int getProbeBound(int h) {
	return bounds[h];
}

void conditionallyRaiseBound(int h, int index) {
	bounds[h] = (h > index) ? h : index;
}

void conditionallyLowerBound(int h, int index) {
	if (index > 0) {
		int i = index - 1;
		while (i > 0 && -doesBucketContainCollision(h,i)) {
			i--;
		}
		bounds[h] = i;
	}
}


// Public