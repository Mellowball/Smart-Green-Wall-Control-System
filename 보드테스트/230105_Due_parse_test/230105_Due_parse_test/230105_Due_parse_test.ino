#include <DHT.h>

//#include "Nextion.h"
// CONNECTIONS:
// DS1302 CLK/SCLK --> 6
// DS1302 DAT/IO --> 5
// DS1302 RST/CE --> 4
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

#include <ThreeWire.h>
#include <RtcDS1302.h>

ThreeWire myWire(5, 6, 4); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

#define countof(a) (sizeof(a) / sizeof(a[0]))

//SoftwareSerial Serial1(2, 3);  // Nextion 시리얼 신호


#define DHTPIN 7    // Signal핀과 연결된 디지털 핀 넘
#define DHTTYPE DHT11   // DHT11 사용 명시

DHT dht(DHTPIN, DHTTYPE); // 사용핀넘버 타입 등록

bool buttonState = 0;
String inCommand;

uint8_t i = 12;

void ht();
int randNumber;
int picNum;

int data_length = 7;
byte received_data[7];
int button_length = 4;
byte received_button[4];
int switch_pos;
int first_data;

int fan_speed = 0;
int flow_fan_pwm = 0;
int flow_fan1 = 0;
int flow_fan2 = 0;


//SoftwareSerial pmsSerial(10, 11);
int PM01; int PM25; int PM10;

//SoftwareSerial actSerial(10, 11);
// Time -> millis
int a = 0;
unsigned long start_time, end_time, sum_time;
unsigned long past, past_motor;
unsigned long looptime;

void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);
//  actSerial.begin(9600);
//  pmsSerial.begin(9600);

  dht.begin(); // DHT 초기화
  while (!Serial) {  }             // 시리얼 포트가 연결될 때까지 기다림
  Serial.println("시리얼 포트 연결됨");
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  // 기본값 설정, 값 저장 기능 도입 시 활용
  Serial1.print(F("tTemp.txt="));
  Serial1.print("\"");
  Serial1.print(0);
  Serial1.print("\"");
  Serial1.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial1.write(0xff);
  Serial1.write(0xff);

  Serial1.print(F("tHumi.txt="));
  Serial1.print("\"");
  Serial1.print(0);
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);

  Serial1.print(F("tDust.txt="));
  Serial1.print("\"");
  Serial1.print(0);
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);

  Serial1.print(F("tUltDust.txt="));
  Serial1.print("\"");
  Serial1.print(0);
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);

  Serial1.print(F("pState.pic=")); // 그림 pic 값 설정은 \"를 붙이지 않는다
  Serial1.print("\"");
  Serial1.print(6);
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);

  Serial1.print(F("tTime.txt="));
  Serial1.print("\"");
  Serial1.print(0);
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
}

void loop() {
  //  unsigned long nowtime = millis(); // 현재 시간과 과거 시간 비교해서 도어 모터 시간 정하기
  //  start_time = millis();
  


  //  end_time = millis();
  //  sum_time += end_time - start_time; // 끝나는 시간 - 시작 시간 = 루프 한번 돌때 시간
  //  if (sum_time > 300) { // 700 이상 지났을 때 OLED로 데이터 보냄
  Serial.println("========================================");
  readPMS(); //PMS 데이터 읽기

  RtcDateTime now = Rtc.GetDateTime();   // 시간
  printDateTime(now);
  Serial.println();

  int hum = dht.readHumidity(); // 습도 읽기
  int temp = dht.readTemperature(); // 온도 읽기
  if (isnan(hum) || isnan(temp))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (Serial1.available() > 0) {                   // Nextion LCD로부터 데이타가 들어오는 경우
    Serial.println("데이터 수신");
    switch_control();
  }

  // 결과 시리얼 출력

  Serial1.print(F("tHumi.txt=")); // Changing the value of box tHumi
  Serial1.print("\"");
  Serial1.print(hum);
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);

  Serial.print(F("tHumi.txt="));
  Serial.print("\"");
  Serial.print(hum);
  Serial.print("\"");
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  Serial.println();

  Serial1.print(F("tTemp.txt="));
  Serial1.print("\"");
  Serial1.print(temp);
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);

  Serial.print(F("tTemp.txt="));
  Serial.print("\"");
  Serial.print(temp);
  Serial.print("\"");
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  Serial.println();

  Serial1.print(F("tDust.txt="));
  Serial1.print("\"");
  Serial1.print(PM10);
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);

  Serial.print(F("tDust.txt="));
  Serial.print("\"");
  Serial.print(PM10);
  Serial.print("\"");
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  Serial.println();

  Serial1.print(F("tUltDust.txt="));
  Serial1.print("\"");
  Serial1.print(PM25);
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);

  Serial.print(F("tUltDust.txt="));
  Serial.print("\"");
  Serial.print(PM25);
  Serial.print("\"");
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  Serial.println();
  delay(900);

  //    txdata();
  //    sum_time = 0;
  //    Serial.println("Tx보내는중");
  //  }
}

