#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <thread>
#include <vector>

#include "NBHashTable.h"

#define NUM_TRIALS 500000
#define TABLE_SIZE 1000

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
    
    delete ht;
    return 0;
}

void generator(NBHashTable *ht, int tid)
{
	//The number to push, or remove
    int num;
    
    //Seeds the random number generation
    srand(time(0));
    
    for(int i = 0; i < NUM_TRIALS; i++)
    {
        //Generates a random number
        num = rand() % 100;
        
        switch(rand() % 2)
        {
            case 0:
                if(ht->put(num))
                    printf("PASS: Successfully put %d\n", num);
                else printf("FAIL: Unsuccessfully put %d\n", num);
                break;
            case 1:
                if(ht->contains(num))
                    if(ht->remove(num))
                        printf("PASS: Successfully removed %d\n", num);
                    else printf("FAIL: Unsuccessfully removed %d\n", num);
        }
    }
}
