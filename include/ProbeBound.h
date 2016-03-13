#ifndef PROBEBOUND_H
#define PROBEBOUND_H

#include <atomic>


class ProbeBound : public std::atomic_int {
	
	public:
		ProbeBound(int pb, bool scanning = false);
		void set (int pb, bool scanning);
		void setScanning(bool scanning);
		void setBound(int pb);
		
		// Getters either take in the "this" value, or from an int
		int getBound();
		bool isScanning();
		
		static int getBound(int raw);
		static bool isScanning(int raw);
};


#endif