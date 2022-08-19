#include <at89c5131.h>
//timers functions 
//for 10 sec game-time .... 305 overflows + 11520 counts required => 0xD300 to be loaded in last cycle.
/*
void config_timer0(void)
{
	TMOD = 0x01;   // timers 0 and 1 in mode 1
	
	EA = 1; // interupt global enable
	ET0 = 1;  // enable timer 0 interupt

	TL0 = 0X00;
	TH0 = 0X00;
	
	TF0 = 0;
}
*/

void interrupt_timer0(void) interrupt 1
{
	inter_count = inter_count+1;
	if (inter_count==305)
	{
		TH0 = 0xD3;
		TL0 = 0x00;
	}
	else if (inter_count==306)
	{
		game_over_flag = 1;
		TR0 = 0;
	}
	else
	{
	}
}