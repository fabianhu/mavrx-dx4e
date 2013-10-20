/*
 * music.c
 *
 * Created: 04.10.2013 18:37:23
 *  Author: Fabi
 */ 

#include <avr/io.h>
#include "config.h"
#include <util/delay.h>
#include "music.h"


uint8_t noteBuffer[20];
uint8_t noteCounter;
uint8_t noteInterruptable;
uint8_t mute;

void stopNote(void) {
	TCCR2A = 0x00; // disconnect OC2B
	PORTD &= ~0x08; // set low
}

void twoTone(uint8_t note) {
	if(noteInterruptable) {
		noteBuffer[0] = note;
		noteBuffer[1] = NOTEPAUSE;
		noteBuffer[2] = note;
		noteBuffer[3] = NOTESTOP;
		noteCounter = 0;
		noteInterruptable = 1;
	}
}

void oneTone(uint8_t note) {
	if(noteInterruptable) {
		noteBuffer[0] = note;
		noteBuffer[1] = NOTESTOP;
		noteCounter = 0;
		noteInterruptable = 1;
	}
}

void playNote(uint8_t note) {
	if(mute == 0) {
		TCCR2A = 0x12; // CTC mode toggle OC2B
		if(OCR2A != note) {
			OCR2A = note;
			OCR2B = note;
			TCNT2 = 0;
		}
	}
}

#define GAP         20
#define SEMIQUAVER  110
#define QUAVER      2*SEMIQUAVER
#define CROTCHET    2*QUAVER
#define MINIM       2*CROTCHET
#define BREVE       2*MINIM
#define TRIPLET     CROTCHET/3

