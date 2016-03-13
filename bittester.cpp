#include <stdio.h>

void printBits(int ref);
bool getBit(int val, int bit);


int main() {
	
	int myInt = 15;
	
	printf("Enter a number: ");
	scanf("%d", &myInt);
	
	printf("All the bits of %d:\n", myInt);
	printBits(myInt);
	
	return 0;
}


/*


	Bit #:    7 | 6 | 5 | 4 | 3 | 2 | 1 | 0
	Value:    1 | 1 | 1 | 0 | 1 | 0 | 0 | 0
	
*/

// Input: number of bytes, and address to memory
// Output: prints binary representation of that memory
void printBits(int ref) {
	
	// Count with two pointers, one for MSB one for LSB
	// When MSB has been counted down below zero, we're done.
	for(int l=sizeof(int)*8-1; l >= 0; l--) {
		printf("%d", getBit(ref, l) ? 1 : 0);
	}
	
	printf("\n");
}

bool getBit(int val, int bit) {
	return val & (0x1 << bit);
}
