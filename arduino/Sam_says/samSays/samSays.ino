#include <Debounce.h>

const int BLUE_BUTTON_PIN      = 4;
const int RED_BUTTON_PIN       = 2;
const int GREEN_BUTTON_PIN     = 5;
const int YELLOW_BUTTON_PIN    = 3;
const int MICROPHONE_PIN       = 12;
const int LED_BLUE_PIN         = 10;
const int LED_RED_PIN          = 8;
const int LED_GREEN_PIN        = 11;
const int LED_YELLOW_PIN       = 9;
const int DEBOUNCE_DELAY       = 22;

static int RED_TONE             = 200;
static int BLUE_TONE            = 400;
static int YELLOW_TONE          = 600;
static int GREEN_TONE           = 800;
static int GAMEOVER_TONE        = 1000;

int gameLevel[200];
int currentLevel;
int currentGameState;
int gameSpeed;
enum color { YELLOW, GREEN, RED, BLUE };
enum gamestate {NEW, INGAME, GAME_OVER};

Debounce blueButton = Debounce( DEBOUNCE_DELAY , BLUE_BUTTON_PIN );
Debounce redButton = Debounce( DEBOUNCE_DELAY , RED_BUTTON_PIN );
Debounce greenButton = Debounce( DEBOUNCE_DELAY , GREEN_BUTTON_PIN );
Debounce yellowButton = Debounce( DEBOUNCE_DELAY , YELLOW_BUTTON_PIN ); 

void setup() {
  pinMode(BLUE_BUTTON_PIN,INPUT);
  pinMode(LED_BLUE_PIN,OUTPUT);
  pinMode(RED_BUTTON_PIN,INPUT);
  pinMode(LED_RED_PIN,OUTPUT);
  pinMode(GREEN_BUTTON_PIN,INPUT);
  pinMode(LED_GREEN_PIN,OUTPUT);
  pinMode(YELLOW_BUTTON_PIN,INPUT);
  pinMode(LED_YELLOW_PIN,OUTPUT);
  pinMode(MICROPHONE_PIN, OUTPUT);
  randomSeed(analogRead(0));
  currentLevel = 0;
  Serial.begin(9600);
  gameSpeed = 500;
  currentGameState = NEW;
}

void loop() {
    switch(currentGameState)
    {
        case NEW:
            playIntro();
            getNewLevel();
            currentGameState = INGAME;
            break;
        case INGAME:
            playLevel();
            break;
        case GAME_OVER:
            gameOverMode();
            break;
        default:
          currentGameState = NEW;

    }
}

void playIntro()
{
    tone(MICROPHONE_PIN, RED_TONE, 200);
    delay(200);
    tone(MICROPHONE_PIN, YELLOW_TONE, 200);
    delay(200);
    tone(MICROPHONE_PIN, GREEN_TONE, 500);
    delay(500);
}

void gameOverMode()
{
    tone(MICROPHONE_PIN, GREEN_TONE, 200);
    delay(200);
    tone(MICROPHONE_PIN, YELLOW_TONE, 200);
    delay(200);
    tone(MICROPHONE_PIN, RED_TONE, 500);
    delay(500);
    int ledState = LOW;
    unsigned long previousMillis = 0;
    while(true) 
    {
        unsigned long currentMillis = millis();

        if (currentMillis - previousMillis >= 600) {
          // save the last time you blinked the LED
          previousMillis = currentMillis;

          // if the LED is off turn it on and vice-versa:
          if (ledState == LOW) {
            ledState = HIGH;
          } else {
            ledState = LOW;
          }

          // set the LED with the ledState of the variable:
          digitalWrite(LED_BLUE_PIN, ledState);
          digitalWrite(LED_RED_PIN, ledState);
          digitalWrite(LED_GREEN_PIN, ledState);
          digitalWrite(LED_YELLOW_PIN, ledState);
        }
        
        updateDebouncedButtons();
        if (redButton.read() > 0 || greenButton.read() > 0 ||
          yellowButton.read() > 0 || blueButton.read() > 0)
        { 
            currentLevel = 0;
            currentGameState = NEW;
            digitalWrite(LED_BLUE_PIN, LOW);
            digitalWrite(LED_RED_PIN, LOW);
            digitalWrite(LED_GREEN_PIN, LOW);
            digitalWrite(LED_YELLOW_PIN, LOW);
            break; 
        }
    }      
}

