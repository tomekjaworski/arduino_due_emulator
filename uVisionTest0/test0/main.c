
#include "ARMCM3.h" 

int main(void)
{
	SystemInit();
	
	while(1){
		//Let's write some code
		
		volatile int x = 0;
		x = 1;
		
		volatile int y = 1;
		
		y = y + 1;
		
		volatile int z = 0;
		
		z = x + y;
	}
}
