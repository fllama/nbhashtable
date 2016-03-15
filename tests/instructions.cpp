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

void fetcher(int, atomic_int *c, NBHashTable *ht, Instruction *arr, int);

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
	
	for(int i = 0; i < numThreads; i++) threads.push_back(thread(fetcher, i, &counter, &ht, ins, numInstructions));
	for(int i = 0; i < numThreads; i++) threads[i].join();
	
	delete ins;
	return 0;
}

void fetcher(int tid, atomic_int *c, NBHashTable *ht, Instruction *arr, int numInstructions) {
	
	while(true) {
		
		int thisIndex = c->fetch_add(1);
		if(thisIndex >= numInstructions) break;
		
		bool boolBuffer;
		switch(arr[thisIndex].type) {
			case Instruction::Type::INSERT:
				printLock.lock();
				boolBuffer = ht->insert(arr[thisIndex].value);
				printf("Thread %d: Inserting %d --- %s\n", tid, arr[thisIndex].value, boolBuffer ? "success" : "failure");
				ht->printHashTableInfo();
				printLock.unlock();
				break;
			case Instruction::Type::CONTAINS:
				printLock.lock();
				boolBuffer = ht->contains(arr[thisIndex].value);
				printf("Thread %d: Checking for %d --- %s\n", tid, arr[thisIndex].value, boolBuffer ? "true" : "false");
				printLock.unlock();
				break;
			case Instruction::Type::DELETE:
				printLock.lock();
				boolBuffer = ht->remove(arr[thisIndex].value);
				printf("Thread %d: Deleting %d --- %s\n", tid, arr[thisIndex].value, boolBuffer ? "success" : "failure");
				ht->printHashTableInfo();
				printLock.unlock();
				break;
			default:
				printLock.lock();
				printf("Thread %d: Unknown instruction\n", tid);
				ht->printHashTableInfo();
				printLock.unlock();
		}
		
	}
}