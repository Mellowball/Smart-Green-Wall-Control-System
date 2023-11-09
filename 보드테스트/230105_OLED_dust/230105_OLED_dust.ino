#include "PMS.h"
#include <SoftwareSerial.h>
const int rx = 13;
const int tx = 12;
SoftwareSerial DustSerial (rx, tx);

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// PMS7003
byte bytCount1 = 0; byte bytCount2 = 0;
unsigned char chrRecv; unsigned char chrData[30];

int PM01; int PM25; int PM10;

unsigned int GetPM_Data(unsigned char chrSrc[], byte bytHigh, byte bytLow) {
  return (chrSrc[bytHigh] << 8) + chrSrc[bytLow];
}

// PMS7003 ---------------
// The amount of time (in milliseconds) between tests
#define TEST_DELAY   1000
#define START_1 0x42
#define START_2 0x4d
#define DATA_LENGTH_H        0
#define DATA_LENGTH_L        1
#define PM1_0_ATMOSPHERE_H   8
#define PM1_0_ATMOSPHERE_L   9
#define PM2_5_ATMOSPHERE_H   10
#define PM2_5_ATMOSPHERE_L   11
#define PM10_ATMOSPHERE_H    12
#define PM10_ATMOSPHERE_L    13
#define PM2_5_PARTICLE_H   16
#define PM2_5_PARTICLE_L   17
#define VERSION              26
#define ERROR_CODE           27
#define CHECKSUM             29


String auto_val = "AUTO";
String off_val = "OFF";
String on_val = "ON";

int onoff_count = 0;
int fan_count = 0;
int water_time_count = 0;
int water_sensor_count = 0;
int led_count = 0;
int heat_count = 0;
int wateruv_count = 0;
int airuv_count = 0;
int due_temp = 0;
int due_humid = 0;
int due_pm10 = 0;
int due_pm25 = 0;

void setup() {
  Serial.begin(115200);
    DustSerial.begin(115200); //데이터 송신 전용
    Serial.setTimeout(50);
    DustSerial.setTimeout(50);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.setTextSize(1);
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE);
  Serial.println("셋업완료");
}

