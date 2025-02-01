/* CONFIGURATION */

#define DEFAULTMODE MIDIMODE // MIDIMODE or PRINTMODE

const int bassPedalPin = A0;
const int bassNote = 36;
const int bassPedalFirstThreshold = 810;
const int bassPedalSecondThreshold = 820;

const int hihatPedalPin = A1;
const int hihatDownThreshold = 830;
const int hihatUpThreshold = 120;

const int digitalInputCount = 6;
const int digitalInputPins[] = {2, 3, 4, 5, 6, 7, 8};
const int digitalInputNotes[] = {44, 49, 51, 48, 47, 43, 38};

const int inputTimeoutDuration = 50;

// A higher number makes the input less sensitive
const int pedalSensitivity = 3;

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
2,A1    hi hat          44
*/