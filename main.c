#include <at89c5131.h>
#include "lcd.h"		//Header file with LCD interfacing functions
#include "serial.c"	//C file with UART interfacing functions
#include <stdio.h> 

bit b0 = 0;              // LSB of state of LFSR

// define global variables 
unsigned char state = 0;  
unsigned char mol_loc = 0; 
unsigned int score = 0;
unsigned int high_score = 0;
unsigned char disp_score[16];
unsigned int inter_count = 0;    // no. of timer 0 interrupts
unsigned char holes[16] = "qwertyuiasdfghjk";               //keys mapped {'q','w','e','r','t','y','u','i','a','s','d','f','g','h','j','k'};
unsigned char game_over_var = 0;	 // works as a flag for indicating if game is over.

//function prototypes	
void init_lfsr(void);
void load_next_state(void);
void start_game(void);	
unsigned char map_to_location(unsigned char key_hit);
void disp_mole(void);
void game_over(void);
void interrupt_timer0(void);	
	
// initialize lfsr with non-zero state and initial location of mole
void init_lfsr(void)
{
	b0 = 1;
	state = 1;
	mol_loc = 1;
}

// We use a Fibonacci maximal 5 bit LFSR for generating PRBS with period 31 , feedback poly = x^5+ x^3 +1 ; refer readme.txt also
void load_next_state(void)
{
  // state = (0 b4 b3 b2 b1) + ((b0^b2) 0 0 0 0) 
	state = (state>>1) + ((b0^ ((state & 0x04)>>2) )<<4);    	
	b0 = (state & 0x01);
	if (state==15)             
	{
		mol_loc = 0;
	}
	else
	{
		mol_loc = state % 16;
	}
}


void start_game(void)
{
	lcd_cmd(0x01);
	lcd_cmd(0x84);
	lcd_write_string("Get Ready");
	msdelay(2000);
	lcd_cmd(0x01);
	
	lcd_cmd(0x83);
	lcd_write_string("********");
	lcd_cmd(0xC3);
	lcd_write_string("********");
	
	msdelay(3000);
	score = 0;              
	inter_count = 0;
	REN = 1;   // enable receiving
	TR0 = 1;  // start timer 0
	
//	state = seed;
}	

// given an argument key_hit which is entered by user , this func returns the corresponding char ie location no. 
//If key is not from the defined holes[] domain, return null char
unsigned char map_to_location(unsigned char key_hit)
{
	unsigned char played_no = 0;
	for (played_no = 0; played_no<16; played_no++)
	{
		if (holes[played_no] == key_hit)
		{
			return played_no;
		}else{
		}
	}
	return "\0";

}
// display mole location on LCD
void disp_mole(void)
{
	unsigned char hole_no = 0;
	lcd_cmd(0x01);
	lcd_cmd(0x83);
	
	while(hole_no < 8)
	{
		if (hole_no == mol_loc)
		{
			lcd_write_string("m");
		}
		else{
			lcd_write_string("*");
		}
		hole_no=hole_no+1;
	}
	
	lcd_cmd(0xC3);
	while(hole_no < 16)
	{
		if (hole_no == mol_loc)
		{
			lcd_write_string("m");
		}
		else{
			lcd_write_string("*");
		}
		hole_no=hole_no+1;
	}
	
}
// This func stops game. Shows score and highscore on LCD. 
void game_over(void)
{
	lcd_cmd(0x01);
	lcd_cmd(0x82);
	lcd_write_string("Game over :D");
	msdelay(2000);
	
	lcd_cmd(0x01);
	lcd_cmd(0x80);
  sprintf(disp_score, "Score: %u", score);
	lcd_write_string(disp_score);
	
	if (score>high_score)
	{
		high_score = score;
	}
	else{
	}
	
	sprintf(disp_score, "High Score: %u", high_score);
	lcd_cmd(0xC0);
	lcd_write_string(disp_score);
	msdelay(3000);
	
	game_over_var = 0;
}
// timer 0 is used as a 10 seconds timer
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
		TR0 = 0;    // stop timer after 10 s
		game_over_var = 1;
		REN = 0;   // stop receiving
	}
	else
	{
		TH0 = 0x00;
		TL0 = 0x00;
	}
}

void main(void)
{
	unsigned char ch = 0;
	unsigned char player_move = 0 ; 
	
	
	//Call initialization functions
	lcd_init();
	uart_init();
	init_lfsr();
	
	// start game 
	start_game();
	disp_mole();
	game_over_var = 0;
	while(1)                // keep playing forever.
	{
		
		 // code to check lsfr sequences
		/* for( ch =0; ch<40; ch++)
		{
			load_next_state();
		}		
		while(1);
		*/
		
		if(game_over_var==1)
		{
			game_over();
			start_game();   // restart game
			disp_mole();
		}
		else{
		}
		
		ch = receive_char(& game_over_var);        
		player_move = map_to_location(ch); 
		
		if (player_move == mol_loc)      
		{
			score = score+1;    
			load_next_state();
			disp_mole(); 
		}
		else
		{
			//do nothing if key does not match 
		}
		
	}
}

