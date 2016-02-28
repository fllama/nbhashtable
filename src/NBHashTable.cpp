#include "NBHashTable.h"

#define DEBUG 0

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

bool NBHashTable::insert(NBType n) {
	mainMutex.lock();
	if (DEBUG) printf("Inserting: %d\n", n);
	int hashValue = hash(n);
	for (int probeJumps = 0; probeJumps <= kSize; probeJumps++) {
		int *bucketValue = getBucketValue(hashValue, probeJumps);
		if (*bucketValue == EMPTY_FLAG) {
			*bucketValue = n;
			conditionallyRaiseBound(hashValue, probeJumps);
			if(DEBUG) printHashTableInfo();
			mainMutex.unlock();
			return true;
		}
	}
	mainMutex.unlock();
	return false;
}

bool NBHashTable::put(NBType n) {
	return this->insert(n);
}

bool NBHashTable::contains(NBType n) {
	mainMutex.lock();
	// Get the hash value for n
	int hashIndex = hash(n), jumps;
	
	// check to see if this value is equal
	for(jumps = 0; jumps <= getProbeBound(hashIndex) && jumps < kSize; jumps++) {
		if(*getBucketValue(hashIndex, jumps) == n) {
			mainMutex.unlock();
			return true;
		}
	}
	
	// If we're here, either we checked too many times or we exceeded our probe bound range
	mainMutex.unlock();
	return false;
}

int NBHashTable::size() {
	mainMutex.lock();
	return kSize;
	mainMutex.unlock();
}

bool NBHashTable::remove(NBType n) {
	//Get the passed values hash index and probe bound
    int hashIndex = hash(n);
    int probeBound = getProbeBound(n);
    
    //Loop through all the probe jumps
    for(int probeJumps; probeJumps < probeBound; probeJumps++){
        
        //Gets the version state for the bucket of the current iteration
        VersionState currentVersionState = getBucketValue(hashIndex, probeJumps);
        
        //Checks to make sure that the key matches and that the bucket is an actual member
        if(getState(currentVersionState) == MEMBER && getBucketValue(hashIndex, probeJumps)->key == n){
            if(atomic_compare_exchange_strong(getBucketValue(hashIndex, probeJumps)->vs,
            setVersionState(currentVersionState.version, MEMBER),
            setVersionState(currentVersionState.version, BUSY))){
                conditionallyLowerBound(hashIndex, probeJumps);
                getBucketValue(hashIndex, probeJumps)->vs = setVersion(currentVersionState.version, EMPTY);
                return true;
            }
        }
    }
    
    return false;
}


// Hash

int NBHashTable::hash(NBType n) {
	return (n % kSize);
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
		while (i > 0 && !doesBucketContainCollision(startIndex,probeJumps)) {
			i--;
		}
		bounds[startIndex] = i;
	}
}

/*

VersionState and ProbeBound types

*/

/*

int NBHashTable::getState(VersionState vs) {
	int msb = sizeof(VersionState)*8 - 1;
	int stateID = 0;
	
	// Start from our max bit size and count down
	for(int bit = 0; bit < NUM_STATE_BITS; bit++) {
		int pow2val = 1 << (NUM_STATE_BITS - bit);
		stateID += pow2val*getBitValue(vs, msb-bit);
	}
	
	return stateID;
}

int NBHashTable::getVersion(VersionState vs);
int NBHashTable::getBound(ProbeBound pb);
bool NBHashTable::getScanning(ProbeBound pb);
VersionState NBHashTable::setState(VersionState vs, int s);
VersionState NBHashTable::setVersion(VersionState vs, int v);
VersionState NBHashTable::setVersionState(int v, int s);
ProbeBound NBHashTable::setScanning(ProbeBound pb, bool s);
ProbeBound NBHashTable::setProbeBound(ProbeBound pb, int p);
ProbeBound NBHashTable::setProbeBound(int p, bool s);

// "bit" must be from 0 to n-1 for an N-bit integer
int NBHashTable::getBitValue(int num, int bit) {
	return num & (1 << bit);
}

*/