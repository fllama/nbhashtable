#ifndef PROBEBOUND_H
#define PROBEBOUND_H

#include <atomic>
#include <cstdio>


class ProbeBound : public std::atomic_int {
	
	static int makeRaw(int pb, bool scanning);
	static bool getBit(int val, int bit);
	
	
	public:
		ProbeBound(int pb = 0, bool scanning = false);
		void set(int pb, bool scanning);
		
		// Getters must input using raw data from myProbeBound.load();
		static int getBound(int raw);
		static bool isScanning(int raw);
		
		// Helpers, public only for testing
		static void printBits(int ref);
		static int setBit(int, bool, int);
};


#endif