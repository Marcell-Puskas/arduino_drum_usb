#include <USB-MIDI.h>
#include "config.h"

/* Variables */
int bassPedalValue;
bool bassPedalPressed;

int hihatPedalValue;
int hihatPedalPervious;

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

long constrainedMap(long x, long in_min, long in_max, long out_min, long out_max)
{
    return constrain(
      map(x, in_min, in_max, out_min, out_max), 
      out_min, 
      out_max
      );
}

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
    hihatPedalValue = analogRead(hihatPedalPin);
    for (int i = 0; i < digitalInputCount; i++)
    {
        digitalInputValues[i] = digitalRead(digitalInputPins[i]);
    }
}

void serialPrintValues()
{
    Serial.print(bassPedalValue);
    Serial.print("\t");
    Serial.print(hihatPedalValue);
    for (int i = 0; i < digitalInputCount; i++)
    {
        Serial.print("\t");
        Serial.print(digitalInputValues[i]);
    }
    Serial.println();
}

void sendMidiValues()
{
    if (bassPedalValue > bassPedalPressThreshold && !bassPedalPressed)
    {
        MIDI.sendNoteOn(bassNote, VELOCITY, CHANNEL);
        bassPedalPressed = true;
    }
    else if (bassPedalValue < bassPedalReleaseThreshold)
    {
        bassPedalPressed = false;
    }

    int hihatPedalMapped = constrainedMap(hihatPedalValue, hihatUpThreshold, hihatDownThreshold, 0, 127);
    int hihatPedalPerviousMapped = constrainedMap(hihatPedalPervious, hihatUpThreshold, hihatDownThreshold, 0, 127);
    if (
      abs(hihatPedalValue - hihatPedalPervious) > pedalSensitivity &&
      hihatPedalMapped != hihatPedalPerviousMapped
    ){
      MIDI.sendControlChange(4, hihatPedalMapped, CHANNEL);
      hihatPedalPervious = hihatPedalValue;
    }

    for (int i = 0; i < digitalInputCount; i++)
    {
        if (digitalInputValues[i] == LOW)
        {
            if (digitalInputTimeouts[i] + inputTimeoutDuration < millis())
            {
                MIDI.sendNoteOn(digitalInputNotes[i], VELOCITY, CHANNEL);
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
