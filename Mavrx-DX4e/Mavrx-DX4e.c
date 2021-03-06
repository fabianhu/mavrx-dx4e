/*
* Mavrx_DX4e.c
*
fabianhu-dx4e

Custom firmware for the Spektrum DX4e (may work on DX5e)

The fabianhu version is focused on blade nano flying without expensive RC.

Set the "mix" dip switch to on for "Nano mode"

DSMX only for now, but see comments for other frame formats.

Aux is throttle hold, Bind is the kill switch.

Hold bind at turning on for bind mode. Press Bind for "Kill engine", unlock with Aux. Hold bind and toggle rate 4x = range test. Hold bind and toggle rate 10x = learn stick center (center all sticks and trim in advance)

To download the new Firmware, set all dip-switches to the upper position, otherwise the SPI lines are connected to GND. Leave the fuses, as they are.

If you manage to download the original Firmware, let me know (the lock bit will be set, that's for sure)


* Original Created: 01/09/2013 23:43:32
*  Author: Yuan
*/

#define FLYSECONDS_MAX 180 // 3 minutes

#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include <util/delay.h>
#include "spektrum-tx.h"
#include "music.h"
#include "StickInput.h"

void fastLoop(void);
void slowLoop(void);

void getDigital(void);

uint8_t LED0Duty = 0;
uint8_t LED1Duty = 0;
uint8_t LED2Duty = 0;
uint8_t LED3Duty = 0;

uint8_t battPulse;
/*uint16_t throVoltage;
uint16_t aileVoltage;
uint16_t elevVoltage;
uint16_t ruddVoltage;*/
uint8_t toggleCounter;

uint8_t rateSwitch; // 0 for LOW, 1 for HIGH, treat as active-low
uint8_t auxSwitch;  // 0 for OFF, 1 for ON, treat as active-high
uint8_t bindSwitch; // 0 for pressed, treat as active-low
uint8_t throToggle; // 0 for NOR, 1 for REV, treat as active-low
uint8_t aileToggle; // 0 for NOR, 1 for REV, treat as active-low
uint8_t elevToggle; // 0 for NOR, 1 for REV, treat as active-low
uint8_t ruddToggle; // 0 for NOR, 1 for REV, treat as active-low
uint8_t mixToggle;  // 0 for NOR, 1 for REV, treat as active-low
uint8_t mdToggle;   // 0 for NOR, 1 for REV, treat as active-low

uint8_t trainerPlugged;

uint16_t FlySeconds =0;
uint16_t IdleSeconds =0;
uint8_t KillEngine=0;

// fixme ideas:
// Limit f�r Pitch
// alles abschaltbar �ber mix-schalter


typedef enum
{
	eTM_normal = 0,
	eTM_bind,
	eTM_trainer_slave,
	eTM_trainer_master
}transmitMode_t;

transmitMode_t transmitMode = eTM_normal;

int16_t rawstickvalues[4]; // the raw stick values

extern uint8_t noteBuffer[20];
extern uint8_t noteCounter;
extern uint8_t noteInterruptable;
extern uint8_t mute;


void LEDOff(void) {
	PORTC |= 0x07;
	PORTD |= 0x01;
}


