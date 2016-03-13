#ifndef VERSIONSTATE_H
#define VERSIONSTATE_H

#include <atomic>

#define NUM_STATE_BITS 3

// Possible states of each VersionState

enum NB_BUCKET_STATE
{
    BUSY, 
    MEMBER,
    INSERTING,
    EMPTY,
    COLLIDED,
    VISIBLE
};

class VersionState : public std::atomic_int {
	
	public:
		VersionState(int version, int state);
		
		int getState(VersionState vs);
		int getVersion(VersionState vs);
		
		void set(int version, int state);
		void setVersion(int version);
		void setState(int state);
};

#endif