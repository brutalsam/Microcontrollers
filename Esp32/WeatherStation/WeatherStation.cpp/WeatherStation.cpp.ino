#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <esp_wifi.h>

#define uS_TO_S_FACTOR 1000000ULL   // Conversion factor from microseconds to seconds
#define TIME_TO_SLEEP  580           // Time for ESP32-E to enter deep sleep
// #define TIME_TO_SLEEP  15
const char* ssid = "...";
const char* password = "...";
const char* mqtt_server = "192.168.0.16";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

Adafruit_BME280 bme;

void setup() {
  Serial.begin(115200);
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  }

  print_wakeup_reason();   // Print the wake-up reason 
  setup_wifi();
  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    client.setServer(mqtt_server, 1883);
    delay(100); 
    sendMessage();                                           //Post the data to Thingspeak
  }
  else
  {
    Serial.println("");
    Serial.println("WiFi connection failed");
  }
  // esp_wifi_stop();  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);  
  client.disconnect();
  WiFi.disconnect();
  
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  Serial.println("Going to sleep now");  // We have set the wake up reason. Now we can start go to sleep of the peripherals need to be in deep sleep. If no wake-up source is provided, but deep sleep is initiated, it will sleep forever unless a hardware reset occurs.
  Serial.flush(); 
  esp_deep_sleep_start();   
}

void print_wakeup_reason(){          
  esp_sleep_wakeup_cause_t wakeup_reason;  
  wakeup_reason = esp_sleep_get_wakeup_cause(); 

  switch(wakeup_reason) {             // Check the wake-up reason and print the corresponding message
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 25) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void sendMessage() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  Serial.println("Sending message:");
  String pressureStr = String(bme.readPressure() / 100.0F, 2);
  String tempStr = String(bme.readTemperature(), 2);
  String humidityStr = String(bme.readHumidity(), 2);
  String message = tempStr + "/" + humidityStr + "/" + pressureStr;
  client.publish("esp32/weather", message.c_str());
  Serial.print("message: ");
  Serial.println(message);
  client.loop();
  delay(500);
}
void loop() {
}