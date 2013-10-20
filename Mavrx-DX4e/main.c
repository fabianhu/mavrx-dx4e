/*
 main.c
 Custom firmware for the Spektrum DX4e 
 Based on a work, which was released by Yuan from mavrx.co to the public domain (unlicense.org - license)
 
 If you like to reprogram your DX4e, switch all switches to the upper position, otherwise ISP does not work.
 Just in case, you want to save the old Firmware = CPU (which is of course lock-fused), 
 the CPU is GLUED to the board, so be sure to unsolder all pads before applying gentle force.
 
 Set the fuses of your new (or erased) CPU to High frequency XTAL and brown out 2.7V
  
 + stick calibration
 + special channel assignment for Blade Nano
 - removed special functions
 - removed easter egg (space!)

Some fixes, that the original manual applies again: fixme
- DSM2 / DSMX switching (including hi/lo rate)
- range test (should work)
- Mode switch over (maybe also the stick assignment for mode 1/3 vs 2/4)
- Servo reverse
+ The mix mode is replaced by the "heli direct mode" which emulates a computer transmitter.
+ France mode is canceled due to newer EU regulations since 2012-07-01
 
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include <util/delay.h>
#include "spektrum-tx.h"