void paschaOvo(void) {
	stopNote();         _delay_ms(CROTCHET);
	playNote(NOTE5AS);  _delay_ms(MINIM - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5AS);  _delay_ms(TRIPLET - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5AS);  _delay_ms(TRIPLET - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5AS);  _delay_ms(TRIPLET - GAP);
	stopNote();         _delay_ms(GAP);
	
	playNote(NOTE5AS);  _delay_ms(QUAVER + SEMIQUAVER);
	playNote(NOTE5GS);  _delay_ms(SEMIQUAVER);
	playNote(NOTE5AS);  _delay_ms(CROTCHET);
	stopNote();         _delay_ms(QUAVER);
	playNote(NOTE5AS);  _delay_ms(TRIPLET - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5AS);  _delay_ms(TRIPLET - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5AS);  _delay_ms(TRIPLET - GAP);
	stopNote();         _delay_ms(GAP);
	
	playNote(NOTE5AS);  _delay_ms(QUAVER + SEMIQUAVER);
	playNote(NOTE5GS);  _delay_ms(SEMIQUAVER);
	playNote(NOTE5AS);  _delay_ms(CROTCHET);
	stopNote();         _delay_ms(QUAVER);
	playNote(NOTE5AS);  _delay_ms(TRIPLET - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5AS);  _delay_ms(TRIPLET - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5AS);  _delay_ms(TRIPLET - GAP);
	stopNote();         _delay_ms(GAP);
	
	playNote(NOTE5AS);  _delay_ms(QUAVER);
	playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
	stopNote();         _delay_ms(GAP);
	playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
	stopNote();         _delay_ms(GAP);
	
	while(1) {
		playNote(NOTE5AS);  _delay_ms(SEMIQUAVER);
		stopNote();         _delay_ms(SEMIQUAVER + QUAVER);
		playNote(NOTE5F);   _delay_ms(CROTCHET + QUAVER);
		playNote(NOTE5AS);  _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5AS);  _delay_ms(SEMIQUAVER);
		playNote(NOTE6C);   _delay_ms(SEMIQUAVER);
		playNote(NOTE6D);   _delay_ms(SEMIQUAVER);
		playNote(NOTE6DS);  _delay_ms(SEMIQUAVER);
		
		playNote(NOTE6F);   _delay_ms(MINIM - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE6F);   _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE6F);   _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE6F);   _delay_ms(TRIPLET);
		playNote(NOTE6FS);  _delay_ms(TRIPLET);
		playNote(NOTE6GS);  _delay_ms(TRIPLET);
		
		playNote(NOTE6AS);  _delay_ms(MINIM + QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE6AS);  _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE6AS);  _delay_ms(TRIPLET);
		playNote(NOTE6GS);  _delay_ms(TRIPLET);
		playNote(NOTE6FS);  _delay_ms(TRIPLET);
		
		playNote(NOTE6GS);  _delay_ms(QUAVER + SEMIQUAVER);
		playNote(NOTE6FS);  _delay_ms(SEMIQUAVER);
		playNote(NOTE6F);   _delay_ms(CROTCHET + QUAVER);
		stopNote();         _delay_ms(QUAVER);
		playNote(NOTE6F);   _delay_ms(CROTCHET);
		
		playNote(NOTE6DS);  _delay_ms(SEMIQUAVER);
		stopNote();         _delay_ms(SEMIQUAVER);
		playNote(NOTE6DS);  _delay_ms(SEMIQUAVER);
		playNote(NOTE6F);   _delay_ms(SEMIQUAVER);
		playNote(NOTE6FS);  _delay_ms(MINIM);
		playNote(NOTE6F);   _delay_ms(QUAVER);
		playNote(NOTE6DS);  _delay_ms(QUAVER);
		
		playNote(NOTE6CS);  _delay_ms(SEMIQUAVER);
		stopNote();         _delay_ms(SEMIQUAVER);
		playNote(NOTE6CS);  _delay_ms(SEMIQUAVER);
		playNote(NOTE6DS);  _delay_ms(SEMIQUAVER);
		playNote(NOTE6F);   _delay_ms(MINIM);
		playNote(NOTE6DS);  _delay_ms(QUAVER);
		playNote(NOTE6CS);  _delay_ms(QUAVER);
		
		playNote(NOTE6C);   _delay_ms(SEMIQUAVER);
		stopNote();         _delay_ms(SEMIQUAVER);
		playNote(NOTE6C);   _delay_ms(SEMIQUAVER);
		playNote(NOTE6D);   _delay_ms(SEMIQUAVER);
		playNote(NOTE6E);   _delay_ms(MINIM);
		playNote(NOTE6G);   _delay_ms(CROTCHET);
		
		playNote(NOTE6F);   _delay_ms(QUAVER);
		playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		
		playNote(NOTE5AS);  _delay_ms(SEMIQUAVER);
		stopNote();         _delay_ms(SEMIQUAVER + QUAVER);
		playNote(NOTE5F);   _delay_ms(CROTCHET + QUAVER);
		playNote(NOTE5AS);  _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5AS);  _delay_ms(SEMIQUAVER);
		playNote(NOTE6C);   _delay_ms(SEMIQUAVER);
		playNote(NOTE6D);   _delay_ms(SEMIQUAVER);
		playNote(NOTE6DS);  _delay_ms(SEMIQUAVER);
		
		playNote(NOTE6F);   _delay_ms(MINIM - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE6F);   _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE6F);   _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE6F);   _delay_ms(TRIPLET);
		playNote(NOTE6FS);  _delay_ms(TRIPLET);
		playNote(NOTE6GS);  _delay_ms(TRIPLET);
		
		playNote(NOTE6AS);  _delay_ms(MINIM + CROTCHET);
		playNote(NOTE7CS);  _delay_ms(CROTCHET);
		
		playNote(NOTE7C);   _delay_ms(SEMIQUAVER);
		stopNote();         _delay_ms(SEMIQUAVER + QUAVER);
		playNote(NOTE6A);   _delay_ms(MINIM);
		playNote(NOTE6F);   _delay_ms(CROTCHET);
		
		playNote(NOTE6FS);  _delay_ms(MINIM + CROTCHET);
		playNote(NOTE6AS);  _delay_ms(CROTCHET);
		
		playNote(NOTE6A);   _delay_ms(SEMIQUAVER);
		stopNote();         _delay_ms(SEMIQUAVER + QUAVER);
		playNote(NOTE6F);   _delay_ms(MINIM - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE6F);   _delay_ms(CROTCHET);
		
		playNote(NOTE6FS);  _delay_ms(MINIM + CROTCHET);
		playNote(NOTE6AS);  _delay_ms(CROTCHET);
		
		playNote(NOTE6A);   _delay_ms(SEMIQUAVER);
		stopNote();         _delay_ms(SEMIQUAVER + QUAVER);
		playNote(NOTE6F);   _delay_ms(MINIM);
		playNote(NOTE6D);   _delay_ms(CROTCHET);
		
		playNote(NOTE6DS);  _delay_ms(MINIM + CROTCHET);
		playNote(NOTE6FS);  _delay_ms(CROTCHET);
		
		playNote(NOTE6F);   _delay_ms(SEMIQUAVER);
		stopNote();         _delay_ms(SEMIQUAVER + QUAVER);
		playNote(NOTE6CS);  _delay_ms(MINIM);
		playNote(NOTE5AS);  _delay_ms(CROTCHET);
		
		playNote(NOTE6C);   _delay_ms(SEMIQUAVER);
		stopNote();         _delay_ms(SEMIQUAVER);
		playNote(NOTE6C);   _delay_ms(SEMIQUAVER);
		playNote(NOTE6D);   _delay_ms(SEMIQUAVER);
		playNote(NOTE6E);   _delay_ms(MINIM);
		playNote(NOTE6G);   _delay_ms(CROTCHET);
		
		playNote(NOTE6F);   _delay_ms(QUAVER);
		playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(SEMIQUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
		playNote(NOTE5F);   _delay_ms(QUAVER - GAP);
		stopNote();         _delay_ms(GAP);
	}
}