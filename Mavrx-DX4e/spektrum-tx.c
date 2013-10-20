/*
 * spektrum_tx.c
 *
 * Created: 04.10.2013 17:47:48
 *  Author: Fabi
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include <util/delay.h>
#include "spektrum-tx.h"

static uint8_t uartBuffer[18];
static uint8_t uartCounter = 0;
static uint8_t uartLength = 0;



void spektrumInit(void)
{
	UBRR0L = 3;  UBRR0H = 0;  // Baud at 125000 baud
	UCSR0C = 0x06; // Async mode, 8N1
	UCSR0B = 0x08; // TX enable
}

// this does DSMX mode only! DSM2 is not (yet) implemented, but DSMX works with cheapo RX
void spektrumSend(uint16_t chs[6], bool bind, bool rangecheck) // the spektrum channel reaches from 0x0000 to 0x03ff (1024 bit) usable range is 162..862
{
	if(bind)
	{
		uartBuffer[0] = 0x98;
		uartBuffer[1] = 0x00;
		uartBuffer[2] = 0x00;
		uartBuffer[3] = 0x00;
		uartBuffer[4] = 0x05;
		uartBuffer[5] = 0xff;
		uartBuffer[6] = 0x09;
		uartBuffer[7] = 0xff;
		uartBuffer[8] = 0x0d;
		uartBuffer[9] = 0xff;
		uartBuffer[10] = 0x10;
		uartBuffer[11] = 0xaa;
		uartBuffer[12] = 0x14;
		uartBuffer[13] = 0xaa;
		uartCounter = 0;
		uartLength = 14;
	}
	else
	{
		uartBuffer[0] = 0x18;
		if(rangecheck) uartBuffer[0] = 0x38; // in range check mode
	
		uartBuffer[1] = 0x00;
	
		for(uint8_t i = 0;i<6;i++)
		{
			uartBuffer[i*2+2] = 0x00 | ((chs[i] >> 8) & 0x03);
			uartBuffer[i*2+3] = ((chs[i]) & 0xff);
		}
		uartCounter = 0;
		uartLength = 14;
	}
	
	while(uartCounter < uartLength) {
		while((UCSR0A & 0x20) == 0);
		UDR0 = uartBuffer[uartCounter++];
	}
	
}

// below some stuff, that might be useful

/*
own test by fabianhu:
Equipment: DX4e with original FW

order for mode 2:
throttle, alieron, elevator, rudder, aux, trainer


DSMX mode:                  18 00 02 3C 05 FC 0A 04 0D FF 13 54 14 AA
DSMX mode, range test:      38 00 02 3C 05 FC 0A 04 0D FE 13 54 17 54
DSMX mode, bind mode:       98 00 00 00 05 FF 09 FF 0D FF 10 AA 14 AA
DSM2 mode:                  10 00 01 D2 05 FF 0A 02 0D FC 13 54 14 AA
DSM2 mode, range test:      30 00 01 D2 05 FC 0A 02 0D FC 13 54 17 54
DSM2 mode, bind mode:       90 00 00 00 05 FF 09 FF 0D FF 10 AA 14 AA
France mode:                00 00 01 C5 04 55 0A 03 0D E3 13 54 14 AA
France mode, range test:    20 00 01 C6 04 55 0A 02 0D E3 13 54 17 54
France mode, bind mode:     80 00 00 00 05 FF 09 FF 0D FF 10 AA 14 AA

One frame every 22ms (21.96 or so...)
At least in the DX4e Transmitter, there is no thing as 2048 bit resolution or 11 ms frame rate.
This may be the case at RX side ??

*/



/*
Well, it seems that DSMX is no miracle either.
I've captured the serial data from my DX4e going through all
possible modes:

DSMX mode:                  18 00 00 8A 05 C0 09 F5 0D F6 13 54 14 AA 
DSMX mode, range test:      38 00 00 8B 05 C0 09 F5 0D F9 13 54 17 54
DSM2 mode:                  10 00 01 F0 05 C0 09 F6 0D EC 13 54 14 AA
DSM2 mode, range test:      30 00 00 8A 05 C5 09 F5 0D F6 13 54 17 54
France mode:                00 00 03 4E 06 01 09 FC 0D EE 13 54 14 AA
France mode, range test:    20 00 00 8A 05 D1 09 F9 0D F7 13 54 17 54

I will open up a new feature request i think.
*/

// DSM2 protocol pulled from th9x - Thanks thus!!!

//http://www.rclineforum.de/forum/board49-zubeh-r-elektronik-usw/fernsteuerungen-sender-und-emp/neuer-9-kanal-sender-f-r-70-au/Beitrag_3897736#post3897736
//(dsm2( LP4DSM aus den RTF ( Ready To Fly ) Sendern von Spektrum.
//http://www.rcgroups.com/forums/showpost.php?p=18554028&postcount=237
// /home/thus/txt/flieger/PPMtoDSM.c
/*
  125000 Baud 8n1      _ xxxx xxxx - ---
#define DSM2_CHANNELS      6                // Max number of DSM2 Channels transmitted
#define DSM2_BIT (8*2)
bind:
  DSM2_Header = 0x80,0
static byte DSM2_Channel[DSM2_CHANNELS*2] = {
                ch
  0x00,0xAA,     0 0aa
  0x05,0xFF,     1 1ff
  0x09,0xFF,     2 1ff
  0x0D,0xFF,     3 1ff
  0x13,0x54,     4 354
  0x14,0xAA      5 0aa
};

normal:
  DSM2_Header = 0,0;
  DSM2_Channel[i*2]   = (byte)(i<<2) | highByte(pulse);
  DSM2_Channel[i*2+1] = lowByte(pulse);


 */
/*

inline void _send_1(uint16_t v)
{
    *pulses2MHzptr++ = v;
}

#define BITLEN_DSM2 (8*2) //125000 Baud
void sendByteDsm2(uint8_t b) //max 10changes 0 10 10 10 10 1
{
    bool    lev = 0;
    uint8_t len = BITLEN_DSM2; //max val: 9*16 < 256
    for( uint8_t i=0; i<=8; i++){ //8Bits + Stop=1
        bool nlev = b & 1; //lsb first
        if(lev == nlev){
            len += BITLEN_DSM2;
        }else{
            _send_1(len -1);
            len  = BITLEN_DSM2;
            lev  = nlev;
        }
        b = (b>>1) | 0x80; //shift in stop bit
    }
    _send_1(len + 10*BITLEN_DSM2 -1); //some more space-time for security
}


void setupPulsesDsm2(uint8_t chns)
{
    static uint8_t dsmDat[2+6*2]={0x80,0,  0x00,0xAA,  0x05,0xFF,  0x09,0xFF,  0x0D,0xFF,  0x13,0x54,  0x14,0xAA};

    static uint8_t state = 0;

    if(state==0){

        if((dsmDat[0] == 0) || ! keyState(SW_Trainer) ){ //init - bind!
            dsmDat[0]=0; dsmDat[1]=0;  //DSM2_Header = 0,0;
            for(uint8_t i=0; i<chns; i++){
                uint16_t pulse = limit(0, g_chans512[i]+512,1023);
                dsmDat[2+2*i] = (i<<2) | ((pulse>>8)&0x03);
                dsmDat[3+2*i] = pulse & 0xff;
            }
        }
    }
    sendByteDsm2(dsmDat[state++]);
    sendByteDsm2(dsmDat[state++]);
    if(state >= 2+chns*2){
        pulses2MHzptr-=3; //remove last stopbits and
        _send_1(20000u*2 -1); //prolong them
        state=0;
    }
}

*/