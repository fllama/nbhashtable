#ifndef VERSIONSTATE_H
#define VERSIONSTATE_H

#include <atomic>


int getState(VersionState vs);
int getVersion(VersionState vs);


class VersionState : std::atomic<int> {
	
};



#endif