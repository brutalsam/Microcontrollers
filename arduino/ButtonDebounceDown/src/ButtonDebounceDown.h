/*
  ButtonDebounceDown Arduino Library
  Sam Hzhesiak, 02.2018
*/

#ifndef BUTTONDEBOUNCEDOWN_H
#define BUTTONDEBOUNCEDOWN_H

typedef void (*buttonDown_cb)(int);

class ButtonDebounceDown{
  private:
    int _pin;
    int _debounceDelay;
    unsigned long _lastDebounceTime = 0;
    int _lastButtonState = LOW;
    int _buttonState;
    buttonDown_cb _buttonDownCallback;
    void ButtonDown();
  public:
    void check(unsigned long now);
    void Setup(int pin, int debounceDelay, buttonDown_cb buttonDownCallback);
};

#include "Arduino.h"