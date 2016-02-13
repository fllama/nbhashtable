#ifndef NBHASHTABLE_H
#define NBHASHTABLE_H

#include <cstdio>
#include <mutex>

// This typedef exists solely to assist in future code understanding
// If we ever wish to add support for multiple types, rather than just ints
// We can immediately distinguish which types we should be editing.
// We could also make our template class variable exactly the same, to minimize code adjustments.
typedef int NBType;

class NBHashTable {
	
	std::mutex printStream;
	std::mutex mainMutex;
	int kSize, *bounds, *buckets;
	
	int bucket(int h, int index);
	bool doesBucketContainCollision(int h, int index);
	void initProbeBound(int h);
	int getProbeBound(int h);
	void conditionallyRaiseBound(int h, int index);
	void conditionallyLowerBound(int h, int index);
	int hash(int n);
	
	public:
		NBHashTable(int ks = 8);
		~NBHashTable();
		void put(NBType);
		// bool lookup(int);
		// bool contains(int);
		// int size();
		// void remove();
};

#endif