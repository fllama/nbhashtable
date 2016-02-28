#ifndef NBHASHTABLE_H
#define NBHASHTABLE_H

#include <cstdio>
#include <mutex>
#define EMPTY_FLAG -1
#define NUM_STATE_BITS 3

// This typedef exists solely to assist in future code understanding
// If we ever wish to add support for multiple types, rather than just ints
// We can immediately distinguish which types we should be editing.
// We could also make our template class variable exactly the same, to minimize code adjustments.
typedef int NBType;

// These are our two values that will be atomically swapped
// We'll write methods that accept
typedef int VersionState;
typedef int ProbeBound;

class NBHashTable {
	
	std::mutex mainMutex;
	int kSize, *bounds, *buckets;
	
	int* getBucketValue(int startIndex, int probeJumps);
	bool doesBucketContainCollision(int startIndex, int probeJumps);
	void initProbeBound(int startIndex);
	int getProbeBound(int startIndex);
	void conditionallyRaiseBound(int startIndex, int probeJumps);
	void conditionallyLowerBound(int startIndex, int probeJumps);
	int hash(NBType n);
	void printHashTableInfo();
	
	// Combined Types, getters and setters
	int getState(VersionState vs);
	int getVersion(VersionState vs);
	int getBound(ProbeBound pb);
	bool getScanning(ProbeBound pb);
	VersionState setState(VersionState vs, int s);
	VersionState setVersion(VersionState vs, int v);
	VersionState setVersionState(int v, int s);
	ProbeBound setScanning(ProbeBound pb, bool s);
	ProbeBound setProbeBound(ProbeBound pb, int p); // Same as setBound, just another name
	ProbeBound setProbeBound(int p, bool s);
	int getBitValue(int num, int bit);
	
	
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