/**
 * moisture sensor for house plants
 *
 * based on:
 * - https://how2electronics.com/capacitive-soil-moisture-sensor-esp8266-esp32-oled-display/
 * - https://github.com/lucafabbri/HiGrow-Arduino-Esp/blob/aac562fce664ef1c951b2ea9673358189671c310/HiGrowEsp32/HiGrowEsp32.ino
 *
 * parts:
 * - ESP32
 * - moisture sensor
 */
#include "arduino_secrets.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define uS_TO_S_FACTOR 1000000LL

int DEEPSLEEP_SECONDS = 86400;

uint64_t chipID;

int SOIL_PIN = 36;

int AIR_VALUE = 4095;
int WATER_VALUE = 1693; // value when sensor is submerged

const char* ssid = SECRET_SSID;
const char* wifiPassword = SECRET_PASS;
const char* chatID = SECRET_CHAT_ID;

const char BotToken[] = SECRET_BOT_TOKEN;

// Initialize Telegram BOT
WiFiClientSecure client;
UniversalTelegramBot bot(BotToken, client);

char deviceid[21];

void setup() {
  Serial.begin(9600);
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  chipID = ESP.getEfuseMac();
  sprintf(deviceid, "%" PRIu64, chipID);
  Serial.print("DeviceId: ");
  Serial.println(deviceid);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  WiFi.begin(ssid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void sendSensorData(){
  float soilMoistureValue = analogRead(SOIL_PIN);
  int moisturePercent = map(soilMoistureValue, AIR_VALUE, WATER_VALUE, 0, 100);

  Serial.print(F("water: "));
  Serial.print(moisturePercent);

  // create telegram message and send
  char message[1024];
  strcpy(message, "deviceId: ");
  strcat(message, String(deviceid).c_str());
  strcat(message, "\nwater: ");
  strcat(message, String(moisturePercent).c_str());
  strcat(message, "%");
  bot.sendMessage(chatID, message, "");
}

void loop() {
  sendSensorData();

  esp_sleep_enable_timer_wakeup(DEEPSLEEP_SECONDS * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}
