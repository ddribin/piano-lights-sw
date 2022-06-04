/*
 * Copyright (c) 2022 Dave Dribin
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <FastLED.h>
#include <MIDI.h>


static const int NUM_LEDS = 90;
static const int START_LED = 16;

// Hardware SPI of Arduino Uno
#define DATA_PIN 11
#define CLOCK_PIN 13

static const uint8_t PIN_BTN0 = 2;
static const uint8_t PIN_BTN1 = 3;
static const uint8_t PIN_BTN2 = 4;

static const uint8_t PIN_POT0 = 0;
static const uint8_t PIN_POT1 = 1;

static const uint8_t PIN_LED_GRN = 6;
static const uint8_t PIN_LED_RED = 7;
// LEDs are active low
static const uint8_t LED_ON = LOW;
static const uint8_t LED_OFF = HIGH;

CRGB leds[NUM_LEDS];

static const int NUM_KEYS = 88;
bool keys[NUM_KEYS];
byte pedal = 0;

MIDI_CREATE_DEFAULT_INSTANCE();

static void handleNoteOn(byte channel, byte note, byte velocity);
static void handleNoteOff(byte channel, byte note, byte velocity);
static void handleControlChange(byte channel, byte number, byte value);

long round_closest(long dividend, long divisor)
{
    return (dividend + (divisor / 2)) / divisor;
}

// Custom version of map() that rounds fractional values instead of truncating.
long my_map(long x, long in_min, long in_max, long out_min, long out_max)
{
    long numerator = (x - in_min) * (out_max - out_min);
    long denominator = (in_max - in_min);
    long result = round_closest(numerator, denominator) + out_min;
    return result;
}

void setup()
{
    pinMode(PIN_BTN0, INPUT_PULLUP);
    pinMode(PIN_BTN1, INPUT_PULLUP);
    pinMode(PIN_BTN2, INPUT_PULLUP);

    pinMode(PIN_LED_GRN, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    digitalWrite(PIN_LED_GRN, LED_ON);
    digitalWrite(PIN_LED_RED, LED_ON);

    FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(84);

    MIDI.begin(MIDI_CHANNEL_OMNI);
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.setHandleControlChange(handleControlChange);

    for (int i = 0; i < NUM_KEYS; i++) {
        keys[i] = false;
    }
}

void loop()
{
    MIDI.read();

#if 0
    int pot0 = analogRead(PIN_POT0);
    int saturation = my_map(pot0, 0, 1023, 0, 255);

    int pot1 = analogRead(PIN_POT1);
    int brightness = my_map(pot1, 0, 1023, 0, 255);
#else
    int saturation = 255;
    int brightness = 255;
#endif

    // First, clear the existing led values
    FastLED.clear();
    bool anyKeyDown = false;
    for (int i = 0; i < NUM_KEYS; i++) {
        if (keys[i]) {
            int led = my_map(i, 0, NUM_KEYS-1, START_LED, NUM_LEDS-1);
            int hue = my_map(i, 0, NUM_KEYS-1, 0, 255);
            leds[led] = CHSV(hue, saturation, brightness);
            anyKeyDown = true;
        }
    }

    digitalWrite(PIN_LED_GRN, anyKeyDown? LED_OFF : LED_ON);
    FastLED.show();
}

/// Note A0 MIDI value
static const byte MIN_PIANO_MIDI_NOTE = 21;
/// Note C8 MIDI value
static const byte MAX_PIANO_MIDI_NOTE = 108;

static void handleNoteOn(byte channel, byte note, byte velocity)
{
    if ((note >= MIN_PIANO_MIDI_NOTE) && (note <= MAX_PIANO_MIDI_NOTE)) {
        keys[note - MIN_PIANO_MIDI_NOTE] = true;
    }
}

static void handleNoteOff(byte channel, byte note, byte velocity)
{
    if ((note >= MIN_PIANO_MIDI_NOTE) && (note <= MAX_PIANO_MIDI_NOTE)) {
        keys[note - MIN_PIANO_MIDI_NOTE] = false;
    }
}

static void handleControlChange(byte channel, byte number, byte value)
{
    if (channel == 64) {
        pedal = number;
    }
}
