/* CONFIGURATION */

#define DEFAULTMODE MIDIMODE // MIDIMODE or PRINTMODE

const int bassPedalPin = A0;
const int bassNote = 36;
const int bassPedalPressThreshold = 820;
const int bassPedalReleaseThreshold = 810;

const int hihatPedalPin = A1;
const int hihatDigitalPin = 9;
const int hihatNoteCount = 5;
const int hihatNotes[] = {60, 26, 25, 24, 22};
const int hihatThresholds[] = {664, 498, 332, 166, 0};

const int digitalInputCount = 6;
const int digitalInputPins[] = {3, 4, 5, 6, 7, 8};
const int digitalInputNotes[] = {49, 51, 48, 47, 43, 38};

const int inputTimeoutDuration = 50;

/*
INPUT   NAME            NOTE(S)
A0      bass            36
3       crash cymbal 1  49
_       crash cymbal 2  57
4       ride cymbal     51
5       low tom         48
6       mid tom         47
7       high tom        43
8       snare drum      38
2,A1    hi hat          60, 26, 25, 24, 22
*/