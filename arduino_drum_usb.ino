#include <USB-MIDI.h>

#define PEDAL0 A0
#define PEDAL1 A1

int p0, p1;
bool p0r, p1r;
int p0trcl = 820;
int p0trop = 810;
int p1trcl = 50;
int p1trop = 60;

const int bass_note = 36;

const int in_num = 7;
const int in_pins[] = {3, 4, 5, 6, 7, 8, 9};
const int in_notes[] = {51, 49, 57, 48, 47, 43, 38};
int in_read[7];
bool in_state[7];
long in_timeouts[7];

const int hi_pin = 10;
int hi_num = 5;
const int hi_notes[] = {60, 26, 25, 24, 22};
const int hi_ranges[] = {664, 498, 332, 166, 0};
int hi_read;
bool hi_state;
long hi_timeout;

int timeout_duration = 50;

/*
3 ride cymbal 51
4 crash cymbal 1 49
5 crash cymbal 2 57
6 low tom 48
7 mid tom 47
8 high tom 43
9 snare drum 38
10 hi hat 60, 26, 25, 24, 22
*/

char until = ';';
char mode = 'm';

USBMIDI_CREATE_DEFAULT_INSTANCE();

void setup()
{
    for (int pini = 0; pini < in_num; pini++)
    {
        pinMode(in_pins[pini], INPUT_PULLUP);
    }
    pinMode(hi_pin, INPUT_PULLUP);
    MIDI.begin(MIDI_CHANNEL_OMNI);
    Serial.begin(115200);
}

void loop()
{
    if(Serial.available())
    {
        switch (Serial.read())
        {
            case 'm': mode = 'm'; break;
            case 'p': mode = 'p'; break;
        }
    }

	p0 = analogRead(PEDAL0);
	p1 = analogRead(PEDAL1);
    for (int i = 0; i < in_num; i++)
    {
        in_read[i] = digitalRead(in_pins[i]);
    }
    hi_read = digitalRead(hi_pin);

    if(mode == 'm')
    {
        if(p0 > p0trcl && !p0r)
        {
            MIDI.sendNoteOn(bass_note, 127, 1);
            p0r = true;
        }
        else if(p0 < p0trop)
        {
            p0r = false;
        }

        if(hi_read == LOW)
        {   
            if(hi_timeout + timeout_duration < millis())
            for(int i = 0; i < hi_num; i++)
            {
                if(p1 > hi_ranges[i])
                {
                    MIDI.sendNoteOn(hi_notes[i], 127, 1);
                    break;
                }
            }
            hi_state = true;
            hi_timeout = millis();
        }
        /* else if(hi_read == HIGH && hi_timeout + timeout_duration < millis())
        {
            hi_state = false;
        } */

        for (int i = 0; i < in_num; i++)
        {
            if(in_read[i] == LOW)
            {
                if(in_timeouts[i] + timeout_duration < millis())
                    MIDI.sendNoteOn(in_notes[i], 127, 1);
                in_state[i] = true;
                in_timeouts[i] = millis();
            }
            /* else if(in_read[i] == HIGH)
            {
                in_state[i] = false;
            } */
        }
    }

    if(mode == 'p')
    {
        Serial.print(p0);
        Serial.print("\t");
        Serial.print(p1);
        for (int i = 0; i < in_num; i++)
        {
            Serial.print("\t");
            Serial.print(in_read[i]);
        }
        Serial.print("\t");
        Serial.println(hi_read);
    }
}
