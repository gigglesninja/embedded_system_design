#include <stdio.h>
#include <stdlib.h>

unsigned int n = 4;

unsigned int avg(unsigned int avg, unsigned int num, unsigned int nu){
	avg = num + (avg * (nu - 1));
	avg = avg / nu;
	return avg;
}

void main(void){
	unsigned int num[4] = {0x0EA3, 0x0FA1, 0x0FA4, 0x0FAF};
	unsigned int avga = 0x0000;
	unsigned int avgb = 0x0000;
	unsigned int sum = 0x0000;
	int i = 0;

	while(i != n){
		avga = avg(avga, num[i], i + 1);
		sum = sum + num[i];
		i++;
	}

	avgb = sum / n;
	printf("AVG A 0x%04x \n",avga);
	printf("AVG B 0x%04x \n",avgb);

}