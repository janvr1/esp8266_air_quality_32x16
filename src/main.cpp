#include <Arduino.h>
#include <jan_mhz19b.h>
#include <esp8266_ledmatrix32.h>
#include <DHTesp.h>
#include <UbidotsMicroESP8266.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <Ticker.h>
#include <zrak_api.h>

#define DHTTYPE DHTesp::DHT22
#define DHTPIN D0
#define T_corr -0.7
#define RH_corr 0

#define SSID "wifi2"
#define PASSWORD "vsegrepozraku"

#define NTPSERVER "ntp.siq.si"

#define DEVICE_NAME "ESP8266"
#define DEVICE_LABEL "esp8266"
#define TOKEN "BBFF-OhEvgOgc8bkmQaT6Oty2ylBAmYtPiP"

#define DEVICE_ID 6

Ubidots ubidotsClient(TOKEN);
HardwareSerial co2serial(0);
MHZ19B mhz;
DHTesp dht;
ledMatrix display;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTPSERVER);
Ticker displayTicker;
Ticker startUpTicker;
TempAndHumidity TH;
zrak_client zrak("jan", "jan");

int CO2 = 5678;
float temp = 12;
float hum = 34;

void sendToUbidots(int CO2, float RH, float temp);
void startUpAnimation();
void displayUpdate();

void setup() {
  display.init();
  displayTicker.attach_ms(10, displayUpdate);
  startUpTicker.attach_ms(20, startUpAnimation);
  
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  timeClient.begin();
  timeClient.update();
  delay(1000);
  dht.setup(DHTPIN, DHTTYPE);
  mhz.begin(&co2serial);

  unsigned long last_time = millis();
  int i = 0;

  ubidotsClient.setDataSourceLabel(DEVICE_LABEL);
  ubidotsClient.setDataSourceName(DEVICE_NAME);

  timeClient.end();
  setTime(timeClient.getEpochTime()+3600);

  startUpTicker.detach();
}

unsigned long last_time5 = millis();
unsigned long last_time1 = millis();
unsigned long last_time_ubi = millis();
unsigned long now_time;

int currentMode = 1;

void loop() {
  now_time = millis();
  if (currentMode == 1) {
    display.drawGUI1(hour(), minute(), second(), day(), month(), year());
    last_time1 = now_time;
    // co2serial.println(analogRead(A0));
  }

  if (currentMode == 1 && (now_time - last_time5) > 5000) {
    currentMode = 2;
    CO2 = mhz.getCO2();
    TH = dht.getTempAndHumidity();
    if (dht.getStatus() == DHTesp::ERROR_NONE) {
      temp = TH.temperature + T_corr;
      hum = TH.humidity + RH_corr;
    }
    else {
      temp = 1;
      hum = 1;
    }
    display.drawGUI2(CO2, round(hum), round(temp));

    if ((now_time - last_time_ubi) > 60000) {
      sendToUbidots(CO2, hum, temp);
      zrak.addVariable("CO2", CO2);
      zrak.addVariable("T", temp);
      zrak.addVariable("RH", hum);
      zrak.send(DEVICE_ID);
      last_time_ubi = now_time;
    }

    last_time5 = now_time;
  }

  if (currentMode == 2  && (now_time - last_time5) > 5000) {
    currentMode = 1;
    display.drawGUI1(hour(), minute(), second(), day(), month(), year());
    last_time5 = now_time;
  }
  delay(1000);
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

void displayUpdate() {
  display.showFrame();
}