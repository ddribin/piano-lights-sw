#include <Arduino.h>
#include <SoftwareSerial.h>
#include <FastLED.h>
#include <MIDI.h>


static const int NUM_LEDS = 44;
static const int START_LED = 6;

#define DATA_PIN 10

#define USE_SERIAL 0

static const uint8_t PIN_BTN0 = 2;
static const uint8_t PIN_BTN1 = 3;
static const uint8_t PIN_BTN2 = 4;

static const uint8_t PIN_POT0 = 0;
static const uint8_t PIN_POT1 = 1;

CRGB leds[NUM_LEDS];


static const int NUM_KEYS = 88;
bool keys[NUM_KEYS];
static byte pedal = 0;

struct MySettings : public midi::DefaultSettings
{
    static const bool Use1ByteParsing = false;
};

// MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MySettings);
MIDI_CREATE_DEFAULT_INSTANCE();

static void handleNoteOn(byte channel, byte note, byte velocity);
static void handleNoteOff(byte channel, byte note, byte velocity);
static void handleControlChange(byte channel, byte number, byte value);

long round_closest(long dividend, long divisor)
{
    return (dividend + (divisor / 2)) / divisor;
}

long my_map(long x, long in_min, long in_max, long out_min, long out_max)
{
    long numerator = (x - in_min) * (out_max - out_min);
    long denominator = (in_max - in_min);
    long result = round_closest(numerator, denominator) + out_min;
    return result;
}

void setup()
{
#if USE_SERIAL
    Serial.begin(115200);
    Serial.println("Setup");
#endif

    pinMode(PIN_BTN0, INPUT_PULLUP);
    pinMode(PIN_BTN1, INPUT_PULLUP);
    pinMode(PIN_BTN2, INPUT_PULLUP);

    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
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
    for (int i = 0; i < NUM_KEYS; i++) {
        keys[i] = false;
    }

    keys[10] = digitalRead(PIN_BTN0) == LOW;
    keys[15] = digitalRead(PIN_BTN1) == LOW;
    keys[20] = digitalRead(PIN_BTN2) == LOW;

    int val1 = analogRead(PIN_POT0);
    int key1 = map(val1, 0, 1023, 0, NUM_KEYS-1);
    keys[key1] = true;

#if USE_SERIAL
    Serial.print("val ");
    Serial.print(val1);
    Serial.print(" key ");
    Serial.print(key1);
    Serial.println();
#endif
#endif
    int val1 = analogRead(PIN_POT0);
    int saturation = my_map(val1, 0, 1023, 0, 255);
    int val2 = analogRead(PIN_POT1);
    int brightness = my_map(val2, 0, 1023, 0, 255);

    MIDI.read();

    // First, clear the existing led values
    FastLED.clear();
    MIDI.read();
    for (int i = 0; i < NUM_KEYS; i++) {
        if (keys[i]) {
            int led = my_map(i, 0, NUM_KEYS-1, START_LED, NUM_LEDS-1);
            int keyHue = my_map(i, 0, NUM_KEYS-1, 0, 255);
            leds[led] = CHSV(keyHue, saturation, brightness);


#if USE_SERIAL
            Serial.print("key ");
            Serial.print(i);
            Serial.print(" led ");
            Serial.print(led);
            Serial.println();
#endif
        }
    }
    MIDI.read();

    FastLED.show();
    MIDI.read();
}

static void handleNoteOn(byte channel, byte note, byte velocity)
{
    if ((note >= 21) && (note <= 108)) {
        keys[note - 21] = true;
    }
}

static void handleNoteOff(byte channel, byte note, byte velocity)
{
    if ((note >= 21) && (note <= 108)) {
        keys[note - 21] = false;
    }
}

static void handleControlChange(byte channel, byte number, byte value)
{
    // if (channel == 64) {
        pedal = number;
    // }
}
