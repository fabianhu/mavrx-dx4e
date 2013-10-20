/*
 * config.h
 *
 * Created: 04.10.2013 17:52:34
 *  Author: Fabi
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#define F_CPU 8000000UL

#define bool uint8_t

/*
1  PD3 - Buzzer (should idle low to prevent unnecessary current drain)
2  PD4 - Trainer port detect (detects whether trainer port is plugged in, plugged in is low) active low
3  GND
4  VCC
5  GND
6  VCC
7  XTAL1
8  XTAL2
9  PD5 - BIND button (depressed is low, released is high) active low
10 PD6 - RATE switch (LO is low, HI is high)
11 PD7 - AUX switch (OFF is low, ON is high)
12 PB0 - THR REV switch (NOR is low, REV is high)
13 PB1 - AIL REV switch (NOR is low, REV is high)
14 PB2 - ELE REV switch (NOR is low, REV is high)
15 PB3 - RUD REV switch (NOR is low, REV is high), also ICSP MOSI (ensure switch is in REV position for ICSP)
16 PB4 - MIX switch (OFF is low, ON is high), also ICSP MISO (ensure switch is in ON position for ICSP)
17 PB5 - MD switch (MD-2 is low, MD-4 is high), also ICSP SCK (ensure switch is in MD-4 position for ICSP)
18 AVCC
19 ADC6 - battery voltage
20 AREF
21 GND
22 ADC7 - Throttle axis
23 PC0 - Green LED 1
24 PC1 - Green LED 2
25 PC2 - Red LED 0
26 PC3/ADC3 - Rudder axis
27 PC4/ADC4 - Elevator Axis
28 PC5/ADC5 - Aileron Axis
29 RESET
30 PD0 - Green LED 3
31 PD1/TXD - radio module
32 PD2 - trainer port (capacitor decoupled, and can be either input or output)

// battery voltage thresholds: 5.75V, 5.25V, 4.75V (brownout at 3.6V)
*/

#define TRAINERPIN  ((PIND >> 4) & 0x1)
#define BINDPIN     ((PIND >> 5) & 0x1)
#define RATEPIN     ((PIND >> 6) & 0x1)
#define AUXPIN      ((PIND >> 7) & 0x1)
#define THROPIN     ((PINB >> 0) & 0x1)
#define AILEPIN     ((PINB >> 1) & 0x1)
#define ELEVPIN     ((PINB >> 2) & 0x1)
#define RUDDPIN     ((PINB >> 3) & 0x1)
#define MIXPIN      ((PINB >> 4) & 0x1)
#define MDPIN       ((PINB >> 5) & 0x1)

#define LED0On()	PORTC &= ~0x04;
#define LED1On()	PORTC &= ~0x01;
#define LED2On()	PORTC &= ~0x02;
#define LED3On()	PORTD &= ~0x01;

#define LED0Tgl()	PINC |= 0x04;
#define LED1Tgl()	PINC |= 0x01;
#define LED2Tgl()	PINC |= 0x02;
#define LED3Tgl()	PIND |= 0x01;

#define CHANNELHELIMODE 1

#define OVERSAMPLE 11 // also fixes the TX data frame frequency 11 => 22 Hz @ FASTLOOPCOUNT 250 - max 63 !!!
#define FASTLOOPCOUNT 250 // 125000 Hz / 250 = 500 Hz  //ex 200: 125000 Hz / 200 = 625 Hz

#endif /* CONFIG_H_ */