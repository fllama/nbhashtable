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
bool NBHashTable::put(NBType n) {
	return this->insert(n);
}

// bool NBHashTable::contains(int n);
// int NBHashTable::size();

bool NBHashTable::remove(int n)
{
    //The jump index
    int probeJumps;
    
    //Gets the hash
    int hashValue = hash(n);
    
    //Finds the correct bucket
    for(probeJumps = 0; probeJumps <= bounds[hashValue] && *getBucketValue(hashValue, probeJumps) != n; probeJumps++);
    
    //Checks if n was found
    if(probeJumps > bounds[hashValue]) return false;
    
    //Sets the slot to empty
    *getBucketValue(hashValue, probeJumps) = -1;
    
    //Checks if it was the last bound
    if(probeJumps == bounds[hashValue])
        conditionallyLowerBound(hashValue, probeJumps);
        
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
	int hashValue = hash(n);
	for (int probeJumps = 0; probeJumps <= kSize; probeJumps++) {
		int *bucketValue = getBucketValue(hashValue, probeJumps);
		if (*bucketValue == -1) {
			*bucketValue = n;
			conditionallyRaiseBound(hashValue, probeJumps);
			return true;
		}
	}
	return false;

 }

 bool NBHashTable::remove(NBType n) {
    //The jump index
    int probeJumps;
    
    //Gets the hash
    int hashValue = hash(n);
    
    //Finds the correct bucket
    for(probeJumps = 0; probeJumps <= bounds[hashValue] && *getBucketValue(hashValue, probeJumps) != n; probeJumps++);
    
    //Checks if n was found
    if(probeJumps > bounds[hashValue]) return false;
    
    //Sets the slot to empty
    *getBucketValue(hashValue, probeJumps) = -1;
    
    //Checks if it was the last bound
    if(probeJumps == bounds[hashValue])
        conditionallyLowerBound(hashValue, probeJumps);

    return true;
}

