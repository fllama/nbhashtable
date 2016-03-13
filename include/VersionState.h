#ifndef VERSIONSTATE_H
#define VERSIONSTATE_H

#include <atomic>
#include <cstdio>

#define NUM_STATE_BITS 3

// Possible states of each VersionState

class VersionState : public std::atomic_int {
	
	public:
		
		enum State
		{
			BUSY,
			MEMBER,
			INSERTING,
			EMPTY,
			COLLIDED,
			VISIBLE
		};
		
		VersionState(int version, VersionState::State state);
		
		
		VersionState::State getState();
		int getVersion();
		static VersionState::State getState(int raw);
		static int getVersion(int raw);
		
		void set(int version, VersionState::State state);
		void setVersion(int version);
		void setState(VersionState::State state);
};

#endif