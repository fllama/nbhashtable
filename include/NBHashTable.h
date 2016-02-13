#ifndef NBHASHTABLE_H
#define NBHASHTABLE_H

#include <cstdio>
#include <mutex>
#define EMPTY_FLAG -1

// This typedef exists solely to assist in future code understanding
// If we ever wish to add support for multiple types, rather than just ints
// We can immediately distinguish which types we should be editing.
// We could also make our template class variable exactly the same, to minimize code adjustments.
typedef int NBType;

class NBHashTable {
	
	std::mutex printStream;
	std::mutex mainMutex;
	int kSize, *bounds, *buckets;
	
	int* getBucketValue(int startIndex, int probeJumps);
	bool doesBucketContainCollision(int startIndex, int probeJumps);
	void initProbeBound(int startIndex);
	int getProbeBound(int startIndex);
	void conditionallyRaiseBound(int startIndex, int probeJumps);
	void conditionallyLowerBound(int startIndex, int probeJumps);
	int hash(NBType n);
	
	public:
		NBHashTable(int ks = 8);
		~NBHashTable();
		bool insert(NBType n);
		bool put(NBType n);
		bool remove(NBType n);
		// bool lookup(int);
		bool contains(int);
		int size();
};

#endif