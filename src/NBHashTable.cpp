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

bool NBHashTable::insert(NBType k) {
	int hashIndex = hash(k);
	int i = -1;

	int version;
	VersionState::State state;

	while (true) {
		if (++i >= kSize) {
			return false;
		}
		int vs = getBucketValue(hashIndex, i)->vs->load();
		version = VersionState::getVersion(vs);
		state = VersionState::getState(vs);

		VersionState *cmpVsPointer = new VersionState(version,VersionState::State::EMPTY);
		int cmpVs = cmpVsPointer->load();
		VersionState *newVsPointer = new VersionState(version,VersionState::State::BUSY);
		int newVs = newVsPointer->load();
		if (getBucketValue(hashIndex, i)->vs->compare_exchange_strong(cmpVs, newVs, std::memory_order_release, std::memory_order_relaxed)) {
			break;
		}
	}
	getBucketValue(hashIndex, i)->key = k;
	while (true) {
		VersionState *visibleVs = new VersionState(version,VersionState::State::VISIBLE);
		getBucketValue(hashIndex, i)->vs = visibleVs;
		conditionallyRaiseBound(hashIndex, i);
		VersionState *insertingVs = new VersionState(version,VersionState::State::INSERTING);
		getBucketValue(hashIndex, i)->vs = insertingVs;
		bool r = assist(k,hashIndex,i,version);
		VersionState collidedVs(version,VersionState::State::COLLIDED);
		if (getBucketValue(hashIndex, i)->vs->load() != collidedVs) {
			return true;
		}
		if (!r) {
			conditionallyLowerBound(hashIndex, i);
			VersionState *emptyVs = new VersionState(version + 1,VersionState::State::EMPTY);
			getBucketValue(hashIndex, i)->vs = emptyVs;
			return false;
		}
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
	mainMutex.lock();
	return kSize;
	mainMutex.unlock();
}

bool NBHashTable::remove(NBType n) {
	int hashIndex = hash(n);
	int max = getProbeBound(hashIndex);
	for(int jumps = 0; jumps <= max; jumps++) {
		int vs = getBucketValue(hashIndex, jumps)->vs->load();
		int version = VersionState::getVersion(vs);
		VersionState::State state = VersionState::getState(vs);

		if (state == VersionState::State::MEMBER && getBucketValue(hashIndex, jumps)->key == n) {
			VersionState *cmpVsPointer = new VersionState(version,VersionState::State::MEMBER);
			int cmpVs = cmpVsPointer->load();
			VersionState *newVsPointer = new VersionState(version,VersionState::State::BUSY);
			int newVs = newVsPointer->load();

			if (getBucketValue(hashIndex, jumps)->vs->compare_exchange_strong(cmpVs, newVs, std::memory_order_release, std::memory_order_relaxed)) {
				conditionallyLowerBound(hashIndex, jumps);
				VersionState *newVs = new VersionState(version + 1,VersionState::State::EMPTY);
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
	printf("Bounds:\n");
	for (int i = 0; i < kSize; i++) {
		printf("%d\t", ProbeBound::getBound(bounds[i]));
	}
	printf("\nKeys:\n");
	for (int i = 0; i < kSize; i++) {
		printf("%d\t", buckets[i].key);
	}
	printf("\nScanning:\n");
	for (int i = 0; i < kSize; i++) {
		printf("%s\t", ProbeBound::isScanning(bounds[i]) ? "Y" : "N");
	}
	printf("\nState:\n");
	for (int i = 0; i < kSize; i++) {
		printf("%s\t", VersionState::getStateString(VersionState::getState(buckets[i].vs->load())));
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

