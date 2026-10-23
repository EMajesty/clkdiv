#include <Arduino.h>
#include <FastLED.h>

// Pins
#define CLK_PIN 2
#define RST_PIN 3

#define DIV1_PIN 4
#define DIV2_PIN 5
#define DIV3_PIN 6
#define DIV4_PIN 7
#define DIV5_PIN 8
#define DIV6_PIN 9
#define DIV7_PIN 10
#define DIV8_PIN 16

#define BTN_PIN 14
#define RGB_PIN 15

#define NUM_LEDS 1

// Output pins
const uint8_t outputPins[8] = {DIV1_PIN, DIV2_PIN, DIV3_PIN, DIV4_PIN,
                               DIV5_PIN, DIV6_PIN, DIV7_PIN, DIV8_PIN};

// Banks
struct Bank {
    CRGB color;
    uint8_t divs[8];
};

const Bank banks[] = {{CRGB::Purple, {2, 3, 4, 6, 8, 12, 16, 24}},
                      {CRGB::Yellow, {2, 3, 4, 6, 8, 12, 16, 24}},
                      {CRGB::Cyan, {2, 3, 4, 6, 8, 12, 16, 24}}};

constexpr uint8_t NUM_BANKS = sizeof(banks) / sizeof(banks[0]);

CRGB rgb[NUM_LEDS];

volatile uint32_t clockCount = 0;
volatile bool resetRequested = false;

uint8_t currentBank = 0;

// ------------------------------------------------------------
// CLOCK INTERRUPT
//
// The 2N3904 input stage inverts the Eurorack clock:
//
// Eurorack HIGH -> Pro Micro LOW
// Eurorack LOW  -> Pro Micro HIGH
//
// Therefore:
//   GPIO LOW  = beginning of clock pulse
//   GPIO HIGH = end of clock pulse
// ------------------------------------------------------------

void clockISR() {
    bool clockHigh = !digitalRead(CLK_PIN);

    if (clockHigh) {
        // Rising edge of the original Eurorack clock
        ++clockCount;

        const Bank &bank = banks[currentBank];

        for (uint8_t i = 0; i < 8; ++i) {
            uint8_t div = bank.divs[i];

            // All divisions fire on clock #1 after reset.
            bool fire = ((clockCount - 1) % div) == 0;

            digitalWrite(outputPins[i], fire ? HIGH : LOW);
        }
    } else {
        // Original Eurorack clock pulse ended.
        // End all generated output pulses.
        for (uint8_t i = 0; i < 8; ++i) {
            digitalWrite(outputPins[i], LOW);
        }
    }
}

// ------------------------------------------------------------
// RESET INTERRUPT
// ------------------------------------------------------------

void resetISR() { resetRequested = true; }

// ------------------------------------------------------------
// SETUP
// ------------------------------------------------------------

void setup() {
    // Inputs
    pinMode(CLK_PIN, INPUT);
    pinMode(RST_PIN, INPUT);
    pinMode(BTN_PIN, INPUT_PULLUP);

    // Divider outputs
    for (uint8_t i = 0; i < 8; ++i) {
        pinMode(outputPins[i], OUTPUT);
        digitalWrite(outputPins[i], LOW);
    }

    // RGB mode LED
    FastLED.addLeds<WS2812B, RGB_PIN, RGB>(rgb, NUM_LEDS);
    FastLED.setBrightness(32);

    rgb[0] = banks[currentBank].color;
    FastLED.show();

    // Both clock edges are required because output pulse width
    // follows the incoming clock pulse width.
    attachInterrupt(digitalPinToInterrupt(CLK_PIN), clockISR, CHANGE);

    // Input transistor inverts reset too, so original rising edge
    // appears as a falling edge at the MCU.
    attachInterrupt(digitalPinToInterrupt(RST_PIN), resetISR, FALLING);
}

// ------------------------------------------------------------
// LOOP
// ------------------------------------------------------------

void loop() {
    if (resetRequested) {
        noInterrupts();

        resetRequested = false;
        clockCount = 0;

        interrupts();

        // Force every divider output low on reset.
        for (uint8_t i = 0; i < 8; ++i) {
            digitalWrite(outputPins[i], LOW);
        }
    }
}
