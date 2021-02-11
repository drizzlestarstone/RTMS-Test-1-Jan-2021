// CHAN TIAN WEN 25298, 11/2/2021
// EDB4123 TEST ONE
// SET 0NE 

#include <stdio.h>
#include <math.h>
#include "NUC1xx.h"
#include "SYS.h"
#include "GPIO.h"
#include "LCD.h"
#include "Seven_Segment.h"
#include "Scankey.h"

// we define the 7 segments by their corresponding hexadecimal number
#define SEG_A   0xF7 
#define SEG_B   0xEF 
#define SEG_C   0xFE 
#define SEG_D   0xDF 
#define SEG_E   0xBF  
#define SEG_F   0xFB 
#define SEG_OFF	0xFF

unsigned char SEG_TASK2_BUF[6]={SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F}; 

// this function takes in a max 3-digit number and outputs it on the 7-segment display
void Task1(int16_t value)
{
	int8_t digit;
	
	digit = value / 100;
	close_seven_segment();
	show_seven_segment(2,digit);
	DrvSYS_Delay(4500);
	
	value = value - digit * 100;
	digit = value / 10;
	close_seven_segment();
	show_seven_segment(1,digit);
	DrvSYS_Delay(4500);
	
	value = value - digit * 10;
	digit = value;
	close_seven_segment();
	show_seven_segment(0,digit);
	DrvSYS_Delay(4500);
}

//	the existing Seven_Segment.c file supplied by Nuvoton works only to display numbers
//	to display the alphabet segments, we need to create another function to display for MSB

void show_MSB_segment(unsigned char number)
{
  unsigned char temp,i;
	temp=SEG_TASK2_BUF[number];
	
	for(i=0;i<8;i++)
	    {
		if((temp&0x01)==0x01)		   		   
		   DrvGPIO_SetBit(E_GPE,i);
		   else
		   DrvGPIO_ClrBit(E_GPE,i);		  
		   temp=temp>>1;
		}
		DrvGPIO_SetBit(E_GPC,7);	
}

// we display a specified bit on the MSB of the 7-segment
void Task2(int segment_index) 
{
	close_seven_segment();
	
	show_MSB_segment(segment_index);
	DrvSYS_Delay(4500);	
}

void Task(int Task1_counter, int Task2_counter, int speed) // Count from 0 - 100 on 7-segment display
{
	int j=0;
	
	for (j=0; j<speed; j++) {
		Task1(Task1_counter);
		Task2(Task2_counter);
	}
	
	close_seven_segment();
}

int32_t main(void)
{
	int counter1, counter2 = 0;
	int speed = 25;
	char str[20];
	
	UNLOCKREG();			    // unlock protected registers for programming
  DrvSYS_Open(48000000);// set System Clock to run at 48MHz 
	                      // 12MHz crystal input, PLL output 48MHz
	LOCKREG();				    // lock protected registers from programming
	
	init_LCD(); 
	

	while(1) {
			
			// 1st key of keypad to slow down rate of change, 7th key to speed up
			// Multiple presses will increase acceleration
			// Max and min speed capped at 1 < speed < 55
			
			if (ScanKey() == 1) {
				if (speed < 41) {
					speed = speed + 10;
				}
			} else if (ScanKey() == 7) {
				if (speed > 10) {
					speed = speed - 10;
				} 
			}
			
			sprintf(str, "%d", speed);
			clear_LCD();
			print_Line(0, "CHAN TIAN WEN");
			print_Line(1, "25298");
			print_Line(2, "EDB4123 TEST 1");
			print_Line(3, str);
			
			if (counter1 > 100) {
				counter1 = 0;
			}
			
			if (counter2 > 5) {
				counter2 = 0;
			}				
		
			Task(counter1, counter2, speed);			
			counter1++;
			counter2++;
	}
}
