#include "VersionState.h"


VersionState::VersionState(int version, VersionState::State state) {
	set(version, state);
}

void VersionState::set(int version, VersionState::State state) {
	this->store(makeRaw(version, state));
}

VersionState::State VersionState::getState(int raw) {
	int sum = 0;
	
	// Convert the binary number to a base10 int
	for(int i = NUM_STATE_BITS-1; i>=0; i--)
		sum += (1 << i) * (int)(getBit(raw, i));
	
	return (VersionState::State) sum;
}

int VersionState::getVersion(int raw) {
	return raw >> NUM_STATE_BITS;
}
	
// Private helper functions below
int VersionState::makeRaw(int version, VersionState::State state) {
	version = version << NUM_STATE_BITS;
	
	// Make the least-sig bits of version reflect the state
	for(int i = NUM_STATE_BITS-1; i >= 0; i--) 
		version = setBit(version, getBit(state, i), i);
	
	return version;
}

bool VersionState::getBit(int val, int bit) {
	return val & (0x1 << bit);
}

int VersionState::setBit(int num, bool value, int position) {
	if(value) return num | (0x1 << position);
	else return num & ~(0x1 << position);
}

const char *VersionState::getStateString(VersionState::State state) {	
	switch(state) {
		case VersionState::State::BUSY:
			return "\"busy\"";
		case VersionState::State::MEMBER:
			return "\"member\"";
		case VersionState::State::INSERTING:
			return "\"inserting\"";
		case VersionState::State::EMPTY:
			return "\"empty\"";
		case VersionState::State::COLLIDED:
			return "\"collided\"";
		case VersionState::State::VISIBLE:
			return "\"visible\"";
		default:
			return "\"unknown\"";
	}
}