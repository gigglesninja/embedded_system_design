#include <stdio.h>
#include <stdlib.h>

int values[] = { 0x0CA1, 0x0CA3, 0x0CA5, 0x0CA4};
int values1[] = { 0x0CA7, 0x0CA8, 0x0CA9, 0x0CA1 }; 
int values2[] = { 0x0CA2, 0x0CA3, 0x0CA4, 0x0CA4 };  

int values4[] = { 0x0CA1, 0x0CA3, 0x0CA5, 0x0CA4, 0x0CA7, 0x0CA8, 0x0CA9,
 0x0CA1, 0x0CA2, 0x0CA3, 0x0CA4, 0x0CA4};

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

int main()
{ 

   qsort(values, 4, sizeof(int), cmpfunc);
   qsort(values1, 4, sizeof(int), cmpfunc);
   qsort(values2, 4, sizeof(int), cmpfunc);
  // qsort(values3, 4, sizeof(int), cmpfunc);

   qsort(values4, 16, sizeof(float), cmpfunc);

   int med = (values[1] + values[2]) >> 1;
   int med1 = (values1[1] + values1[2]) >> 1;
   int med2 = (values2[1] + values2[2]) >> 1;
   //int med3 = (values3[1] + values3[2]) >> 1;

   int controlmed = (values4[7] + values4[8]) >> 1;

   med = (med + med1 + med2) >> 3;

   printf("value1 %02x \n", med);
   printf("value2 %02x \n", controlmed);

   return(0);
}