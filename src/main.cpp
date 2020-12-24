/**
 * moisture sensor for house plants
 *
 * based on:
 * - https://github.com/lucafabbri/HiGrow-Arduino-Esp/blob/aac562fce664ef1c951b2ea9673358189671c310/HiGrowEsp32/HiGrowEsp32.ino
 *
 * parts:
 * - ESP32
 * - moisture sensor
 */
#include "arduino_secrets.h"
#include <WiFi.h>
#include <ESP.h>
#include <esp_deep_sleep.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define uS_TO_S_FACTOR 1000000LL

int DEEPSLEEP_SECONDS = 86400;

uint64_t chipID;

const int SOIL_PIN = 32;

const char* ssid = SECRET_SSID;
const char* wifiPassword = SECRET_PASS;
const char* chatID = SECRET_CHAT_ID;

const char BotToken[] = SECRET_BOT_TOKEN;

// Initialize Telegram BOT
WiFiClientSecure client;
UniversalTelegramBot bot(BotToken, client);

char deviceid[21];

void setup() {
  Serial.begin(115200);
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
  char message[1024];

  int soilMoistureValue = analogRead(SOIL_PIN);
  int moisturePercentage = (100 - ((soilMoistureValue / 1023.00) * 100));

  Serial.print(F("water %: "));
  Serial.print(moisturePercentage);

  strcpy(message, "deviceId: ");
  strcat(message, String(deviceid).c_str());
  strcat(message, "\nwater %: ");
  strcat(message, String(moisturePercentage).c_str());
  strcat(message, "%");

  bot.sendMessage(chatID, message, "");
}

void loop() {
  sendSensorData();

  esp_sleep_enable_timer_wakeup(DEEPSLEEP_SECONDS * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}
