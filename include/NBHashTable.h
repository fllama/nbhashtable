#ifndef NBHASHTABLE_H
#define NBHASHTABLE_H

#include <cstdio>
#include <mutex>
#include <atomic>
#include <algorithm> 
#include "ProbeBound.h"
#include "VersionState.h"
#define EMPTY_FLAG -1
//#define NUM_STATE_BITS 3

// This typedef exists solely to assist in future code understanding
// If we ever wish to add support for multiple types, rather than just ints
// We can immediately distinguish which types we should be editing.
// We could also make our template class variable exactly the same, to minimize code adjustments.
typedef int NBType;

// These are our two values that will be atomically swapped
// We'll write methods that accept
//typedef int VersionState;
//typedef int ProbeBound;

typedef struct buckett {
	VersionState *vs;
	NBType key;
} BucketT;

class NBHashTable {
	
	std::mutex mainMutex;
	int kSize;
	ProbeBound *bounds;
	BucketT *buckets;
	
	BucketT* getBucketValue(int startIndex, int probeJumps);
	bool doesBucketContainCollision(int startIndex, int probeJumps);
	void initProbeBound(int startIndex);
	int getProbeBound(int startIndex);
	void conditionallyRaiseBound(int startIndex, int probeJumps);
	void conditionallyLowerBound(int startIndex, int probeJumps);
	int hash(NBType n);
	bool assist(NBType key, int h, int i, int ver_i);
	
	public:
		NBHashTable(int ks = 8);
		~NBHashTable();
		bool insert(NBType n);
		bool put(NBType n);
		bool remove(NBType n);
		bool contains(int);
		int size();
		
		void printHashTableInfo();
};

#endif