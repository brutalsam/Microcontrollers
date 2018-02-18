#ifndef ButtonDebounceDown
#define ButtonDebounceDown
//// constants won't change. They're used here to set pin numbers:
//const int buttonPin = 2;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin
ButtonDebounceDown button1;
//
//// Variables will change:
//int ledState = HIGH;         // the current state of the output pin
//int buttonState;             // the current reading from the input pin
//int lastButtonState = LOW;   // the previous reading from the input pin
//
//// the following variables are unsigned longs because the time, measured in
//// milliseconds, will quickly become a bigger number than can be stored in an int.
//unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
//unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
//  pinMode(buttonPin, INPUT);
//  pinMode(ledPin, OUTPUT);
//
//  // set initial LED state
//  digitalWrite(ledPin, ledState);
button1.Setup(2, 50, onButtonDown);
}

void loop() {
//  // read the state of the switch into a local variable:
//  int reading = digitalRead(buttonPin);
//
//  // check to see if you just pressed the button
//  // (i.e. the input went from LOW to HIGH), and you've waited long enough
//  // since the last press to ignore any noise:
//
//  // If the switch changed, due to noise or pressing:
//  if (reading != lastButtonState) {
//    // reset the debouncing timer
//    lastDebounceTime = millis();
//  }
//
//  if ((millis() - lastDebounceTime) > debounceDelay) {
//    // whatever the reading is at, it's been there for longer than the debounce
//    // delay, so take it as the actual current state:
//
//    // if the button state has changed:
//    if (reading != buttonState) {
//      buttonState = reading;
//
//      // only toggle the LED if the new button state is HIGH
//      if (buttonState == HIGH) {
//        ledState = !ledState;
//      }
//    }
//  }
//
//  // set the LED:
//  digitalWrite(ledPin, ledState);
//
//  // save the reading. Next time through the loop, it'll be the lastButtonState:
//  lastButtonState = reading;
}


void onButtonDown(int pin)
{
  digitalWrite(ledPin, HIGH);
}


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
#endif

