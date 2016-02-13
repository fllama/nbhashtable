int main()
{
    NBHashTable *ht = new NBHashTable();
    
    //Puts three values in
    ht->put(1);
    ht->put(9);
    ht->put(17);
    
    //Removes the "Middle" collision (1, 9, 17) -> (1, , 17)
    if(ht->remove(9))
        std::cout << "PASS: 9 was successfully removed" << std::endl;
    else
        std::cout << "FAIL: 9 was not removed from the table" << std::endl;
    
    //Checks to make sure seventeen will still be found
    if(ht->contains(17))
        std::cout << "PASS: 17 is still in the table" << std::endl;
    else std::cout << "FAIL: 17 was not found in the table" << std::endl;
    
    //Checks to make sure you can remove 
    
    return 0;
}