// Relay2 OUTPUT
#define onoff 50
#define water_sensor 51
#define led 52
#define airuv 53

// 부저(피에조) 함수 -> Due에서 사용
#include "NewToneLib.h"
MyTone t(false);

#define buzzerPin 5 //selected pin
int num_tones = 8;
int tones[] = {261, 294, 330, 349, 392, 440, 494, 523, 587, 659, 698};

// Time -> millis
unsigned long start_time, end_time, sum_time;
unsigned long past, past_motor;
unsigned long looptime;

// button
#define onoff 22              // on/off 버튼

// 버튼 카운트
int onoff_count = 0;
int water_sensor_count = 0;
int led_count = 0;

void setup() {
  pinMode(led, OUTPUT);             pinMode(water_sensor, OUTPUT);
  digitalWrite(led, LOW);           digitalWrite(water_sensor, LOW);

  pinMode(airuv, OUTPUT);
  digitalWrite(airuv, LOW);

  musicnewyear(); // 피에조 부팅 사운드
}

void loop() {
  unsigned long nowtime = millis(); // 현재 시간과 과거 시간 비교해서 도어 모터 시간 정하기
  start_time = millis();

  if (digitalRead(onoff) == LOW) {}

  //////////water_senser btn
//  if (digitalRead(water_sensor_btn) == LOW) {
//    water_sensor_count += 1;
//    if (water_sensor_count == 1) t.tone(buzzerPin, tones[0], 300);
//    else if (water_sensor_count == 2) t.tone(buzzerPin, tones[7], 300);
//    if (water_sensor_count >= 3 ) {
//      water_sensor_count = 0; t.tone(buzzerPin, tones[3], 300);
//    }
//    Serial.println("water_sensor_btn");
//  }
}

void musicnewyear() {
  t.tone(buzzerPin, tones[0], 500); t.tone(buzzerPin, tones[2], 500);
  t.tone(buzzerPin, tones[4], 500); t.tone(buzzerPin, tones[7], 500);

}
