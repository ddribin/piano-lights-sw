#include <Arduino.h>
#include <SoftwareSerial.h>
#include <FastLED.h>


#define NUM_LEDS 30
#define DATA_PIN 11
#define CLOCK_PIN 13

#define USE_SERIAL 0

static const uint8_t PIN_BTN0 = 2;
static const uint8_t PIN_BTN1 = 3;
static const uint8_t PIN_BTN2 = 4;

static const uint8_t PIN_POT0 = 0;
static const uint8_t PIN_POT1 = 1;

CRGB leds[NUM_LEDS];


static const int NUM_KEYS = 88;
bool keys[NUM_KEYS];

void setup()
{
#if USE_SERIAL
    Serial.begin(115200);
    Serial.println("Setup");
#endif

    pinMode(PIN_BTN0, INPUT_PULLUP);
    pinMode(PIN_BTN1, INPUT_PULLUP);
    pinMode(PIN_BTN2, INPUT_PULLUP);

    FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB, DATA_RATE_KHZ(100)>(leds, NUM_LEDS);
    FastLED.setBrightness(84);
}

void loop()
{
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

    int val2 = analogRead(PIN_POT1);
    int brightness = map(val2, 0, 1024, 0, 255);
    static int hue = 0;

    // First, clear the existing led values
    FastLED.clear();
    for (int i = 0; i < NUM_KEYS; i++) {
        if (keys[i]) {
            int led = map(i, 0, NUM_KEYS-1, 0, NUM_LEDS-1);
            leds[led] = CHSV(brightness, 255, 255);


#if USE_SERIAL
            Serial.print("key ");
            Serial.print(i);
            Serial.print(" led ");
            Serial.print(led);
            Serial.println();
#endif
        }
    }

    hue++;
    FastLED.show();
}
