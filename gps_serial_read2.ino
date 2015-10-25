/* Originally posted on www.bbangpan.com
   Program Description: FM module test code
   Needed library : TinyGPS https://github.com/mikalhart/TinyGPS
   Tested on : Arduino 1.7.4, Arduino Mega ADK, blox Neo 6M GPS Module, TigyGPS
   
   Copyright (c) 2015 www.bbangpan.com. All rights reserved.
   This program can be used for any non-commercial purpose freely.   */

#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include "U8glib.h"
U8GLIB_PCD8544 u8g(A1, A2, A4, A3, A5);    // SPI Com: SCK,CLK = A0, MOSI,SDIN = A2, CS,SCE = A4, A0,DC = A3, Reset = A5

#define ONE_WIRE_BUS 5

char temperatureF[6];

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

float tempC = 0;
float tempF = 0;

//SoftwareSerial mySerial(3,4); // RX(<-TX of GPS module), TX(<-RX of GPS module)
TinyGPSPlus gps2;
TinyGPSCustom fix(gps2, "GPGSA", 2);
SoftwareSerial ss(4, 3);

void setup()  
{
  Serial.begin(9600);
  sensors.begin();
  // set the data rate for the SerialPort(use SoftwareSerial port on Arduino UNO)
  ss.begin(9600);  // Serial2->mySerial on Arduino UNO
  
  delay(2000);
  Serial.print("uBlox Neo 6M / TinyGPSPlus - ");
  Serial.println(TinyGPSPlus::libraryVersion());

  u8g.setFont(u8g_font_04b_03r);
  //u8g.drawFrame(2,2,82,46);
  u8g.firstPage();  
  do {
    u8g.setPrintPos(20,25);
    u8g.print("GPS Ready"); 
  } while( u8g.nextPage() );
}

void draw(float lat, float lon, String l_datey, String l_datem, String l_dated, String l_timeh, String l_timem, String l_times, 
          String f_alt, String f_speed, unsigned short sat, String temp) {
  
  u8g.setFont(u8g_font_04b_03r);
  u8g.firstPage();
  do {
      u8g.setFontPosTop();
      u8g.setPrintPos(0,0);
      u8g.print("POS:"); 
      u8g.print(lat);  // latitude
      //Serial.println(lat);
      u8g.setPrintPos(38,0);
      u8g.print("/"); 
      u8g.print(lon); // longitude
      //Serial.println(lon);
      String date = l_datey + "-" + l_datem + "-" + l_dated;
      u8g.setPrintPos(0,6);
      u8g.print("Date:");
      u8g.print(date); 
      //Serial.println(l_date);
      u8g.setPrintPos(0,12);
      u8g.print("Time:");
      String time = l_timeh + ":" + l_timem + ":" + l_times; 
      u8g.print(time); // time(hhmmsscc, UTC 0)
      //Serial.println(l_time);
      u8g.setPrintPos(0,18);
      u8g.print("Alt:");
      String altitude = f_alt + "m";
      u8g.print(altitude);
      //Serial.println(f_alt);
      u8g.setPrintPos(0,24);
      u8g.print("Speed:");
      String speed = f_speed + "kmph";
      u8g.print(speed); // speed(km per hour)
      //Serial.println(f_speed);
      u8g.setPrintPos(0,30);
      u8g.print("Satellites:"); 
      u8g.print(sat);
      //Serial.println(sat);
      u8g.setPrintPos(0,36);
      u8g.print("Temp:");
      String tempC = temp + "C"; 
      u8g.print(tempC);
      //Serial.println(temp);
      //u8g.setPrintPos(40,42);
      //u8g.print("Fix:");
      //u8g.print(fix.value());
      int f = atoi(fix.value());
      switch (f){
        case 3:
          u8g.setPrintPos(63,42);
          u8g.print("Fix:3D");
          break;
        case 2:
          u8g.setPrintPos(63,42);
          u8g.print("Fix:2D");
          break;
        case 1:
          u8g.setPrintPos(61,42);
          u8g.print("Fix:---");
          break;
      }
  } while( u8g.nextPage() );
}

void loop() // run over and over
{
  bool newdata = false;
  unsigned long start = millis();
  float flat, flon;
  unsigned short sat;

  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
  tempF = sensors.toFahrenheit(tempC);  
  convertToString(tempC);
  
  // Every 2 seconds we print an update
  while (millis() - start < 2000) {
    if (ss.available() && gps2.encode(ss.read())) {
        newdata = true;
        //break;  // uncomment to print new data immediately!
    }
  }
  
  if (newdata) {
    flat = gps2.location.lat();
    flon = gps2.location.lng();
    String datey = String(gps2.date.year());
    String datem = String(gps2.date.month());
    String dated = String(gps2.date.day());
    String timeh = String(gps2.time.hour());
    String timem = String(gps2.time.minute());
    String times = String(gps2.time.second());
    String falt = String(gps2.altitude.meters());
    String fspeed = String(gps2.speed.kmph());
    sat = gps2.satellites.value();
    draw(flat, flon, datey, datem, dated, timeh, timem, times, falt, fspeed, sat, temperatureF);
  }
}

void convertToString(float number)
{
   dtostrf(number, 3, 1, temperatureF);
}