void switch_control() {
  int size = Serial1.readBytesUntil('\n', received_data, data_length);

  Serial.println("++++++++received_data++++++++");
  for (int i = 0; i < data_length; i++) {
    Serial.println(received_data[i]);
  }

  first_data = received_data[0];
  if (first_data = 101) {
    int size = Serial1.readBytesUntil('\n', received_button, button_length);
    for (int i = 0; i < button_length; i++) {
      Serial.println(received_button[i]);
    }

    if (received_data[2] == 19) {               // 자외선 버튼 클릭 시
      if (received_button[0] == 1) {
        Serial.println("LED ON");
        digitalWrite(12, 1);
      } else if (received_button[0] == 0) {
        Serial.println("LED OFF");
        digitalWrite(12, 0);
      }
    }
    else if (received_data[2] == 20) {          // 분무기 버튼 클릭 시
      if (received_button[0] == 1) {
        Serial.println("LED ON");
        digitalWrite(13, 1);
      } else if (received_button[0] == 0) {
        Serial.println("LED OFF");
        digitalWrite(13, 0);
      }
    }
    else if (received_data[2] == 31) {          // LED 버튼 클릭 시
      if (received_button[0] == 1) {
        Serial.println("LED ON");
        digitalWrite(13, 1);
      } else if (received_button[0] == 0) {
        Serial.println("LED OFF");
        digitalWrite(13, 0);
      }
    }
    else if (received_data[2] == 32) {          // 팬 약 버튼 클릭 시
      fan_speed = 130;
      digitalWrite(flow_fan1, HIGH);
      digitalWrite(flow_fan2, LOW);
      analogWrite(flow_fan_pwm, fan_speed);
      Serial.print("Flow Fan Speed = ");
      Serial.println(fan_speed);
    }
    else if (received_data[2] == 33) {          // 팬 중 버튼 클릭 시
      fan_speed = 180;
      digitalWrite(flow_fan1, HIGH);
      digitalWrite(flow_fan2, LOW);
      analogWrite(flow_fan_pwm, fan_speed);
      Serial.print("Flow Fan Speed = ");
      Serial.println(fan_speed);
    }
    else if (received_data[2] == 34) {          // 팬 강 버튼 클릭 시
      fan_speed = 255;
      digitalWrite(flow_fan1, HIGH);
      digitalWrite(flow_fan2, LOW);
      analogWrite(flow_fan_pwm, fan_speed);
      Serial.print("Flow Fan Speed = ");
      Serial.println(fan_speed);
    }
    else if (received_data[2] == 35) {          // 팬 OFF 버튼 클릭 시
      fan_speed = 0;
      analogWrite(flow_fan_pwm, fan_speed);
      Serial.print("Flow Fan Speed = ");
      Serial.println(fan_speed);
    }
  }
}

void printDateTime(const RtcDateTime& dt)
{
  char datestring[30];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%04u년%02u월%02u일 %02u:%02u:%02u"),
             dt.Year(),
             dt.Month(),
             dt.Day(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  Serial.print(datestring);
  Serial1.print(F("tTime.txt=")); // Changing the value of box tTime
  Serial1.print("\"");
  Serial1.print(datestring);
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
}

//Communication with Due(Sensor Value)-------------------------------------------------
//void readPMS() { // RTC, Sensor 보드에서 parseing 값 받아오는 함수
//  // Data Parsing
//  char input;
//  if (pmsSerial.available()) {
//    while (pmsSerial.available() > 8) {    //Data Bytes
//      input = pmsSerial.read();
//    }
//    pmsSerial.readStringUntil('@');                     //Parsing Starter
//    String inString = pmsSerial.readStringUntil(';');   //Parsing Terminater
//    //@,0000,0000,0000,0000,;
//    int i, count, strlength;
//    strlength = inString.length();
//    count = 0;
//    for (i = 0; i < strlength; i++)
//    {
//      if (inString.charAt(i) == ',')      //Parsing Separater
//        count++;
//    }
//    if (count == 2) {
//      // data read
//      int index1  = inString.indexOf(',');
//      int index2  = inString.indexOf(',', index1 + 1);
////      int index3  = inString.indexOf(',', index2 + 1);
//
//      //Start  = inString.substring(0, index1).toInt();
//      //0번째~
//      PM10 = inString.substring(index1 + 1, index2 ).toInt();
////      PM25 = inString.substring(index2 + 1, index3 ).toInt();
//
//      // 확인용 출력
//      Serial.print("PM10 :");
//      Serial.print(PM10);
////      Serial.print(" PM25 :");
////      Serial.print(PM25);
//      Serial.println(";");
//    } //if (count == 2)
//  } // if (pmsSerial.available())
//} // void readPMS()

//void txdata() { // OLED로 데이터 송신 (Parse)
//  actSerial.print("@,");
//  actSerial.print("");
//  actSerial.print(",");
//  actSerial.println(";");
//}