void loop() {
  //Loading page//////////////////////////////////////////////
  display.display();
  display.clearDisplay();

  ////  //main page//////////////////////////////////////////////
  display.setCursor(20, 0);
  display.print(F("Plant Air System"));

  display.setCursor(5, 10);
  display.print(F("Temp:"));
  display.setCursor(40, 10);
  display.print(due_temp);

  display.setCursor(69, 10);
  display.print(F("Humid:"));
  display.setCursor(104, 10);
  display.print(due_humid);

  display.setCursor(5, 19);
  display.print(F("PM10:"));
  display.setCursor(40, 19);
  display.print(due_pm10);

  display.setCursor(69, 19);
  display.print(F("PM25:"));
  display.setCursor(104, 19);
  display.print(due_pm25);

  display.setCursor(5, 28);
  display.print(F("T_Mst:"));
  if (water_time_count == 0) {
    display.setCursor(40, 28);
    display.print(auto_val);
  } else if (water_time_count == 1) {
    display.setCursor(40, 28);
    display.print(off_val);
  } else if (water_time_count == 2) {
    display.setCursor(40, 28);
    display.print(on_val);
  }

  display.setCursor(69, 28);
  display.print(F("S_Mst:"));
  if (water_sensor_count == 0) {
    display.setCursor(104, 28);
    display.print(auto_val);
  } else if (water_sensor_count == 1) {
    display.setCursor(104, 28);
    display.print(off_val);
  } else if (water_sensor_count == 2) {
    display.setCursor(104, 28);
    display.print(on_val);
  }

  display.setCursor(5, 37);
  display.print(F("LED1:"));
  if (led_count == 0) {
    display.setCursor(40, 37);
    display.print(auto_val);
  } else if (led_count == 1) {
    display.setCursor(40, 37);
    display.print(off_val);
  } else if (led_count == 2) {
    display.setCursor(40, 37);
    display.print(on_val);
  }

  display.setCursor(5, 46);
  display.print(F("W_Lam:"));
  if (wateruv_count == 0) {
    display.setCursor(40, 46);
    display.print(auto_val);
  } else if (wateruv_count == 1) {
    display.setCursor(40, 46);
    display.print(off_val);
  } else if (wateruv_count == 2) {
    display.setCursor(40, 46);
    display.print(on_val);
  }

  display.setCursor(69, 37);
  display.print(F("Heat:"));
  if (heat_count == 0) {
    display.setCursor(104, 37);
    display.print(auto_val);
  } else if (heat_count == 1) {
    display.setCursor(104, 37);
    display.print(off_val);
  } else if (heat_count == 2) {
    display.setCursor(104, 37);
    display.print(on_val);
  }
  display.setCursor(69, 46);
  display.print(F("A_Lam:"));
  if (airuv_count == 0) {
    display.setCursor(104, 46);
    display.print(off_val);
  } else if (airuv_count == 1) {
    display.setCursor(104, 46);
    display.print(on_val);
  }

  display.setCursor(35, 55);
  display.print(F("Fan:"));
  if (fan_count == 0) {
    display.setCursor(69, 55);
    display.print(auto_val);
  } else if (fan_count == 1) {
    display.setCursor(69, 55);
    display.print(off_val);
  } else if (fan_count == 2) {
    display.setCursor(69, 55);
    display.print(F("LOW"));
  } else if (fan_count == 3) {
    display.setCursor(69, 55);
    display.print(F("MID"));
  } else if (fan_count == 4) {
    display.setCursor(69, 55);
    display.print(F("HIGH"));
  }

//  //  //pms7003 dust Sensor-------------------------------------------------------------------------
//    uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
//  
//    if (DustSerial.available())   {
//      for (int i = 0; i < 32; i++)     {
//        chrRecv = DustSerial.read();
//        if (chrRecv == START_2 ) {
//          bytCount1 = 2;
//          break;
//        }
//      }
//  
//      if (bytCount1 == 2) {
//        bytCount1 = 0;
//        for (int i = 0; i < 30; i++) {
//          chrData[i] = DustSerial.read();
//        }
//  
//        if ( (unsigned int) chrData[ERROR_CODE] == 0 ) {
//          PM25  = GetPM_Data(chrData, PM2_5_ATMOSPHERE_H, PM2_5_ATMOSPHERE_L);
//          PM10  = GetPM_Data(chrData, PM10_ATMOSPHERE_H, PM10_ATMOSPHERE_L);
//          Serial.print("PM10 = ");
//          Serial.print(PM10);
//  
//          Serial.print(", PM2.5 = ");
//          Serial.println(PM25);
//  
//          if (PM10 < 30) {
//            Serial.println("미세먼지 좋음");
//          } else if (PM10 >= 30 && PM10 < 80) {
//            Serial.println("미세먼지 보통");
//          } else if (PM10 >= 80 && PM10 < 150) {
//            Serial.println("미세먼지 나쁨");
//          } else if (PM10 >= 150) {
//            Serial.println("미세먼지 매우 나쁨");
//          }
//  
//          if (PM25 < 15) {
//            Serial.println("초미세먼지 좋음");
//          } else if (PM25 >= 15 && PM25 < 35) {
//            Serial.println("초미세먼지 보통");
//          } else if (PM25 >= 35 && PM25 < 75) {
//            Serial.println("초미세먼지 나쁨");
//          } else if (PM25 >= 75) {
//            Serial.println("초미세먼지 매우 나쁨");
//          }
//        }// if (unsigned int) chrData[ERROR_CODE] == 0
//        else {
//          Serial.println("PMS7003  ERROR");
//        }
//      }
//    }//if(DustSerial.available())
//    else {
//      Serial.println("PMS7003 NOT available");
//    }
//    delay(900);
// 
} // loop
