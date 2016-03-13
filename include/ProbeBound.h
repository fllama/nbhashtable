#ifndef PROBEBOUND_H
#define PROBEBOUND_H

#include <atomic>


class ProbeBound : public std::atomic_int {
	
	public:
		ProbeBound(int pb, bool scanning = false);
		void set (int pb, bool scanning);
		void setScanning(bool scanning);
		void setBound(int pb);
		int getBound();
		bool isScanning();
};


#endif