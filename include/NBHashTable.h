#ifndef NBHASHTABLE_H
#define NBHASHTABLE_H

#include <cstdio>
#include <mutex>

class NBHashTable {
	
	std::mutex printStream;
	
	public:
		NBHashTable();
		void put(int, int);
		// bool contains(int);
		// int size();
		// void remove();
};

#endif