int main(void) {
	// Setup pins
	DDRB = 0x00;	// All pins inputs
	DDRC = 0x07;	// PC0,1,2 as outputs for the LEDs
	DDRD = 0x0b;	// PD0 as output for LED, PD1 for TXD, PD3 for buzzer
	
	PORTD = 0xe0;	// Pullups for switches
	PORTB = 0x3f;	// Pullups for switches
	
	// LEDs
	LEDOff();
	
	// ADC
	ADMUX = 0x4f; // start off with mux on internal input
	ADCSRA = 0x80; // ADC EN
	DIDR0 = 0x38; // disable digital buffers on ADC pins
	
	// Timer
	TCCR0A = 0x00;  // Normal mode
	TCCR0B = 0x03;  // prescaler at 64 results in 125kHz ticks
	
	// UART
	cli(); // disable global interrupts

	stickInit(); // initialise stick input
	
	spektrumInit();
	
	// Get startup mode
	trainerPlugged = TRAINERPIN;
	bindSwitch = BINDPIN;
	transmitMode = 0;
	if(trainerPlugged == 0) transmitMode = eTM_trainer_master; // fixme or trainer slave, if PPM signal present

	if(bindSwitch == 0) transmitMode = eTM_bind;
	
	// Timer loop!
	static uint8_t fastScaler = 255;
	static uint8_t slowScaler = 255;
	static uint16_t secondScaler = 0xffff;
	
	// Buzzer
	TCCR2B = 0x03; // prescaler at 64
	stopNote();
	noteBuffer[0] = NOTE6G;
	noteBuffer[1] = NOTE6GS;

	noteBuffer[2] = NOTESTOP;
	noteCounter = 0;
	noteInterruptable = 0;
	

	
	while(1) {
		
		
		while(TCNT0 < FASTLOOPCOUNT)
		{
			if(TCNT0 < FASTLOOPCOUNT && TCNT0 >= FASTLOOPCOUNT-LED0Duty/3) LED0On(); // red LED too bright, reduce duty
			if(TCNT0 < FASTLOOPCOUNT && TCNT0 >= FASTLOOPCOUNT-LED1Duty) LED1On();
			if(TCNT0 < FASTLOOPCOUNT && TCNT0 >= FASTLOOPCOUNT-LED2Duty) LED2On();
			if(TCNT0 < FASTLOOPCOUNT && TCNT0 >= FASTLOOPCOUNT-LED3Duty) LED3On();
		}
		TCNT0 = 0; // reduce jitter by resetting soon (SPEKTRUM does not like jitter!)
		LEDOff();
		
		// should be going at about 625Hz
		getDigital();
		stickGetRawADC(rawstickvalues);
		
	
		if(++fastScaler >= OVERSAMPLE) //should be 22ms for DSMX (Nano Board can not handle it faster!!)
		{ 
			fastScaler = 0;
			
			// this loop runs slower than 50Hz
			fastLoop();
			/*throVoltage = 0;
			ruddVoltage = 0;
			elevVoltage = 0;
			aileVoltage = 0;*/
			rawstickvalues[0]=0;
			rawstickvalues[1]=0;
			rawstickvalues[2]=0;
			rawstickvalues[3]=0;
			
			if(++slowScaler >= 6) 
			{ // should be going at about 10Hz
				slowScaler = 0;
				slowLoop();
			}
		}
		
		// we are here every 2ms
		if(++secondScaler >=500)
		{
			// every second
			secondScaler=0;
			IdleSeconds++;
			if(auxSwitch && mixToggle)
			{	
				FlySeconds++;
					
				if(FlySeconds == FLYSECONDS_MAX)
					oneTone(NOTE7B);
				if(FlySeconds == FLYSECONDS_MAX+10)
					twoTone(NOTE7C);
			
				if(IdleSeconds >= FLYSECONDS_MAX*2)
				{
					twoTone(NOTE7C);
				}
			}
		}
	}
}

