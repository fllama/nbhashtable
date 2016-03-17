#include "NBHashTable.h"

#define DEBUG 0

typedef VersionState::State VSTATE;

NBHashTable::NBHashTable(int ks) {
	int i;
	
	kSize = ks;
	buckets = new BucketT[kSize];
	bounds = new ProbeBound[kSize];
	
	for(i = 0; i < kSize; i++) {
		buckets[i].vs = new VersionState(0,VersionState::State::EMPTY);
		buckets[i].key = EMPTY_FLAG;
	}
}

NBHashTable::~NBHashTable() {
	delete bounds;
	delete buckets;
}

bool NBHashTable::insert(NBType k) {
	int hashIndex = hash(k);
	int i = -1;

	int version;
	VersionState::State state;

	while (true) {
		
        //Checks to make sure that the table isn't full
        if (++i >= kSize) {
			return false;
		}
        
        //Gets the current bucket's version state and splits it into its two parts
		int vs = getBucketValue(hashIndex, i)->vs->load();
		version = VersionState::getVersion(vs);
		state = VersionState::getState(vs);

        //Creates the two objects for the comparison
		VersionState cmpVsObj(version,VersionState::State::EMPTY);
		int cmpVs = cmpVsObj.load();
		VersionState newVsObj(version,VersionState::State::BUSY);
		int newVs = newVsObj.load();
        
        //Performs the compare and swap for the state change (EMPTY -> BUSY)
		if (getBucketValue(hashIndex, i)->vs->compare_exchange_strong(cmpVs, newVs, std::memory_order_release, std::memory_order_relaxed)) {
			break;
		}
	}
    
    //Updates the bucket's key value
	getBucketValue(hashIndex, i)->key = k;
	
    while (true) {
		//Sets the bucket's state to VISIBLE
        getBucketValue(hashIndex, i)->vs->set(version, VersionState::State::VISIBLE);
		
        //Updates the bucket's probe bound
        conditionallyRaiseBound(hashIndex, i);
		
        //Updates the bucket's state to INSERTING
        getBucketValue(hashIndex, i)->vs->set(version,VersionState::State::INSERTING);
		
        //Performs the assist operation
        bool r = assist(k,hashIndex,i,version);
		
        //Creates the version state for the comparison operation
        VersionState collidedVs(version,VersionState::State::COLLIDED);
        
        //If the current bucket is not in a collision state return true
		if (getBucketValue(hashIndex, i)->vs->load() != collidedVs.load()) {
			return true;
		}
        
        //If the assist operation returned false, update the bound, increment the version, and empty the bucket
		if (!r) {
            //Update the bound
			conditionallyLowerBound(hashIndex, i);
			
            //Create a new version state with an updated version and empty state
            VersionState emptyVs(version + 1,VersionState::State::EMPTY);
            
            //Update the bucket's version state
			getBucketValue(hashIndex, i)->vs->store(emptyVs);
			return false;
		}
        
        //Increment the version
		version++;

	}


}

bool NBHashTable::put(NBType n) {
	return this->insert(n);
}

bool NBHashTable::contains(NBType k) {
	int hashIndex = hash(k);
	int max = getProbeBound(hashIndex);
	for(int jumps = 0; jumps <= max; jumps++) {
		int vs = getBucketValue(hashIndex, jumps)->vs->load();
		int version = VersionState::getVersion(vs);
		VersionState::State state = VersionState::getState(vs);

		if (state == VersionState::State::MEMBER && getBucketValue(hashIndex, jumps)->key == k) {
			VersionState newVs(version,VersionState::State::MEMBER);
			if (getBucketValue(hashIndex, jumps)->vs->load() == newVs.load()) {
				return true;
			}
		}
	}
	return false;
}

int NBHashTable::size() {
	return kSize;
}

