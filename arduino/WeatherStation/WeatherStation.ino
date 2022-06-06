#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 


#define DHTPIN 2  
#define DHTTYPE DHT11

LiquidCrystal_I2C lcd(0x27,16,2);
DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
unsigned long previousMillis = 0;
float previousTemp = 0;
float previousHumitity = 0;

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  // while (!Serial) {
  //   ; // wait for serial port to connect. Needed for native USB port only
  // }
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

    String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    lcd.setCursor(0,0);
    lcd.print("Connecting to");
    lcd.setCursor(0,1);
    lcd.print(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= delayMS)
    {
        sensors_event_t event;
        float currentTemperature = getCurrentTemperature();
        float currentHumidity = getCurrentHumidity();

        if (isHumidityChanged(currentHumidity) || isTemperatureChanged(currentTemperature))
        {
            lcd.clear();
            displayHumidity(currentHumidity);
            displayTemperature(currentTemperature);
        }
    }

    // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an HTTP request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n' && currentLineIsBlank) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.print("Temp=");
            client.print((float)previousTemp);
            client.print(" C");
            client.println("<br />");
            client.print("Humidity=");
            client.print((float)previousHumitity);//print the humidity
            client.print("%");
            client.println("<br />");

            client.println("</html>");
            break;
        }
        if (c == '\n') {
            currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("client disconnected");
  }
}

void displayTemperature(float temperature)
{
    lcd.setCursor(0, 0);
    lcd.print("Temp=");
    lcd.print((float)temperature);
    lcd.print(" C");
    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C"));
    previousTemp = temperature;
}

void displayHumidity(float humidity)
{
    lcd.setCursor(0, 1);
    lcd.print("Humidity=");
    lcd.print((float)humidity);//print the humidity
    lcd.print("%");
    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.println(F("%"));
    previousHumitity = humidity;
}

bool isTemperatureChanged(float temperature)
{
    return (temperature > 0) && (fabs(previousTemp - temperature) > 0.001);
}

bool isHumidityChanged(float humidity)
{
    return (humidity > 0) && (fabs(previousHumitity - humidity) > 0.001);
}

float getCurrentTemperature()
{
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        Serial.println(F("Error reading temperature!"));
        return -1;
    }

    return event.temperature;
}

float getCurrentHumidity()
{
    sensors_event_t event;
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
        Serial.println(F("Error reading humidity!"));
        return -1;
    }

    return event.relative_humidity;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
