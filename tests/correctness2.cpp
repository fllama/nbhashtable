#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <thread>
#include <vector>

#include "NBHashTable.h"

#define NUM_TRIALS 8
#define TABLE_SIZE 8

using namespace std;

void generator(NBHashTable*, int);

int main()
{
	
	NBHashTable *ht = new NBHashTable(TABLE_SIZE);
	std::vector<std::thread> threads;
	int numThreads, i;
	
	printf("Enter a number of threads: ");
	scanf("%d", &numThreads);
	
	//Populate the vector
	for(i = 0; i < numThreads; i++)
		threads.push_back(std::thread(generator, ht, i));
	
	for(i = 0; i < numThreads; i++) threads[i].join();
	
	ht->printHashTableInfo();
	
	delete ht;
	return 0;
}

void generator(NBHashTable *ht, int tid) {
	
	//The number to push, or remove
	int num;
	
	//Seeds the random number generation
	srand(time(0));
	
	for(int i = 0; i < NUM_TRIALS; i++)
	{
		//Generates a random number
		num = rand() % 100;
		
		if(rand()%2) {
			printf("Removing %d\n", num);
			ht->remove(num);
		} else {
			printf("Putting %d\n", num);
			ht->put(num);
		}
		
	}
}
