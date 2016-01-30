#include <stdio.h>

unsigned int number = 0;
unsigned int bit = 1;

int main(void){
	number |= bit << 0;
	printf("%02x\n", number);
	number |= bit << 1;
	printf("%02x\n", number);
	number |= bit << 2;
	printf("%02x\n", number);
	number |= bit << 3;
	printf("%02x\n", number);
	number |= bit << 4;
	printf("%02x\n", number);
	number |= bit << 5;
	printf("%02x\n", number);
	number |= bit << 6;
	printf("%02x\n", number);
	number |= bit << 7;
	printf("%02x\n", number);
	return 0;
}