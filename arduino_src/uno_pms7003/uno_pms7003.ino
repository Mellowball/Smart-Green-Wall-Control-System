#include "PMS.h"
#include <SoftwareSerial.h>

//Serial USB로 통신, (0,1)
//Serial Serial1 Seria2 Seria3
//아두이노 우노에 2,3핀에 미세먼지센서(PMS7003)의 Tx,Rx핀이 순서대로 연결되어있다!

SoftwareSerial nockanda(2, 3); //rx=2, tx=3

PMS pms(nockanda);
PMS::DATA data;

void setup()
{
  Serial.begin(9600); //PC-아두이노간 통신(결과를 출력하기 위한)
  nockanda.begin(9600);  //아두이노-미세먼지센서간 통신
  //pms.activeMode(); //패시브모드로 사용하다가 액티브모드로 쓰는데 값이 안날라오면 주석풀기!
}

void loop()
{
  //미세먼지센서에서 데이터가 수신이된다면~~ 출력하겠다!
  if (pms.read(data))
  {
    Serial.print("@,");
    Serial.print(data.PM_AE_UG_1_0);//
    Serial.print(",");
    Serial.print(data.PM_AE_UG_2_5);//
    Serial.print(",");
    Serial.println(";");
  }
}
