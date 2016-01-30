#include <stdio.h>
#include <stdlib.h>


void main(void){
	unsigned int format = 0x0C;
	unsigned int sample = 0x0800;
	unsigned int vref = 3300;
	unsigned int max = 0x0FFF;

	unsigned int result = (vref * sample) >> format;

	printf("AVG A 0x%04x \n",result);
}