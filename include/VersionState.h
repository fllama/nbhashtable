#ifndef VERSIONSTATE_H
#define VERSIONSTATE_H

#include <atomic>
#include <cstdio>

#define NUM_STATE_BITS 3 // equal to ceil(log2(number of states))

// Possible states of each VersionState

class VersionState : public std::atomic_int {
	
	public:
		// Declaring this first so everything has access to it
		enum State
		{
			BUSY = 0,
			MEMBER,
			INSERTING,
			EMPTY,
			COLLIDED,
			VISIBLE
		};
	
	private:
		static bool getBit(int, int);
		static int setBit(int, bool, int);
		int makeRaw(int version, VersionState::State state);
	
	public:
		// Constructor / setters
		VersionState(int version, VersionState::State state);
		void set(int version, VersionState::State state);
		
		// Getters
		static VersionState::State getState(int raw);
		static int getVersion(int raw);
		
		static const char *getStateString(VersionState::State state);
};

#endif