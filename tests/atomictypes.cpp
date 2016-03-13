#include <cstdio>
#include "ProbeBound.h"
#include "VersionState.h"

void printBoundStuff(ProbeBound &pb);
void printBits(int ref);
bool getBit(int val, int bit);
void printBitSetTester();
void printVersionStuff(VersionState &v);

int main() {
	int raw;
	
	// ProbeBound testing
	ProbeBound b(10);
	printBoundStuff(b); // Should be "10, false"
	
	raw = b.load();
	b.set(ProbeBound::getBound(raw)*20, !ProbeBound::isScanning(raw));
	printBoundStuff(b); // Should be "200, true"
	
	b.set(-100, false);
	printBoundStuff(b); // Should be "0, false"
	
	printf("-----------------------------\n");
	
	// VersionState printBitSetTesterting
	VersionState v(1, VersionState::State::BUSY);
	printVersionStuff(v);
	for(int i = 1; i < 12; i++) {
		int raw = v.load();
		v.set(VersionState::getVersion(raw)*2, (VersionState::State)(VersionState::getState(raw)+1));
		printVersionStuff(v);
	}
	
	return 0;
}


void printBoundStuff(ProbeBound &pb) {
	int raw = pb.load();
	printf("Bound is %d, ", ProbeBound::getBound(raw));
	printf("scanning bit is %-7s", ProbeBound::isScanning(raw) ? "\"true\"" : "\"false\"");
	printf("\t--- raw: ");
	ProbeBound::printBits(raw);
}

void printBitSetTester() {
	int zero = 10;
	for(int i = 0; i < 5; i++) {
		printf("%d bit false: ", i);
		ProbeBound::printBits(ProbeBound::setBit(zero, false, i));
		printf(" %d bit true: ", i);
		ProbeBound::printBits(ProbeBound::setBit(zero, true, i));
	}
}

void printVersionStuff(VersionState &v) {
	int raw = v.load();
	printf("Version is %d, ", VersionState::getVersion(raw));
	printf("state is %-12s", VersionState::getStateString(VersionState::getState(raw)));
	printf("\t--- raw: ");
	ProbeBound::printBits(raw);
}