/*
 * music.h
 *
 * Created: 04.10.2013 18:37:08
 *  Author: Fabi
 */ 


#ifndef MUSIC_H_
#define MUSIC_H_

#define NOTE5C	239
#define NOTE5CS	225
#define NOTE5D	213
#define NOTE5DS	201
#define NOTE5E	190
#define NOTE5F	179
#define NOTE5FS	169
#define NOTE5G	159
#define NOTE5GS	150
#define NOTE5A	142
#define NOTE5AS	134
#define NOTE5B	127

#define NOTE6C	119
#define NOTE6CS	113
#define NOTE6D	106
#define NOTE6DS	100
#define NOTE6E	95
#define NOTE6F	89
#define NOTE6FS	84
#define NOTE6G	80
#define NOTE6GS	75
#define NOTE6A	71
#define NOTE6AS	67
#define NOTE6B	63

#define NOTE7C	60
#define NOTE7CS	56
#define NOTE7D	53
#define NOTE7DS	50
#define NOTE7E	47
#define NOTE7F	45
#define NOTE7FS	42
#define NOTE7G	40
#define NOTE7GS	38
#define NOTE7A	36
#define NOTE7AS	34
#define NOTE7B	32

#define NOTEPAUSE 1
#define NOTESTOP 0

void stopNote(void);
void twoTone(uint8_t note);
void oneTone(uint8_t note); 
void playNote(uint8_t note);



#endif /* MUSIC_H_ */