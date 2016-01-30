#include <stdio.h>
#include <stdlib.h>

// overflow happens at 10000!!!!
// only handles from 0 - 9999
unsigned int __hexToValue(unsigned int u16_value){
  unsigned int u16_count;
  unsigned int u16_check;
  u16_count = 0x00;
  while(u16_value > 0x0A){
    u16_count = u16_count + 0x10;
    u16_value = u16_value - 0x0A;
    if(u16_count == 0xA0){
    	u16_count = 0x100;
    }
    // *******************************
    // need to roll over from F0 to 60
    u16_check = u16_count & 0x00F0;
    if(u16_check == 0x00F0){
    	u16_count = u16_count + 0x060;
    }
    // *******************************
    // need to roll over from F00 to 600
    u16_check = u16_count & 0x0F00;
    if(u16_check == 0x0F00){
    	u16_count = u16_count + 0x0600;
    }
  }
  u16_count = u16_count + u16_value;
  // ************************************************
  // CONVERT 0xA
  u16_check = u16_count & 0x0F;
  if(u16_check == 0x0A){
  	u16_count = u16_count + 0x0010;
  	u16_count = u16_count & 0xFFF0;
  }
  // ************************************************

  // ************************************************
  // CONVERT 0xA0 - 0xF0
  // Handles from 200 - 999
  u16_check = u16_count & 0x00F0;
  if(u16_check == 0x00A0){
  	u16_count = u16_count + 0x0100;
  	u16_count = u16_count & 0xFF0F;
  }
  else if(u16_check == 0x00B0){
  	u16_count = u16_count + 0x0100;
  	u16_count = u16_count & 0xFF0F;
  	u16_count = u16_count + 0x0010;
  }
  else if(u16_check == 0x00C0){
  	u16_count = u16_count + 0x0100;
  	u16_count = u16_count & 0xFF0F;
  	u16_count = u16_count + 0x0020;
  }
  else if(u16_check == 0x00D0){
  	u16_count = u16_count + 0x0100;
  	u16_count = u16_count & 0xFF0F;
  	u16_count = u16_count + 0x0030;
  }
  else if(u16_check == 0x00E0){
  	u16_count = u16_count + 0x0100;
  	u16_count = u16_count & 0xFF0F;
  	u16_count = u16_count + 0x0040;
  }
  else if(u16_check == 0x00F0){
  	u16_count = u16_count + 0x0100;
  	u16_count = u16_count & 0xFF0F;
  	u16_count = u16_count + 0x0050;
  }
  // ************************************************
  // CONVERT 0xA00 - 0xF00
  // Handles from 1000 - 9999
   u16_check = u16_count & 0x0F00;
  if(u16_check == 0x0A00){
  	u16_count = u16_count + 0x1000;
  	u16_count = u16_count & 0xF0FF;
  }
  else if(u16_check == 0x0B00){
  	u16_count = u16_count + 0x1000;
  	u16_count = u16_count & 0xF0FF;
  	u16_count = u16_count + 0x0100;
  }
  else if(u16_check == 0x0C00){
  	u16_count = u16_count + 0x1000;
  	u16_count = u16_count & 0xF0FF;
  	u16_count = u16_count + 0x0200;
  }
  else if(u16_check == 0x0D00){
  	u16_count = u16_count + 0x1000;
  	u16_count = u16_count & 0xF0FF;
  	u16_count = u16_count + 0x0300;
  }
  else if(u16_check == 0x0E00){
  	u16_count = u16_count + 0x1000;
  	u16_count = u16_count & 0xF0FF;
  	u16_count = u16_count + 0x0400;
  }
  else if(u16_check == 0x0F00){
  	u16_count = u16_count + 0x1000;
  	u16_count = u16_count & 0xF0FF;
  	u16_count = u16_count + 0x0500;
  }
  return u16_count;
}

void main(void){
	unsigned int value = 100;
	int i = 1;
	printf("5 0x%04x \n",__hexToValue(5));
	printf("60 0x%04x \n",__hexToValue(60));
	printf("64 0x%04x \n",__hexToValue(64));
	while(i < 1982){
		printf("%i 0x%04x \n", value, __hexToValue(value));
		value = value + 5;
		i++;
	}
}