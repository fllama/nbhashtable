#include "VersionState.h"


VersionState::VersionState(int version, VersionState::State state) {
	printf("Making a version state with version %d\n", version);
	switch(state) {
		case VersionState::State::BUSY:
			printf("This is the busy state.\n");
			break;
		case VersionState::State::MEMBER:
			printf("This is the member state.\n");
			break;
		case VersionState::State::INSERTING:
			printf("This is the inserting state.\n");
			break;
		case VersionState::State::EMPTY:
			printf("This is the empty state.\n");
			break;
		case VersionState::State::COLLIDED:
			printf("This is the collided state.\n");
			break;
		case VersionState::State::VISIBLE:
			printf("This is the visible state.\n");
			break;
		default:
			printf("I didn't recognize this state!\n");
	}
}

VersionState::State VersionState::getState() {
	return VersionState::State::BUSY;
}

int VersionState::getVersion() {
	return 0;
}

VersionState::State VersionState::getState(int raw) {
	return VersionState::State::BUSY;
}

int VersionState::getVersion(int raw) {
	return 0;
}

void VersionState::set(int version, VersionState::State state) {
	
}

void VersionState::setVersion(int version) {
	
}

void VersionState::setState(VersionState::State state) {
	
}