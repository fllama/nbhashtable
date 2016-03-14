// This program can read in a file with instructions, 
// and dispatch them to threads in that order.
// Input file format:
// --------------------------------------
// [numThreads] [tableSize]
// [numInstructions]
// ["ins" | "del" | "con"] [number]
// ["ins" | "del" | "con"] [number]     
// ... (repeat numInstructions times)
// ["ins" | "del" | "con"] [number]

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>

#include "NBHashTable.h"

typedef struct instr {
	
	enum Type {
		INSERT,
		CONTAINS,
		DELETE
	};
	
	Type type;
	int value;
	
} Instruction;

using namespace std;

mutex printLock;

int fetcher(NBHashTable &ht, atomic_int &count, Instruction *ins);

int main() {
	
	int numThreads, tableSize, numInstructions;
	cin >> numThreads >> tableSize >> numInstructions;
	
	NBHashTable ht(tableSize);
	
	// Generate our instructions
	string buffer;
	Instruction *ins = new Instruction[numInstructions];
	for(int i = 0; i < numInstructions; i++) {
		int n;
		cin >> buffer >> n;
		if(buffer == "ins") {
			ins[i].type = Instruction::Type::INSERT;
			ins[i].value = n;
		} else if(buffer == "del") {
			ins[i].type = Instruction::Type::DELETE;
			ins[i].value = n;
		} else if(buffer == "con") {
			ins[i].type = Instruction::Type::CONTAINS;
			ins[i].value = n;
		} else {
			continue;
		}
	}
	
	atomic_int counter;
	counter.store(0);
	vector<thread> threads;
	
	for(int i = 0; i < numThreads; i++) threads.push_back(thread(fetcher, ht, counter, ins));
	for(int i = 0; i < numThreads; i++) threads[i].join();
	
	
	delete ins;
	return 0;
}

int fetcher(NBHashTable &ht, atomic_int &count, Instruction *ins) {
	printLock.lock();
	printf("Count is %d\n", count.fetch_add(1));
	printLock.unlock();
}