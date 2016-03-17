#include "ProbeBound.h"

ProbeBound::ProbeBound(int pb, bool scanning) {
	set(pb, scanning);
}

void ProbeBound::set(int pb, bool scanning) {
	this->store(makeRaw(pb, scanning));
}

// Usage:
//    *You're* supposed to use ProbeBound->load to get the raw value.
//    Once you have the raw value, you can get the actual values using these static functions.
//    Ex:
//    ProbeBound p(10);
//    int rawB = p->load();
//    printf("The probe bound is %d\n", ProbeBound::getBound(raw));
//    printf("The scanning bit is %s\n", ProbeBound::isScanning(raw) ? "true" : "false");
int ProbeBound::getBound(int raw) {
	return raw >> 1;
}

bool ProbeBound::isScanning(int raw) {
	int rawBit = raw & 0x1;
	return rawBit == 0x1;
}

// Private functions below

int ProbeBound::makeRaw(int pb, bool scanning) {
	pb = (pb >= 0) ? pb << 1 : 0;
	return setBit(pb, scanning, 0);
}

void ProbeBound::printBits(int ref) {
	// Count with two pointers, one for MSB one for LSB
	// When MSB has been counted down below zero, we're done.
	for(int l=sizeof(int)*8-1; l >= 0; l--) {
		printf("%d", getBit(ref, l) ? 1 : 0);
	}
	
	printf("\n");
}

bool ProbeBound::getBit(int val, int bit) {
	return val & (0x1 << bit);
}


int ProbeBound::setBit(int num, bool value, int position) {
	if(value) return num | (0x1 << position);
	else return num & ~(0x1 << position);
}