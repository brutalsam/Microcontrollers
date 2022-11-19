// Basic demo for accelerometer readings from Adafruit MPU6050
#include "mpu_6050_setup.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_AHRS.h>
#include <math.h>
#include <BleMouse.h>
const float XDEG_MULTIPLIER = 28;
const float YDEG_MULTIPLIER = 18;
const float SENSITIVITY_THRESHOLD = 0.2;
#define LEFT_BUTTON_PIN 13 // GIOP13 pin connected to button
#define BACK_BUTTON_PIN 12 // GIOP13 pin connected to button
BleMouse bleMouse;

Adafruit_MPU6050 mpu;
Adafruit_Madgwick filter;
uint32_t timestamp;
Adafruit_Sensor_Calibration_EEPROM cal;
float previousXValue;
float previousYValue;

// Variables will change:
int leftLastState = HIGH; // the previous state from the input pin
int leftCurrentState;     // the current reading from the input pin

int backLastState = HIGH; // the previous state from the input pin
int backCurrentState;     // the current reading from the input pin

void setup(void) {
  Serial.begin(115200);
  setup_mpu_6050(mpu);

  filter.begin(66);
  cal.begin();
  cal.loadCalibration();
  bleMouse.begin();
  bleMouse.setBatteryLevel(15);
  Wire.setClock(400000); 
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BACK_BUTTON_PIN, INPUT_PULLUP);
  delay(100);
}

void loop() {
 
  if(bleMouse.isConnected()) {
    if ((millis() - timestamp) < (15)) {
      return;
    }
    timestamp = millis();

    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    cal.calibrate(a);
    cal.calibrate(g);

    float gx = g.gyro.x * SENSORS_RADS_TO_DPS;
    float gy = g.gyro.y * SENSORS_RADS_TO_DPS;
    float gz = g.gyro.z * SENSORS_RADS_TO_DPS;

    filter.updateIMU(gx, gy, gz,
              a.acceleration.x, a.acceleration.y, a.acceleration.z);

    float yaw = filter.getYaw();
    float pitch = filter.getPitch();
    float roll = filter.getRoll();

    float beta = filter.getBeta();

    float xMoveValue = previousXValue - yaw;
    if (previousXValue != 0 && abs(xMoveValue) <= SENSITIVITY_THRESHOLD) {
      xMoveValue = 0;
    }

    float yMoveValue = previousYValue - pitch;
    if (previousYValue != 0 && abs(yMoveValue) <= SENSITIVITY_THRESHOLD) {
      yMoveValue = 0;
    }

    previousXValue = yaw;
    previousYValue = pitch;


    // ToDO move buttons to a class/method
    leftCurrentState = digitalRead(LEFT_BUTTON_PIN);
    if (leftLastState == HIGH && leftCurrentState == LOW){
      Serial.println("The button is pressed");
      bleMouse.press(MOUSE_MIDDLE);
      
    }
    else if (leftLastState == LOW && leftCurrentState == HIGH){
      Serial.println("The button is released");
      bleMouse.release(MOUSE_MIDDLE); 
    }
    // save the last state
    leftLastState = leftCurrentState;

    backCurrentState = digitalRead(BACK_BUTTON_PIN);
    if (backLastState == HIGH && backCurrentState == LOW){
      Serial.println("The button is pressed");
      bleMouse.press(MOUSE_BACK);
      
    }
    else if (backLastState == LOW && backCurrentState == HIGH){
      Serial.println("The button is released");
      bleMouse.release(MOUSE_BACK); 
    }
    // save the last state
    backLastState = backCurrentState;

    if (yMoveValue != 0 || xMoveValue !=0) {
      // Move shit
      Serial.print("Mouse move: ");
      Serial.print(xMoveValue);
      Serial.print(", ");
      Serial.println(yMoveValue);
      bleMouse.move(round(xMoveValue * XDEG_MULTIPLIER), -round(yMoveValue * YDEG_MULTIPLIER));
    }
  }
}