void getDigital(void) {
	// Get switches
	static uint8_t rateSwitchPrev = 3;
	rateSwitch = RATEPIN;
	if(rateSwitch != rateSwitchPrev) {
		rateSwitchPrev = rateSwitch;
		IdleSeconds=0;
		if(rateSwitch) oneTone(NOTE6C);
		else twoTone(NOTE6C);
		
		if(bindSwitch == 0) { // if bind button is held down and the rate switch toggled four times, enter range check mode
			toggleCounter++;
			if(toggleCounter == 4) {
				noteBuffer[0] = NOTE5G;
				noteBuffer[1] = NOTE5FS;
				noteBuffer[2] = NOTE5F;
				noteBuffer[3] = NOTE5E;
				noteBuffer[4] = NOTE5DS;
				noteBuffer[5] = NOTE5D;
				noteBuffer[6] = NOTE5CS;
				noteBuffer[7] = NOTE5C;
				noteBuffer[8] = NOTESTOP;
				noteCounter = 0;
				noteInterruptable = 0;
			}
			else if(toggleCounter > 10) stickCalibrate();//paschaOvo();
		}
	}
	
	static uint8_t auxSwitchPrev = 3;
	auxSwitch  = AUXPIN;
	if(auxSwitch != auxSwitchPrev) {
		auxSwitchPrev = auxSwitch;
		IdleSeconds=0;
		if(auxSwitch) 
		{
			// on
			KillEngine = 0;
			twoTone(NOTE6E);
		}
		else 
		{
			// off
			oneTone(NOTE6E);
			if(mixToggle) // Heli special
			{
				if(FlySeconds > FLYSECONDS_MAX)
				{
					FlySeconds = 0;
				}

			}
		}
	}

	static uint8_t bindSwitchPrev = 3;
	bindSwitch = BINDPIN;
	if(bindSwitch == 0) KillEngine = 1;
	if(bindSwitch != bindSwitchPrev) {
		bindSwitchPrev = bindSwitch;
		IdleSeconds=0;
		if(bindSwitch) {
			oneTone(NOTE6C);
			toggleCounter = 0; // when releasing Bind switch, zero the toggle counter (for the range check mode)
			if(transmitMode == 1) transmitMode = 0; // when releasing Bind switch, escape from bind mode
		}
		else oneTone(NOTE6G);
	}
	static uint8_t throTogglePrev = 3;
	throToggle  = THROPIN;
	if(throToggle != throTogglePrev) {
		throTogglePrev = throToggle;
		if(throToggle) oneTone(NOTE5C);
		else twoTone(NOTE5C);
	}

	static uint8_t aileTogglePrev = 3;
	aileToggle  = AILEPIN;
	if(aileToggle != aileTogglePrev) {
		aileTogglePrev = aileToggle;
		if(aileToggle) oneTone(NOTE5D);
		else twoTone(NOTE5D);
	}
	
	static uint8_t elevTogglePrev = 3;
	elevToggle  = ELEVPIN;
	if(elevToggle != elevTogglePrev) {
		elevTogglePrev = elevToggle;
		if(elevToggle) oneTone(NOTE5E);
		else twoTone(NOTE5E);
	}
	
	static uint8_t ruddTogglePrev = 3;
	ruddToggle  = RUDDPIN;
	if(ruddToggle != ruddTogglePrev) {
		ruddTogglePrev = ruddToggle;
		if(ruddToggle) oneTone(NOTE5F);
		else twoTone(NOTE5F);
	}

	static uint8_t mixTogglePrev = 3;
	mixToggle  = MIXPIN;
	if(mixToggle != mixTogglePrev) {
		mixTogglePrev = mixToggle;
		if(mixToggle) oneTone(NOTE5G);
		else twoTone(NOTE5G);
	}
	
	static uint8_t mdTogglePrev = 3;
	mdToggle  = MDPIN; // mute
	if(mdToggle != mdTogglePrev) {
		mdTogglePrev = mdToggle;
		if(mdToggle) 
			mute = 1; // fixme add mode stuff here
		else {
			mute = 0; // plays tone on unmute
			if(noteInterruptable) {
				noteBuffer[0] = NOTE6C;
				noteBuffer[1] = NOTE7C;
				noteBuffer[2] = NOTE7D;
				noteBuffer[3] = NOTE7E;
				noteBuffer[4] = NOTESTOP;
				noteCounter = 0;
			}
		}
	}
}


