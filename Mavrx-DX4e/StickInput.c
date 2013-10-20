/*
 * StickInput.c
 *
 * Created: 05.10.2013 19:01:32
 *  Author: Fabi
 */ 

#include <avr/io.h>
#include "config.h"
#include <util/delay.h>
#include "StickInput.h"
#include <avr/eeprom.h>
#include "music.h"
#include "spektrum-tx.h"

uint16_t stickCenter[4],stickGain[4];

void stickInit(void)
{
	eeprom_read_block(stickCenter,(void*)0,sizeof(stickCenter));
	eeprom_read_block(stickGain,(void*)sizeof(stickCenter),sizeof(stickCenter));
	
	if(stickCenter[0]== 0xffff || stickCenter[0]== 0x0000)
	{
		stickCalibrate(); // force calibration
	}

}

uint16_t ADCgetCh(uint8_t ch)
{
	// Get stick inputs
	uint8_t mux = 0x40 | ch;
	ADMUX = mux;   // select chan
	_delay_us(20); // wait for mux to switch
	ADCSRA |= 0x40; // start
	while(ADCSRA & 0x40); // wait for completion
	return ADC;
};

void sticksGetRaw(uint16_t* _pChannels)
{
	// Get stick inputs
	_pChannels[0] += ADCgetCh(7);	// select chan 7 Throttle // fixme define mode 1/2
	_pChannels[3] += ADCgetCh(3);	// select chan 3 Rudder
	_pChannels[2] += ADCgetCh(4);	// select chan 4 Elevator
	_pChannels[1] += ADCgetCh(5);    // select chan 5 Aileron
}

void sticksProcessRaw(uint16_t* _pChannels)
{
	for(uint8_t i=0;i<4;i++)
	{
		_pChannels[i] /= OVERSAMPLE;
	}

	for(uint8_t i=0;i<4;i++)
	{
		_pChannels[i] +=SPEKTRUM_med;
		_pChannels[i] -=stickCenter[i];
	}
}


extern void LEDOff(void);

void stickCalibrate(void)
{
	stopNote();
	
	uint8_t i;
	LEDOff();
	
	for(i=0;i<10;i++)
	{
		LED0Tgl();
		_delay_ms(300);
		
		if(i==3) LED1On();
		if(i==6) LED2On();
		if(i==9) LED3On();

	}
	
	LEDOff();
	stickCenter[0]=0;
	stickCenter[1]=0;
	stickCenter[2]=0;
	stickCenter[3]=0;
	
	for(i=0;i<OVERSAMPLE;i++)
	{
		_delay_ms(2);
		sticksGetRaw(stickCenter);
	}
	stickCenter[0]/=OVERSAMPLE;
	stickCenter[1]/=OVERSAMPLE;
	stickCenter[2]/=OVERSAMPLE;
	stickCenter[3]/=OVERSAMPLE;
	
	eeprom_update_block(stickCenter,(void*)0,sizeof(stickCenter));
	
// 	uint16_t max[4],min[4];
// 	for(i=0;i<4;i++)
// 	{
// 		
// 	}
}