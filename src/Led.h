/*
  Led.h - Library for easy use of leds.
  Created by Primoz Flander 06.11.2020.
  Released into the public domain.
*/

#ifndef LED_H
#define LED_H
#include <Arduino.h>
class Led {
  private:
    bool inverse = false;
    byte pin;
    byte brightness = 0;
    bool state = 0;
    unsigned long previousMillis = 0;
  public:
    Led(byte pin = 13, bool inverse = false);
    void init();
    void on();
    void off();
    void blinkMs(unsigned int blinkInterval);
};
#endif
