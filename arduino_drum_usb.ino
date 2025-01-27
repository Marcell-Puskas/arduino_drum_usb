#include <USB-MIDI.h>
#include "config.h"

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
