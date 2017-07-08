/*
 * StickInput.h
 *
 * Created: 05.10.2013 19:01:41
 *  Author: Fabi
 */ 


#ifndef STICKINPUT_H_
#define STICKINPUT_H_


uint16_t ADCgetCh(uint8_t ch);


void stickGetRawADC(int16_t* _pChannels);
void sticksProcessRaw(int16_t* _pChannels);
void stickCalibrate(void);
uint16_t stickScale(uint16_t);
void stickInit(void);

#endif /* STICKINPUT_H_ */