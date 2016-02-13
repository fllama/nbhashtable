#include <iostream>
#include "NBHashTable.h"

int main()
{
    NBHashTable *ht = new NBHashTable();
    
    //Puts three values in
    if(ht->put(1))
        std::cout << "PASS: 1 put successfully" << std::endl;
    else std::cout << "FAIL: 1 was not put successfully" << std::endl;
    
    if(ht->put(9))
        std::cout << "PASS: 9 put successfully" << std::endl;
    else std::cout << "FAIL: 9 was not put successfully" << std::endl;
    
    if(ht->put(17))
        std::cout << "PASS: 17 put successfully" << std::endl;
    else std::cout << "FAIL: 17 was not put successfully" << std::endl;
    
    if(ht->put(25))
        std::cout << "PASS: 25 put successfully" << std::endl;
    else std::cout << "FAIL: 25 was not put successfully" << std::endl;    
    
    //Removes the "Middle" collision (1, 9, 17) -> (1, , 17)
    if(ht->remove(9))
        std::cout << "PASS: 9 was successfully removed" << std::endl;
    else
        std::cout << "FAIL: 9 was not removed from the table" << std::endl;
    
    //Checks to make sure seventeen will still be found
    if(ht->contains(17))
        std::cout << "PASS: 17 is still in the table" << std::endl;
    else
        std::cout << "FAIL: 17 was not found in the table" << std::endl;
    
    //Checks to make sure you can remove 
    if(ht->remove(17))
        std::cout << "PASS: 17 was successfully removed from the table" << std::endl;
    else
        std::cout << "FAIL: 17 was not removed from the table" << std::endl;
    
    
    //Checks to make sure you can remove 
    if(ht->remove(25))
        std::cout << "PASS: 25 was successfully removed from the table" << std::endl;
    else
        std::cout << "FAIL: 25 was not removed from the table" << std::endl;
    
    
    return 0;
}