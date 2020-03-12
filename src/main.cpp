#include <Arduino.h>
#include <jan_mhz19b.h>
#include <esp8266_ledmatrix32.h>
#include <DHTesp.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <Ticker.h>
#include <zrak_api_esp8266.h>

#define DHTTYPE DHTesp::DHT22
#define DHTPIN D0
#define T_corr -0.5
#define RH_corr -3

#define SSID "wifi2"
#define PASSWORD "vsegrepozraku"

#define NTPSERVER "pool.ntp.org"

#define HOST "api.zrak.janvr.wtf"
#define HTTPS true
#define DEVICE_ID "esp8266"
#define WEB_INTERVAL 30 //minute

HardwareSerial co2serial(0);
MHZ19B mhz;
DHTesp dht;
ledMatrix display;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTPSERVER);
Ticker displayTicker;
Ticker startUpTicker;
TempAndHumidity TH;
zrak_client zrak("jan", "janvr1", HOST, HTTPS);

int CO2 = 5678;
float temp = 12;
float hum = 34;

void startUpAnimation();
void displayUpdate();

void setup()
{
  // Serial.begin(9600);
  dht.setup(DHTPIN, DHTTYPE);
  mhz.begin(&co2serial);

  display.init();
  displayTicker.attach_ms(10, displayUpdate);
  startUpTicker.attach_ms(20, startUpAnimation);
  WiFi.enableAP(false);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
  }

  timeClient.begin();
  timeClient.update();
  delay(1000);

  unsigned long last_time = millis();
  int i = 0;

  timeClient.end();
  setTime(timeClient.getEpochTime() + 3600);

  startUpTicker.detach();
}

unsigned long last_time5 = millis();
unsigned long now_time;

bool measurement_sent = false;

int currentMode = 1;

void loop()
{
  // co2serial.println(analogRead(A0));
  now_time = millis();

  if (currentMode == 1)
  {
    display.drawGUI1(hour(), minute(), second(), day(), month(), year());
    delay(1000);
  }

  if (currentMode == 1 && (now_time - last_time5) > 5000)
  {
    currentMode = 2;
    CO2 = mhz.getCO2();
    TH = dht.getTempAndHumidity();
    if (dht.getStatus() == DHTesp::ERROR_NONE)
    {
      temp = TH.temperature + T_corr;
      hum = TH.humidity + RH_corr;
    }
    else
    {
      temp = 1;
      hum = 1;
    }
    display.drawGUI2(CO2, round(hum), round(temp));
    last_time5 = now_time;
  }

  if (currentMode == 2 && (now_time - last_time5) > 5000)
  {
    currentMode = 1;
    last_time5 = now_time;
  }

  if (!measurement_sent && (minute() == 30 || minute() == 0))
  {
    zrak.addVariable("CO2", CO2);
    zrak.addVariable("T", temp);
    zrak.addVariable("RH", hum);
    zrak.send(DEVICE_ID);
    measurement_sent = true;
  }

  if (measurement_sent && (minute() == 29 || minute() == 59))
  {
    measurement_sent = false;
  }
}

void startUpAnimation()
{
  static int i = 0;
  if (i < 512)
  {
    display.setPixel(1, i % 32, i / 32, display.RED);
    i++;
  }
}

void displayUpdate()
{
  display.showFrame();
}