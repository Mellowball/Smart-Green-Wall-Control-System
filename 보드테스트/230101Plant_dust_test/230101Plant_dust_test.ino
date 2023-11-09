
#include<SoftwareSerial.h>
const int rxPin = 5;
const int txPin = 6;

const int T = 7;
const int R = 8;

//Delay 제외 -> millis()
unsigned long start_time, end_time, sum_time;

SoftwareSerial mySerial(rxPin, txPin);
SoftwareSerial SendSerial(R, T);
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

// PMS7003 변수
byte bytCount1 = 0; byte bytCount2 = 0;
unsigned char chrRecv; unsigned char chrData[30];

int PM01; int PM25; int PM10;

unsigned int GetPM_Data(unsigned char chrSrc[], byte bytHigh, byte bytLow) {
  return (chrSrc[bytHigh] << 8) + chrSrc[bytLow];
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.setTimeout(50);
//  SendSerial.begin(9600);
//  SendSerial.setTimeout(50);
}

void loop() {
    unsigned long nowtime = millis(); // Delay 없이
    start_time = millis();

  //pms7003 dust Sensor-------------------------------------------------------------------------
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };

  if (mySerial.available())   {
    for (int i = 0; i < 32; i++)     {
      chrRecv = mySerial.read();
      if (chrRecv == START_2 ) {
        bytCount1 = 2;
        break;
      }
    }

    if (bytCount1 == 2) {
      bytCount1 = 0;
      for (int i = 0; i < 30; i++) {
        chrData[i] = mySerial.read();
      }

      if ( (unsigned int) chrData[ERROR_CODE] == 0 ) {
        PM25  = GetPM_Data(chrData, PM2_5_ATMOSPHERE_H, PM2_5_ATMOSPHERE_L);
        PM10  = GetPM_Data(chrData, PM10_ATMOSPHERE_H, PM10_ATMOSPHERE_L);
        Serial.print("PM10 = ");
        Serial.print(PM10);

        Serial.print(", PM2.5 = ");
        Serial.println(PM25);

        if (PM10 < 30) {
          Serial.println("미세먼지 좋음");
        } else if (PM10 >= 30 && PM10 < 80) {
          Serial.println("미세먼지 보통");
        } else if (PM10 >= 80 && PM10 < 150) {
          Serial.println("미세먼지 나쁨");
        } else if (PM10 >= 150) {
          Serial.println("미세먼지 매우 나쁨");
        }

        if (PM25 < 15) {
          Serial.println("초미세먼지 좋음");
        } else if (PM25 >= 15 && PM25 < 35) {
          Serial.println("초미세먼지 보통");
        } else if (PM25 >= 35 && PM25 < 75) {
          Serial.println("초미세먼지 나쁨");
        } else if (PM25 >= 75) {
          Serial.println("초미세먼지 매우 나쁨");
        }
      }// if (unsigned int) chrData[ERROR_CODE] == 0
      else {
        Serial.println("PMS7003  ERROR");
      }
    }
  }//if(Serial1.available())
  else {
    Serial.println("PMS7003 NOT available");
  }
//  Txdata();
  Serial.print("@,");
  Serial.print(PM10);//
  Serial.print(",");
  Serial.print(PM25);//
  Serial.print(",");
  Serial.println(";");
  
  delay(900);

//    sum_time += end_time - start_time; // 끝나는 시간 - 시작 시간 = 루프 한번 돌때 시간
//    if (sum_time > 300) { // 700 이상 지났을 때 OLED로 데이터 보냄
//      Txdata(); // 데이터 보내기 -> OLED
//      sum_time = 0;
//      Serial.println("Tx보내는중");
//    }
}

void Txdata () {
  SendSerial.print("@,");
  SendSerial.print(PM10);//
  SendSerial.print(",");
  SendSerial.print(PM25);//
  SendSerial.print(",");
  SendSerial.println(";");
}