void fastLoop(void) {
	uint16_t outch[6];
	// Output to radio
	
	sticksProcessRaw(rawstickvalues); //no more raw after that
	
	uint16_t elevV = elevToggle ? 0x3ff - (uint16_t)rawstickvalues[0] : (uint16_t)rawstickvalues[0];
	uint16_t aileV = aileToggle ? 0x3ff - (uint16_t)rawstickvalues[1] : (uint16_t)rawstickvalues[1];
	uint16_t pitchV = throToggle ? 0x3ff - (uint16_t)rawstickvalues[2] : (uint16_t)rawstickvalues[2];
	uint16_t ruddV = ruddToggle ? 0x3ff - (uint16_t)rawstickvalues[3] : (uint16_t)rawstickvalues[3];
	
	switch(transmitMode) {
		case eTM_trainer_master: // in trainer master mode
		if(rateSwitch == 0) { // rate switch in slave has control position
			break;
		}
		// FALLTHROUGH! only falls through if rateSwitch is 1, i.e. master in control
		case eTM_normal: // in normal mode
		case eTM_bind: // in bind mode
		
		if(mixToggle)
		{			// special for blade heli (tested with Blade nano)
			outch[0] = (auxSwitch && !KillEngine)?SPEKTRUM_max:SPEKTRUM_min; //idle up;;
			outch[1] = aileV;
			outch[2] = elevV;
			outch[3] = ruddV;
			outch[4] = SPEKTRUM_med; // fixme no idea!
			outch[5] = pitchV; // fixme limit here
		}
		else
		{			// normal output
			outch[0] = elevV;
			outch[1] = aileV;
			outch[2] = pitchV;
			outch[3] = ruddV;
			outch[4] = auxSwitch?SPEKTRUM_min:SPEKTRUM_max; //aux1;
			outch[5] = rateSwitch?SPEKTRUM_min:SPEKTRUM_max; //aux2;
		}
		
		
		spektrumSend(outch,transmitMode,toggleCounter >= 4);
		
		break;

		case eTM_trainer_slave: // in trainer slave mode
		// do not send, but make PPM frame fixme
		break;
	}
	

	
	// Power LED - pulses depending on battery level
	static uint8_t LED0Dir = 1;
	if(LED0Dir) {
		if(LED0Duty > 100) LED0Dir = 0;
		else LED0Duty += battPulse;
	}
	else {
		if(LED0Duty < battPulse) LED0Dir = 1;
		else LED0Duty -= battPulse;
	}
	
	// LED1 - is on for range check or bind mode (solid for range check, pulse for bind)
	static uint8_t LED1Dir = 0;
	if(transmitMode == 1) {// bind mode
		if(LED1Dir) {
			if(LED1Duty > 100) LED1Dir = 0;
			else LED1Duty += 10;
		}
		else {
			if(LED1Duty < 10) LED1Dir = 1;
			else LED1Duty -= 10;
		}
	}
	else if(toggleCounter >= 4) { // range check
		LED1Dir = 1;
		if(LED1Duty <= 190) LED1Duty+=10;
	}
	else {
		LED1Dir = 0;
		if(LED1Duty >= 10) LED1Duty-=10;
	}
	
	// LED2 - is on in trainer mode, pulses when slave is in control
	static uint8_t LED2Dir = 0;
	if(trainerPlugged) { // trainer unplugged, LED fades off
		LED2Dir = 0;
		if(LED2Duty >= 10) LED2Duty-=10;
	}
	else { // trainer plugged
		if(transmitMode == 3 && rateSwitch == 0) { // LED pulses in traner master mode and slave in control
			if(LED2Dir) {
				if(LED2Duty > 100) LED2Dir = 0;
				else LED2Duty += 10;
			}
			else {
				if(LED2Duty < 10) LED2Dir = 1;
				else LED2Duty -= 10;
			}
		}
		else {
			// LED is on (trainer slave mode, or trainer master mode but master in control)
			LED2Dir = 1;
			if(LED2Duty <= 190) LED2Duty+=10;
		}
	}
	
	// LED3 - mode indicator for the MIX switch
	static uint8_t LED3Dir = 0;
	LED3Dir = !KillEngine && auxSwitch;// display engine on mixToggle;
	if(LED3Dir) {
		if(LED3Duty <= 190) LED3Duty+=10;
	}
	else {
		if(LED3Duty >= 10) LED3Duty-=10;
	}
	
}

