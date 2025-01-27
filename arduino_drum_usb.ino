#include <USB-MIDI.h>

/* CONFIGURATION */

#define DEFAULTMODE MIDIMODE // MIDIMODE or PRINTMODE

const int bassPedalPin = A0;
const int bassNote = 36;
const int bassPedalPressThreshold = 820;
const int bassPedalReleaseThreshold = 810;

const int hihatPedalPin = A1;
const int hihatDigitalPin = 10;
const int hihatNoteCount = 5;
const int hihatNotes[] = {60, 26, 25, 24, 22};
const int hihatThresholds[] = {664, 498, 332, 166, 0};

const int digitalInputCount = 7;
const int digitalInputPins[] = {2, 3, 4, 5, 6, 7, 8};
const int digitalInputNotes[] = {51, 49, 57, 48, 47, 43, 38};

const int inputTimeoutDuration = 50;

/*
INPUT   NAME            NOTE(S)
A0      bass            36
3       crash cymbal 1  49
4       crash cymbal 2  57
5       ride cymbal     51
6       low tom         48
7       mid tom         47
8       high tom        43
9       snare drum      38
10,A1   hi hat          60, 26, 25, 24, 22
*/

/* Variables */

int bassPedalValue;
bool bassPedalPressed;

int HihatPedalValue;

int hihatValue;
long hihatTimeout;

int digitalInputValues[7];
bool digitalInputsPressed[7];
long digitalInputTimeouts[7];

enum Modes
{
    MIDIMODE,
    PRINTMODE
};

enum Modes selectedMode = DEFAULTMODE;

USBMIDI_CREATE_DEFAULT_INSTANCE();

void readSerialMode()
{
    if (Serial.available())
    {
        switch (Serial.read())
        {
        case 'm':
            selectedMode = MIDIMODE;
            break;
        case 'p':
            selectedMode = PRINTMODE;
            break;
        }
    }
}

void readValues()
{
    bassPedalValue = analogRead(bassPedalPin);
    HihatPedalValue = analogRead(hihatPedalPin);
    for (int i = 0; i < digitalInputCount; i++)
    {
        digitalInputValues[i] = digitalRead(digitalInputPins[i]);
    }
    hihatValue = digitalRead(hihatDigitalPin);
}

void serialPrintValues()
{
    Serial.print(bassPedalValue);
    Serial.print("\t");
    Serial.print(HihatPedalValue);
    for (int i = 0; i < digitalInputCount; i++)
    {
        Serial.print("\t");
        Serial.print(digitalInputValues[i]);
    }
    Serial.print("\t");
    Serial.println(hihatValue);
}

void sendMidiValues()
{
    if (bassPedalValue > bassPedalPressThreshold && !bassPedalPressed)
    {
        MIDI.sendNoteOn(bassNote, 127, 1);
        bassPedalPressed = true;
    }
    else if (bassPedalValue < bassPedalReleaseThreshold)
    {
        bassPedalPressed = false;
    }

    if (hihatValue == LOW)
    {
        if (hihatTimeout + inputTimeoutDuration < millis())
            for (int i = 0; i < hihatNoteCount; i++)
            {
                if (HihatPedalValue > hihatThresholds[i])
                {
                    MIDI.sendNoteOn(hihatNotes[i], 127, 1);
                    break;
                }
            }
        hihatTimeout = millis();
    }

    for (int i = 0; i < digitalInputCount; i++)
    {
        if (digitalInputValues[i] == LOW)
        {
            if (digitalInputTimeouts[i] + inputTimeoutDuration < millis())
            {
                MIDI.sendNoteOn(digitalInputNotes[i], 127, 1);
            }
            digitalInputTimeouts[i] = millis();
        }
    }
}

void setup()
{
    for (int pini = 0; pini < digitalInputCount; pini++)
    {
        pinMode(digitalInputPins[pini], INPUT_PULLUP);
    }
    pinMode(hihatDigitalPin, INPUT_PULLUP);
    MIDI.begin(MIDI_CHANNEL_OMNI);
    Serial.begin(115200);
}

void loop()
{
    readSerialMode();

    readValues();

    if (selectedMode == MIDIMODE)
    {
        sendMidiValues();
    }

    if (selectedMode == PRINTMODE)
    {
        serialPrintValues();
    }
}
