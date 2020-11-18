/*
  Led.cpp - Library for easy use of leds.
  Created by Primoz Flander 06.11.2020.
  Released into the public domain.
*/

#include "Led.h"
Led::Led(byte pin, bool inverse) {
  this->pin = pin;
  this->inverse = inverse;
  init();
}

void Led::init() {
  pinMode(pin, OUTPUT);
  off();
}

void Led::on() {
  if (!inverse) {
    digitalWrite(pin, HIGH);
    brightness = 255;
  }
  else
  {
    digitalWrite(pin, LOW);
    brightness = 255;
  }
}

void Led::off() {
  if (!inverse) {
    digitalWrite(pin, LOW);
    brightness = 0;
  }
  else
  {
    digitalWrite(pin, HIGH);
    brightness = 0;
  }
}

void Led::blinkMs(unsigned int blinkInterval) {
  if (millis() - previousMillis >= blinkInterval) {
    digitalWrite(pin, state);
    state = !state;
  previousMillis += blinkInterval;
  }
}