void slowLoop(void) {
	// Read battery voltage
	ADMUX = 0x46;   // select chan 6
	ADCSRA |= 0x40; // start
	while(ADCSRA & 0x40); // wait for completion
	uint8_t level = ADC >> 2; // note: level is now approx 26.1x the actual voltage
	
	// Battery level pulse speeds
	if(level > 150) battPulse = 1; // slow pulse
	else if(level > 137) battPulse = 2;
	else if(level > 120) battPulse = 4;
	else {
		battPulse = 24; // very fast pulses
		
		if(noteBuffer[noteCounter] == 0) {
			noteBuffer[0] = NOTEPAUSE;
			noteBuffer[1] = NOTEPAUSE;
			noteBuffer[2] = NOTEPAUSE;
			noteBuffer[3] = NOTEPAUSE;
			noteBuffer[4] = NOTEPAUSE;
			noteBuffer[5] = NOTEPAUSE;
			noteBuffer[6] = NOTEPAUSE;
			noteBuffer[7] = NOTEPAUSE;
			noteBuffer[8] = NOTEPAUSE;
			noteBuffer[9] = NOTE5C;
			noteBuffer[10] = NOTE5C;
			noteBuffer[11] = NOTE5CS;
			noteBuffer[12] = NOTE5CS;
			noteBuffer[13] = NOTESTOP;
			noteCounter = 0;
			noteInterruptable = 1;
		}
	}
	
	// Check trainer plug plug
	static uint8_t trainerPluggedPrev;
	trainerPlugged = TRAINERPIN;
	if(trainerPlugged != trainerPluggedPrev) {
		trainerPluggedPrev = trainerPlugged;
		if(trainerPlugged) {
			// Not Plugged IN
			if(noteInterruptable) {
				noteBuffer[0] = NOTE6C;
				noteBuffer[1] = NOTE7C;
				noteBuffer[2] = NOTE6C;
				noteBuffer[3] = NOTESTOP;
				noteCounter = 0;
			}
			if(transmitMode == 2 || transmitMode == 3) transmitMode = 0; // escape out of trainer mode
		}
		else {
			// Plugged IN
			noteBuffer[0] = NOTE6C; // these notes interrupt the startup tune, this is by design
			noteBuffer[1] = NOTE6C;
			noteBuffer[2] = NOTE7C;
			noteBuffer[3] = NOTESTOP;
			noteCounter = 0;
			noteInterruptable = 0;
			if(transmitMode == 0) transmitMode = 3; // enter trainer mode
		}
	}
	
	// Beeps while toggling or binding beeps
	if(toggleCounter >= 4 || transmitMode == 1) {
		if(noteBuffer[noteCounter] == 0) {
			noteBuffer[0] = NOTEPAUSE;
			noteBuffer[1] = NOTEPAUSE;
			noteBuffer[2] = NOTEPAUSE;
			noteBuffer[3] = NOTEPAUSE;
			noteBuffer[4] = NOTEPAUSE;
			noteBuffer[5] = NOTEPAUSE;
			noteBuffer[6] = NOTEPAUSE;
			noteBuffer[7] = NOTEPAUSE;
			noteBuffer[8] = NOTEPAUSE;
			noteBuffer[9] = NOTE5C;
			noteBuffer[10] = NOTESTOP;
			noteCounter = 0;
			noteInterruptable = 1;
		}
	}
	
	// Play sounds
	if(noteBuffer[noteCounter] > 0) {
		if(noteBuffer[noteCounter] == NOTEPAUSE) {
			stopNote();
		}
		else {
			playNote(noteBuffer[noteCounter]);
		}
		noteCounter++;
	}
	else {
		stopNote();
		noteInterruptable = 1;
	}
}

