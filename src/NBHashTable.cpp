#include "NBHashTable.h"

#define DEBUG 1

NBHashTable::NBHashTable(int ks) {
	int i;
	
	kSize = ks;
	bounds = new int[kSize];
	buckets = new int[kSize];
	for(i = 0; i < kSize; i++) initProbeBound(i);
	for(i = 0; i < kSize; i++) buckets[i] = EMPTY_FLAG;
	
	// TODO: Allocate table for actual data
}

NBHashTable::~NBHashTable() {
	delete bounds;
	delete buckets;
}

// The threadid is probably not necessary
// But I just wanted to print something out
bool NBHashTable::put(NBType n) {
	return this->insert(n);
}

bool NBHashTable::contains(NBType n) {
	// Get the hash value for n
	int hashIndex = hash(n), jumps;
	
	// check to see if this value is equal
	for(jumps = 0; jumps <= getProbeBound(hashIndex) && jumps < kSize; jumps++) {
		if(*getBucketValue(hashIndex, jumps) == n) return true;
	}
	
	// If we're here, either we checked too many times or we exceeded our probe bound range
	return false;
}

int NBHashTable::size() {
	return kSize;
}

bool NBHashTable::remove(NBType n) {
	if (DEBUG) printf("Removing: %d\n", n);
	//The jump index
	int probeJumps;
	
	//Gets the hash
	int hashValue = hash(n);
	
	//Finds the correct bucket
	for(probeJumps = 0; probeJumps <= bounds[hashValue] && *getBucketValue(hashValue, probeJumps) != n; probeJumps++);
	
	//Checks if n was found
	if(probeJumps > bounds[hashValue]){
		if (DEBUG) printHashTableInfo();
		return false;
	}
	
	//Sets the slot to empty
	*getBucketValue(hashValue, probeJumps) = EMPTY_FLAG;
	
	//Checks if it was the last bound
	if(probeJumps == bounds[hashValue])
		conditionallyLowerBound(hashValue, probeJumps);

	if (DEBUG) printHashTableInfo();
		
	return true;
}

// Hash

int NBHashTable::hash(NBType n) {
	return (n % kSize);
}


// Bucket

int* NBHashTable::getBucketValue(int startIndex, int probeJumps) {
	return &buckets[(startIndex + (probeJumps * (probeJumps + 1)) /2) % kSize];
}

bool NBHashTable::doesBucketContainCollision(int startIndex, int probeJumps) {
	return (*getBucketValue(startIndex,probeJumps) != EMPTY_FLAG && hash(*getBucketValue(startIndex,probeJumps)) == startIndex);
}


// Bounds

void NBHashTable::initProbeBound(int index) {
	bounds[index] = 0;
}

int NBHashTable::getProbeBound(int startIndex) {
	return bounds[startIndex];
}

void NBHashTable::conditionallyRaiseBound(int startIndex, int probeJumps) {
	bounds[startIndex] = (bounds[startIndex] > probeJumps) ? bounds[startIndex] : probeJumps;
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

bool NBHashTable::insert(NBType n) {
	if (DEBUG) printf("Inserting: %d\n", n);
	int hashValue = hash(n);
	for (int probeJumps = 0; probeJumps <= kSize; probeJumps++) {
		int *bucketValue = getBucketValue(hashValue, probeJumps);
		if (*bucketValue == EMPTY_FLAG) {
			*bucketValue = n;
			conditionallyRaiseBound(hashValue, probeJumps);
			printHashTableInfo();
			return true;
		}
	}
	return false;
}

void NBHashTable::printHashTableInfo() {
	for (int i = 0; i < kSize; i++) {
		printf("%d\t", bounds[i]);
	}
	printf("\n");
	for (int i = 0; i < kSize; i++) {
		printf("%d\t", buckets[i]);
	}
	printf("\n\n");
}