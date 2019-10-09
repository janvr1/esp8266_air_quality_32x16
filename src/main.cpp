#include <Arduino.h>
#include <mhz19b.h>
#include <ledmatrix32.h>
#include <DHTesp.h>
#include <UbidotsMicroESP8266.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <Ticker.h>

#define DHTTYPE DHTesp::DHT22
#define DHTPIN D0

#define SSID "wifi2"
#define PASSWORD "vsegrepozraku"

#define NTPSERVER "ntp.siq.si"

#define DEVICE_NAME "ESP8266"
#define DEVICE_LABEL "esp8266"
#define TOKEN "BBFF-OhEvgOgc8bkmQaT6Oty2ylBAmYtPiP"

Ubidots ubidotsClient(TOKEN);
MHZ19B mhz;
DHTesp dht;
ledMatrix display;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTPSERVER);
Ticker displayTicker;
Ticker startUpTicker;

int CO2 = 1;
float temp = 1;
float hum = 1;

void sendToUbidots(int CO2, float RH, float temp);
void startUpAnimation();

void setup() {
  display.init();
  display.start(10);
  startUpTicker.attach_ms(20, startUpAnimation);
  
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  timeClient.begin();
  timeClient.update();
  dht.setup(DHTPIN, DHTTYPE);
  mhz.begin();

  unsigned long last_time = millis();
  int i = 0;

  ubidotsClient.setDataSourceLabel(DEVICE_LABEL);
  ubidotsClient.setDataSourceName(DEVICE_NAME);

  timeClient.end();
  setTime(timeClient.getEpochTime()+7200);

  startUpTicker.detach();
}

unsigned long last_time5 = millis();
unsigned long last_time1 = millis();
unsigned long last_time_ubi = millis();
unsigned long now_time;

int currentMode = 1;

void loop() {
  now_time = millis();

  if (currentMode == 1 && (now_time - last_time1) > 1000) {
    display.drawGUI1(hour(), minute(), second(), day(), month(), year());
    last_time1 = now_time;

  }

  if (currentMode == 1 && (now_time - last_time5) > 5000) {
    currentMode = 2;
    CO2 = mhz.getCO2();
    temp = dht.getTemperature();
    hum = dht.getHumidity();
    display.drawGUI2(CO2, round(hum-17.5), round(temp-1.2));

    if ((now_time - last_time_ubi) > 60000) {
      sendToUbidots(CO2, hum, temp);
      last_time_ubi = now_time;
    }

    last_time5 = now_time;
  }

  if (currentMode == 2  && (now_time - last_time5) > 5000) {
    currentMode = 1;
    display.drawGUI1(hour(), minute(), second(), day(), month(), year());
    last_time5 = now_time;
  }
}

void sendToUbidots(int CO2, float RH, float T) {
  ubidotsClient.add("temperature", round(T*10)/10);
  ubidotsClient.add("humidity", (int) round(RH));
  ubidotsClient.add("co2", CO2);
  ubidotsClient.sendAll(true);
}

void startUpAnimation() {
  static int i = 0;
  if (i<512) {
    display.setPixel(1, i%32, i/32, display.RED);
    i++;
  }
}