bool NBHashTable::remove(NBType n) {
	int hashIndex = hash(n);
	int max = getProbeBound(hashIndex);
	for(int jumps = 0; jumps <= max; jumps++) {
		int vs = getBucketValue(hashIndex, jumps)->vs->load();
		int version = VersionState::getVersion(vs);
		VersionState::State state = VersionState::getState(vs);

		if (state == VersionState::State::MEMBER && getBucketValue(hashIndex, jumps)->key == n) {
			VersionState cmpVsObj(version,VersionState::State::MEMBER);
			int cmpVs = cmpVsObj.load();
			VersionState newVsObj(version,VersionState::State::BUSY);
			int newVs = newVsObj.load();

			if (getBucketValue(hashIndex, jumps)->vs->compare_exchange_strong(cmpVs, newVs, std::memory_order_release, std::memory_order_relaxed)) {
				conditionallyLowerBound(hashIndex, jumps);
				VersionState newVs(version + 1,VersionState::State::EMPTY);
				getBucketValue(hashIndex, jumps)->vs->store(newVs);
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
	mainMutex.lock();
	printf("Bounds:\t| ");
	for (int i = 0; i < kSize; i++) {
		printf("%-8d | ", ProbeBound::getBound(bounds[i].load()));
	}
	printf("\nKeys:\t| ");
	for (int i = 0; i < kSize; i++) {
		if(buckets[i].key == EMPTY_FLAG) printf("%-8s | ", "-");
		else printf("%-8d | ", buckets[i].key);
	}
	printf("\nScan:\t| ");
	for (int i = 0; i < kSize; i++) {
		printf("%-8s | ", ProbeBound::isScanning(bounds[i].load()) ? "true" : "false");
	}
	printf("\nState:\t| ");
	for (int i = 0; i < kSize; i++) {
		printf("%-8s | ", VersionState::getStateString(VersionState::getState(buckets[i].vs->load())));
	}
	printf("\n\n");
	mainMutex.unlock();
}


// Bucket

BucketT* NBHashTable::getBucketValue(int startIndex, int probeJumps) {
	return &buckets[(startIndex + (probeJumps * (probeJumps + 1)) /2) % kSize];
}

bool NBHashTable::doesBucketContainCollision(int startIndex, int probeJumps) {
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
	bounds[index].set(0, false);
}

int NBHashTable::getProbeBound(int startIndex) {
	return ProbeBound::getBound(bounds[startIndex].load());
}

void NBHashTable::conditionallyRaiseBound(int startIndex, int probeJumps) {
	
	if(DEBUG) {
		mainMutex.lock();
		printf("Conditionally raising bound for bucket[%d], there were %d jumps\n", startIndex, probeJumps);
		mainMutex.unlock();
	}
	
	while (true) {
		int pb = bounds[startIndex].load();
		int oldBound = ProbeBound::getBound(pb);
		bool scanning = ProbeBound::isScanning(pb);

		int newBound = std::max(oldBound, probeJumps);
		ProbeBound newPb(newBound, false);
		
		if (bounds[startIndex].compare_exchange_strong(pb, newPb.load())) {
			return;
		}
	}
}

void NBHashTable::conditionallyLowerBound(int startIndex, int probeJumps) {
	int pb = bounds[startIndex].load();
	int bound = ProbeBound::getBound(pb);
	bool scanning = ProbeBound::isScanning(pb);
	if (scanning) {
		ProbeBound newPbObj(false,bound);
		int newPb = newPbObj.load();
		bounds[startIndex].compare_exchange_strong(pb, newPb, std::memory_order_release, std::memory_order_relaxed);
	}
	if (probeJumps > 0) {

		ProbeBound cmpPbObj(false,probeJumps);
		ProbeBound newPbObj(true,probeJumps);
		int cmpPb = cmpPbObj.load();
		int newPb = newPbObj.load();
		while (bounds[startIndex].compare_exchange_strong(cmpPb, newPb, std::memory_order_release, std::memory_order_relaxed)) {
			int i = probeJumps - 1;
			while (i > 0 && !doesBucketContainCollision(startIndex,probeJumps)) {
				i--;
			}

			ProbeBound cmpPbObj2(true,probeJumps);
			ProbeBound newPbObj2(false,i);
			int cmpPb2 = cmpPbObj2.load();
			int newPb2 = newPbObj2.load();
			bounds[startIndex].compare_exchange_strong(cmpPb2, newPb2, std::memory_order_release, std::memory_order_relaxed);
		}
	}
}

// Try to insert key at index i
// key is the value we're trying to insert
// h is the hash key, or the initial index
// i is the number of jumps away from h we are, so i = 0 on first run
// ver_i is the version at bucket[h,i]
bool NBHashTable::assist(NBType key, int h, int i, int ver_i) {
	int max = getProbeBound(h);
	
	// Look through all the possible number of jumps
	for(int j = 0; j < max; j++) {
		
		// If we're looking at the same index, skip it
		if(i==j) continue;
			
		// Line 91, Fig. 10 -> Get the version/state atomically
		int rawVS = getBucketValue(h, j)->vs->load();
		int ver_j = VersionState::getVersion(rawVS);
		VSTATE state_j = VersionState::getState(rawVS);
		
		// If this state is inserting, and it has the same key we're trying to insert
		if(state_j == VSTATE::INSERTING && getBucketValue(h, j)->key == key) {
			
			// If it was found earlier in the probe sequence
			if(j < i) {
				
				VersionState vsj_ins(ver_j, VSTATE::INSERTING);
				if (getBucketValue(h, j)->vs->load() == vsj_ins.load()) {
					int vsi_ins = VersionState::getRawVS(ver_i, VSTATE::INSERTING);
					getBucketValue(h, i)->vs->compare_exchange_strong(vsi_ins, VersionState::getRawVS(ver_i, VSTATE::COLLIDED), std::memory_order_release, std::memory_order_relaxed);
					return assist(key,h,j,ver_j);
				}
				
			} else {
				
				VersionState vsi_ins(ver_i, VSTATE::INSERTING);
				if(getBucketValue(h, i)->vs->load() == vsi_ins.load()) {
					int vsj_ins = VersionState::getRawVS(ver_j, VSTATE::INSERTING);
					getBucketValue(h,j)->vs->compare_exchange_strong(vsj_ins, VersionState::getRawVS(ver_j, VSTATE::COLLIDED), std::memory_order_release, std::memory_order_relaxed);
				}
			}
		}
		
		// Grab our VersionState again
		rawVS = getBucketValue(h,j)->vs->load();
		ver_j = VersionState::getVersion(rawVS);
		state_j = VersionState::getState(rawVS);
		
		// If we find our value already is a member then exit immediately
		if(state_j == VSTATE::MEMBER && getBucketValue(h, j)->key == key) {
			VersionState vsj_mem(ver_j, VSTATE::MEMBER);
			if(getBucketValue(h, j)->vs->load() == vsj_mem.load()) {
				int vsi_ins = VersionState::getRawVS(ver_i, VSTATE::INSERTING);
				getBucketValue(h, i)->vs->compare_exchange_strong(vsi_ins, VersionState::getRawVS(ver_i, VSTATE::COLLIDED), std::memory_order_release, std::memory_order_relaxed);
				return false;
			}
		}
	}
	
	// If we're here, that means we finally arrived at the index to insert at
	// Now, we can finally mark it as a member!
	int vsi_ins = VersionState::getRawVS(ver_i, VSTATE::INSERTING);
	getBucketValue(h,i)->vs->compare_exchange_strong(vsi_ins, VersionState::getRawVS(ver_i, VSTATE::MEMBER), std::memory_order_release, std::memory_order_relaxed);
	return true;
}
