#include "NBHashTable.h"

#define DEBUG 0

NBHashTable::NBHashTable(int ks) {
	int i;
	
	kSize = ks;
	buckets = new BucketT[kSize];
	for(i = 0; i < kSize; i++) initProbeBound(i);
	for(i = 0; i < kSize; i++) {
		buckets[i].vs = new VersionState(0,VersionState::State::EMPTY);
		buckets[i].key = EMPTY_FLAG;
	}
	
}

NBHashTable::~NBHashTable() {
	delete bounds;
	delete buckets;
}

bool NBHashTable::insert(NBType n) {
	/*
	mainMutex.lock();
	if (DEBUG) printf("Inserting: %d\n", n);
	int hashValue = hash(n);
	for (int probeJumps = 0; probeJumps <= kSize; probeJumps++) {
		BucketT *bucketValue = getBucketValue(hashValue, probeJumps);
		if (bucketValue->key == EMPTY_FLAG) {
			bucketValue->key = n;
			conditionallyRaiseBound(hashValue, probeJumps);
			if(DEBUG) printHashTableInfo();
			mainMutex.unlock();
			return true;
		}
	}
	mainMutex.unlock();
	*/
	return false;

}

bool NBHashTable::put(NBType n) {
	return this->insert(n);
}

bool NBHashTable::contains(NBType n) {
	/*
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
	*/
	return false;
}

int NBHashTable::size() {
	mainMutex.lock();
	return kSize;
	mainMutex.unlock();
}

bool NBHashTable::remove(NBType n) {
	/*
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
    */
    return false;
}


// Hash

int NBHashTable::hash(NBType n) {
	return (n % kSize);
}


void NBHashTable::printHashTableInfo() {
	/*
	for (int i = 0; i < kSize; i++) {
		printf("%d\t", bounds[i]);
	}
	printf("\n");
	for (int i = 0; i < kSize; i++) {
		printf("%d\t", buckets[i]);
	}
	printf("\n\n");
	*/
}


// Bucket

BucketT* NBHashTable::getBucketValue(int startIndex, int probeJumps) {
	/*
	return &buckets[(startIndex + (probeJumps * (probeJumps + 1)) /2) % kSize];
	*/
	return NULL; // Placeholder - remove
}

bool NBHashTable::doesBucketContainCollision(int startIndex, int probeJumps) {
	/*
	return (*getBucketValue(startIndex,probeJumps) != EMPTY_FLAG && hash(*getBucketValue(startIndex,probeJumps)) == startIndex);
	*/
	return false; // Placeholder - remove
}


// Bounds

void NBHashTable::initProbeBound(int index) {
	//DONE
	bounds[index].setScanning(false);
	bounds[index].setBound(0);
}

int NBHashTable::getProbeBound(int startIndex) {
	// DONE
	return ProbeBound::getBound(bounds[startIndex].load());
}

void NBHashTable::conditionallyRaiseBound(int startIndex, int probeJumps) {
	// DONE
	while (true) {
		int pb = bounds[startIndex].load();
		int oldBound = ProbeBound::getBound(pb);
		bool scanning = ProbeBound::isScanning(pb);

		int newBound = std::max(oldBound, probeJumps);
		ProbeBound *newPbPointer = new ProbeBound(false,newBound);
		int newPb = newPbPointer->load();
		if (bounds[startIndex].compare_exchange_strong(pb, newPb, std::memory_order_release, std::memory_order_relaxed)) {
			return;
		}
	}

}

void NBHashTable::conditionallyLowerBound(int startIndex, int probeJumps) {
	// DONE
	int pb = bounds[startIndex].load();
	int bound = ProbeBound::getBound(pb);
	bool scanning = ProbeBound::isScanning(pb);
	if (scanning) {
		ProbeBound *newPbPointer = new ProbeBound(false,bound);
		int newPb = newPbPointer->load();
		bounds[startIndex].compare_exchange_strong(pb, newPb, std::memory_order_release, std::memory_order_relaxed);
	}
	if (probeJumps > 0) {

		ProbeBound *cmpPbPointer = new ProbeBound(false,probeJumps);
		ProbeBound *newPbPointer = new ProbeBound(true,probeJumps);
		int cmpPb = cmpPbPointer->load();
		int newPb = newPbPointer->load();
		while (bounds[startIndex].compare_exchange_strong(cmpPb, newPb, std::memory_order_release, std::memory_order_relaxed)) {
			int i = probeJumps - 1;
			while (i > 0 && !doesBucketContainCollision(startIndex,probeJumps)) {
				i--;
			}

			ProbeBound *cmpPbPointer2 = new ProbeBound(true,probeJumps);
			ProbeBound *newPbPointer2 = new ProbeBound(false,i);
			int cmpPb2 = cmpPbPointer->load();
			int newPb2 = newPbPointer->load();
			bounds[startIndex].compare_exchange_strong(cmpPb2, newPb2, std::memory_order_release, std::memory_order_relaxed);
		}
	}
}

/*

VersionState and ProbeBound types

*/



// int NBHashTable::getState(VersionState vs) {
// 	int msb = sizeof(VersionState)*8 - 1;
// 	int stateID = 0;
	
// 	// Start from our max bit size and count down
// 	for(int bit = 0; bit < NUM_STATE_BITS; bit++) {
// 		int pow2val = 1 << (NUM_STATE_BITS - bit);
// 		stateID += pow2val*getBitValue(vs, msb-bit);
// 	}
	
// 	return stateID;
// }

// int NBHashTable::getVersion(VersionState vs);
// int NBHashTable::getBound(ProbeBound pb);
// bool NBHashTable::getScanning(ProbeBound pb);
// VersionState NBHashTable::setState(VersionState vs, int s);
// VersionState NBHashTable::setVersion(VersionState vs, int v);
// std::atomic<VersionState> NBHashTable::setVersionState(int v, int s) {
// 	return NULL; //Placeholder - remove
// }
// ProbeBound NBHashTable::setScanning(ProbeBound pb, bool s);
// ProbeBound NBHashTable::setProbeBound(ProbeBound pb, int p);
// ProbeBound NBHashTable::setProbeBound(int p, bool s);

// // "bit" must be from 0 to n-1 for an N-bit integer
// int NBHashTable::getBitValue(int num, int bit) {
// 	return num & (1 << bit);
// }