void playLevel()
{
    for (int i = 0; i < currentLevel; ++i) {
      playNote(gameLevel[i], gameSpeed);
      flashLed(gameLevel[i], gameSpeed);
    }

    if (userInput() == 1)
    {
      getNewLevel();
    }
}

void updateDebouncedButtons()
{
  blueButton.update ( );
  redButton.update ( );
  greenButton.update ( );
  yellowButton.update ( );
}

void flashLed(int led, int flashSpeed) {
    Serial.print("flashLed: Flashing LED: ");
    Serial.print(led);
    Serial.print(" with speed: ");
    Serial.println(flashSpeed);

    led = colorCodeToPin(led);

    digitalWrite(led, HIGH);
    delay(flashSpeed);
    digitalWrite(led, LOW);
}

void playNote(int note, int noteSpeed) {
    Serial.print("playNote: Playing note: ");
    Serial.print(note);
    Serial.print(" with speed: ");
    Serial.println(noteSpeed);
    
    note = getNote(note);
    
    tone(MICROPHONE_PIN, note, noteSpeed);  
}

int colorCodeToPin(int value) {
    int ret_val = -1;
   
    switch(value) {
      case RED:
          ret_val = LED_RED_PIN;
          break;
      case GREEN:
          ret_val = LED_GREEN_PIN;
          break;
      case BLUE:
          ret_val = LED_BLUE_PIN;
          break;
      case YELLOW:
          ret_val = LED_YELLOW_PIN;
          break;
      default:
        Serial.println("colorCodeToPin: Invalid value!");
        delay(1000);
        exit(0);
    }

    return ret_val;
}

int getNote(int note) {
    int return_value = -1;
    switch(note) {
      case YELLOW:
          return_value = YELLOW_TONE;
          break;
      case GREEN:
          return_value = GREEN_TONE;
          break;
      case RED:
          return_value = RED_TONE;
          break;
      case BLUE:
          return_value = BLUE_TONE;
          break;
      case 4:
          return_value = GAMEOVER_TONE;
          break;        
      default:
        Serial.println("playNote: Error! Invalid note!");
        delay(1000);
        exit(0);
    }
    return return_value;
}


int userInput() {
    Serial.print("Current level is: ");
    Serial.println(currentLevel);
    for (int i = 0; i < currentLevel; ++i) {
      Serial.println("userInput: User is pressing.");
      int buttonPressed = -1;
      int buttonValue = -1;
      while(true) {
          updateDebouncedButtons();
          buttonValue = redButton.read();
          if (buttonValue > 0) 
          { 
              buttonPressed = RED; 
              break; 
          }
          buttonValue = greenButton.read();
          if (buttonValue > 0) 
          { 
              buttonPressed = GREEN; 
              break; 
          }
          buttonValue = yellowButton.read();
          if (buttonValue > 0) 
          { 
              buttonPressed = YELLOW; 
              break; 
          }
          buttonValue = blueButton.read();
          if (buttonValue > 0) 
          { 
              buttonPressed = BLUE; 
              break; 
          }
      }
      Serial.print("buttonValue: ");
      Serial.println(buttonValue);
      Serial.print("buttonPressed: ");
      Serial.println(buttonPressed);
      if (buttonPressed != gameLevel[i]) {
          currentGameState = GAME_OVER;
          return 0;
      }
      playNote(buttonPressed, gameSpeed);
      flashLed(buttonPressed, gameSpeed);
    }
    delay(500);
    return 1;
}

void getNewLevel()
{
  gameLevel[currentLevel] = random(0, 4);
  currentLevel++;
}

