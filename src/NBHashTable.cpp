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
	// DONE
	int hashIndex = hash(n);
	int max = getProbeBound(hashIndex);
	for(int jumps = 0; jumps <= max; jumps++) {
		int vs = getBucketValue(hashIndex, jumps)->vs->load();
		int version = VersionState::getVersion(vs);
		VersionState::State state = VersionState::getState(vs);

		if (state == VersionState::State::MEMBER && getBucketValue(hashIndex, jumps)->key == n) {
			VersionState newVs(version,VersionState::State::MEMBER);
			if (getBucketValue(hashIndex, jumps)->vs->load() == newVs.load()) {
				return true;
			}
		}
	}
	return false;
}

int NBHashTable::size() {
	mainMutex.lock();
	return kSize;
	mainMutex.unlock();
}

bool NBHashTable::remove(NBType n) {
	// int hashIndex = hash(n);
	// int max = getProbeBound(hashIndex);
	// for(int jumps = 0; jumps <= max; jumps++) {
	// 	int vs = getBucketValue(hashIndex, jumps)->vs->load();
	// 	int version = VersionState::getVersion(vs);
	// 	VersionState::State state = VersionState::getState(vs);

	// 	if (state == VersionState::State::MEMBER && getBucketValue(hashIndex, jumps)->key == n) {
	// 		VersionState newVs(version,VersionState::State::MEMBER);
	// 		ProbeBound *newPbPointer = new ProbeBound(false,newBound);
	// 		int newPb = newPbPointer->load();
	// 		if (bounds[startIndex].compare_exchange_strong(pb, newPb, std::memory_order_release, std::memory_order_relaxed)) {
	// 			return;
	// 		}
	// 		if (getBucketValue(hashIndex, jumps)->vs->load() == newVs.load()) {
	// 			return true;
	// 		}
	// 	}
	// }
	// return false;
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
	// DONE
	return &buckets[(startIndex + (probeJumps * (probeJumps + 1)) /2) % kSize];
}

bool NBHashTable::doesBucketContainCollision(int startIndex, int probeJumps) {
	// DONE
	int vs = getBucketValue(startIndex, probeJumps)->vs->load();
	int version1 = VersionState::getVersion(vs);
	VersionState::State state1 = VersionState::getState(vs);
	if (state1 == VersionState::State::VISIBLE || state1 == VersionState::State::INSERTING || state1 == VersionState::State::MEMBER) {
		if (hash(getBucketValue(startIndex, probeJumps)->key) == startIndex) {
				int vs2 = getBucketValue(startIndex, probeJumps)->vs->load();
				int version2 = VersionState::getVersion(vs2);
				VersionState::State state2 = VersionState::getState(vs2);
				if (state2 == VersionState::State::VISIBLE || state2 == VersionState::State::INSERTING || state2 == VersionState::State::MEMBER) {
					if (version1 == version2) {
						return true;
					}
				}
		}
	}
	return false;
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

