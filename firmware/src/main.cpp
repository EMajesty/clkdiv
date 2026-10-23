#include <Arduino.h>

#define CLK_PIN 2
#define OUT_PIN 4

void setup() {
    pinMode(CLK_PIN, INPUT);
    pinMode(OUT_PIN, OUTPUT);
}

void loop() {
    // Q1 inverts the clock
    digitalWrite(OUT_PIN, !digitalRead(CLK_PIN));
}
