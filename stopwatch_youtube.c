/*
 * stopwatch_youtube.c
 *
 * Created: 7/27/2016 10:05:10 PM
 *  Author: Ikramuzzaman Muntasir
 */ 


#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd_lib.h"

void button_conditions(void);
void display(void);
void convert(void);

uint16_t mil_sec = 0;
uint8_t sec = 0, min = 0, hour = 0, start_stop_flag = 0;

char str_mil_sec[4] = {'\0'}, str_sec[3] = {'\0'}, str_min[3] = {'\0'}, str_hour[3] = {'\0'};

int main(void)
{
	DDRD &= ~((1 << PD2) | (1 << PD3));
	TCCR0 |= (1 << WGM01) | (1 << CS02);
	TIMSK |= (1 << OCIE0);
	
	OCR0 = 30;
	TCNT0 = 0x00;
	
	sei();
	LCDinit();
	_delay_ms(50);
	
	LCDWriteStringXY(4, 0,"HR MN SC mSC");
	LCDWriteStringXY(4, 1, "00:00:00:000");
	//initialization 
	
    while(1)
    {
		button_conditions();
		convert();
		if(start_stop_flag)
			display();
        //call functions
    }
}

ISR(TIMER0_COMP_vect){
	
	if(start_stop_flag)
		mil_sec++;
	if(mil_sec >= 1000){
		mil_sec = 0;
		sec++;
	}
	if(sec >= 60){
		sec = 0;
		min++;
	}
	if(min >= 60){
		
		min = 0;
		hour++;
	}

}

void convert(void){
	
	str_mil_sec[2] = (mil_sec % 10) + '0';				//435, 5
	str_mil_sec[1] = (mil_sec / 10) % 10 + '0';			//435, 43, 3
	str_mil_sec[0] = (mil_sec / 100) + '0';
	
	str_sec[1] = (sec % 10) + '0';
	str_sec[0] = (sec / 10) + '0';
	
	str_min[1] = (min % 10) + '0';
	str_min[0] = (min / 10) + '0';
	
	str_hour[1] = (hour % 10) + '0';
	str_hour[0] = (hour / 10) + '0';
	
	return;
}

void display(void){
	
	_delay_us(100);
	LCDWriteStringXY(13, 1, str_mil_sec);
	_delay_us(100);
	LCDWriteStringXY(10, 1, str_sec);
	_delay_us(100);
	LCDWriteStringXY(7, 1, str_min);
	_delay_us(100);
	LCDWriteStringXY(4, 1, str_hour);
	_delay_us(100);
	
	return;
}

void button_conditions(){
	
	if((PIND & (1 << PD2)) == 0){						//is start/stop pressed?
		
		if(start_stop_flag == 1){
			start_stop_flag = 0;
			TCCR0 &= ~(CS02);
		}else{
			start_stop_flag = 1;
			TCCR0 |= (1 << CS02);
		}
		while((PIND & (1 << PD2)) == 0);
	}
	
	if((PIND & (1 << PD3)) == 0){
		
		start_stop_flag = 0;
		TCCR0 &= ~(1 << CS02);
		mil_sec = 0;
		sec = 0;
		min = 0;
		hour = 0;
		TCNT0 = 0;
		display();
	}
	
	
	return;
}

