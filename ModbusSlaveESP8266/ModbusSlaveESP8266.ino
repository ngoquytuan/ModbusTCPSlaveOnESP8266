#include <ESP8266WiFi.h>
#include "AModbusTCPSlave.h"
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
////https://forums.adafruit.com/viewtopic.php?f=31&t=61774&sid=06d9885dcf852259d1f568fa96e40265&start=15

////WIFI Settings
///*
//byte ip[]      = { 192, 168, 1, 126};
//byte gateway[] = { 192, 168, 1, 1 };
//byte subnet[]  = { 255, 255, 255, 0 };
//*/
ModbusTCPSlave Mb;
#include<time.h>
unsigned long timer;
unsigned long checkRSSIMillis;
#define ledpin 16
int16_t pwm = 0;
int16_t freq = 1000;
void setup()
{

  //Serial.printf("Connection status: %d\n", WiFi.status());
  //Serial.begin(115200);
  
  Mb.begin("SWAT", "thitmeocon");
  
  delay(1000);
  Mb.MBInputRegister[0] = 100;
  Mb.MBInputRegister[1] = 65500;
  Mb.MBInputRegister[2] = 300;
  Mb.MBInputRegister[3] = 400;
  Mb.MBInputRegister[4] = 500;

  Mb.MBHoldingRegister[0] = 0x1101;
  Mb.MBHoldingRegister[1] = 0x2202;
  Mb.MBHoldingRegister[2] = 0x2203;
  Mb.MBHoldingRegister[3] = 0x3304;
  Mb.MBHoldingRegister[4] = 0x4405;
/*
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  */
  ads.begin();
  pinMode (ledpin, OUTPUT );
  analogWriteFreq(80000);
  analogWrite(ledpin,400);
  
}

void loop()
{
  int16_t adc0, adc1, adc2, adc3;
  
  Mb.Run();
  delay(10);

  if (millis() - timer >= 100) {
    timer = millis();
    Mb.MBInputRegister[1]++;
    //WiFi.printDiag(Serial);
    adc0 = ads.readADC_SingleEnded(0);
    adc1 = ads.readADC_SingleEnded(1);
    adc2 = ads.readADC_SingleEnded(2);
    adc3 = ads.readADC_SingleEnded(3);
    //Serial.print("AIN0: "); Serial.print(adc0);
    //Serial.print(", AIN1: "); Serial.print(adc1);
    //Serial.print(", AIN2: "); Serial.print(adc2);
    //Serial.print(", AIN3: "); Serial.println(adc3);
    Mb.MBInputRegister[2] = adc2;
    Mb.MBInputRegister[3] = adc3;
    //analogWrite(ledpin,pwm++);
    //time_t now = time(nullptr);
    //Serial.println(ctime(&now));
    //digitalWrite(ledpin, HIGH);
    //pwm = pwm + 10;
    //analogWrite(ledpin,pwm);
    //analogWriteFreq(150000);
  }

  if (millis() - checkRSSIMillis >= 10000) {
    //freq = freq +1000;
    //analogWriteFreq(freq);
    //pwm = 0;
    checkRSSIMillis = millis();
    //digitalWrite(ledpin, LOW);
    Mb.MBInputRegister[0] = checkRSSI();
  }
}

/*****FUNZIONI*****/

byte checkRSSI() {
  byte quality;
  long rssi = WiFi.RSSI();
  if (rssi <= -100)
    quality = 0;
  else if (rssi >= -50)
    quality = 100;
  else
    rssi = rssi + 100;
  quality = byte(rssi * 2);

  return quality;
}


