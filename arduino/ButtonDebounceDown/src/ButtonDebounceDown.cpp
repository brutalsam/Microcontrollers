/*
  ButtonDebounceDown Arduino Library
  Sam Hzhesiak, 02.2018
*/

#include "ButtonDebounceDown.h"


void ButtonDebounceDown::Setup(int pin, int debounceDelay, buttonDown_cb buttonDownCallback)
{
  _pin = pin;
  _debounceDelay = debounceDelay;
  _buttonDownCallback = buttonDownCallback;
  pinMode(_pin, INPUT);
}

void ButtonDebounceDown::check(unsigned long now)
{
  // read the state of the switch into a local variable:
  int reading = digitalRead(_pin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != _lastButtonState) {
    // reset the debouncing timer
    _lastDebounceTime = now;
  }

  if ((millis() - _lastDebounceTime) > _debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != _buttonState) {
      _buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (_buttonState == HIGH) {
          //        ledState = !ledState;
        //callback
      }
    }
  }

  // set the LED:
//  digitalWrite(ledPin, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  _lastButtonState = reading;
}

void ButtonDebounceDown::ButtonDown()
{
  if(_buttonDownCallback) {
    _buttonDownCallback(_pin);
  }
}