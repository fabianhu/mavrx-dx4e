/*
 * spektrum_tx.h
 *
 * Created: 04.10.2013 17:48:02
 *  Author: Fabi
 */ 


#ifndef SPEKTRUM_TX_H_
#define SPEKTRUM_TX_H_

#define SPEKTRUM_med 512
#define SPEKTRUM_min 162
#define SPEKTRUM_max 862 //162..862


void spektrumInit(void);
void spektrumSend(uint16_t chs[6], bool bind, bool rangecheck); // expects an array



#endif /* SPEKTRUM_TX_H_ */

/*
http://www.rcgroups.com/forums/showpost.php?p=7925933&postcount=8

Decided to play with this a little more this evening and everything fell into place.
I tried a serial port rate of 125K instead of 115200. The data is now super easy to see and follow.

The radio transmits 14 bytes per frame, 6 channels of information, 2 header bytes. Data rate is 125K 8,N,1

Each frame begins with 00 00

Each pair of bytes after that represent the channel and its value. Each channel has a valid range(1024 steps - 10 bit), the channel number is embedded in each pair of bytes.
The radio transmits a 6th channel which is a copy of channel 1 with its endpoints limited. This appears to simply be a function of the radio when in airplane mode, channel 6 is still a full 10 bit channel.

Here is a breakdown of the min and max values for each channel and the valid 10 bit range for each channel.

ch1(left stick, up/down)
bytes 3&4
0056-03AA -measured min/max
10 bit range - 000-3FF

ch2(right stick, left/right)
bytes 5&6
0455-07A9 - measured min/max
10 bit range - 400-7FF

ch3(right stick, up/down)
bytes 7&8
0855-0BA9 - measured min/max
10 bit range - 800-BFF

ch4(left stick, right/left)
bytes 9&10
0C56-0FAA - measured min/max
10 bit range - C00-FFF

ch5(knob, upper right)
bytes 11&12
1056-13AA - measured min/max
10 bit range - 1000-13FF

ch6(copy of throttle channel) - travel limited
bytes 13&14
152A-162A - measured min/max
10 bit range - 1400-17FF

Channel breakdown:

First two bits are 00
Next four bits indicate channel #
remaining 10 bits are serial values for the PPM data

00 00 00 xx xx xx xx xx - ch1 0000-03FF

00 00 01 xx xx xx xx xx - ch2 0400-07FF

00 00 10 xx xx xx xx xx - ch3 0800-0BFF

00 00 11 xx xx xx xx xx - ch4 0C00-0FFF

00 01 00 xx xx xx xx xx - ch5 1000-13FF

00 01 01 xx xx xx xx xx - ch6 1400-17FF

If supported by DSM2 low power module:

00 01 10 xx xx xx xx xx - ch7 1800-1BFF

00 01 11 xx xx xx xx xx - ch8 1C00-1FFF

So there it is, using the above data its now possible to build a 6 channel PPM to serial encoder to interface to the DSM2 module from the LP5DSM radio.


*/