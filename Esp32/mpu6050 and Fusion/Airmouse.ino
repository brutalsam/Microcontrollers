// Basic demo for accelerometer readings from Adafruit MPU6050
#include "mpu_6050_setup.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_AHRS.h>
#include <math.h>
#include <BleMouse.h>
const float DEG_MULTIPLIER = 14;
const float SENSITIVITY_THRESHOLD = 0.2;
BleMouse bleMouse;

Adafruit_MPU6050 mpu;
Adafruit_Madgwick filter;
uint32_t timestamp;
Adafruit_Sensor_Calibration_EEPROM cal;
float previousXValue;
float previousYValue;


void setup(void) {
  Serial.begin(115200);
  setup_mpu_6050(mpu);

  filter.begin(66);
  cal.begin();
  cal.loadCalibration();
  bleMouse.begin();
  Wire.setClock(400000); 
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

    float yMoveValue = previousYValue - roll;
    if (previousYValue != 0 && abs(yMoveValue) <= SENSITIVITY_THRESHOLD) {
      yMoveValue = 0;
    }

    previousXValue = yaw;
    previousYValue = roll;

    // Serial.print("Orientation: ");
    // Serial.print(previousXValue);
    // Serial.print(", ");
    // Serial.print(yaw);
    // Serial.print(", ");
    // Serial.print(previousYValue);
    // Serial.print(", ");
    // Serial.print(roll);


    // if (yMoveValue != 0 || xMoveValue !=0) {
    //   // Move shit
    //   Serial.print("Mouse move: ");
    //   Serial.print(xMoveValue);
    //   Serial.print(", ");
    //   Serial.println(yMoveValue);

    //   bleMouse.move(round(xMoveValue * DEG_MULTIPLIER), -round(yMoveValue * DEG_MULTIPLIER));
    // }
    
    Serial.print("Orientation: ");
    Serial.print(pitch);
    Serial.print(", ");
    Serial.print(yaw);
    Serial.print(", ");
    Serial.print(roll);
    Serial.print(", ");
    Serial.println(beta);
  }
}