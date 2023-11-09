#include <Adafruit_NeoPixel.h>
#define LED_PIN 2
#define LED_CON_PIN 3 // LED Relay Pin
#define LED_COUNT 60


Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long pixelPrevious = 0;        // Previous Pixel Millis
unsigned long patternPrevious = 0;      // Previous Pattern Millis
int           patternCurrent = 0;       // Current Pattern Number
int           patternInterval = 5000;   // Pattern Interval (ms)
int           pixelInterval = 50;       // Pixel Interval (ms)
int           pixelQueue = 0;           // Pattern Pixel Queue
int           pixelCycle = 0;           // Pattern Pixel Cycle
uint16_t      pixelCurrent = 0;         // Pattern Current Pixel Number
uint16_t      pixelNumber = LED_COUNT;  // Total Number of Pixels



int fan_speed = 0;
int flow_fan_pwm = A0;
int flow_fan1 = 12;
int flow_fan2 = 13;
int uv_state, neb_state, led_state;

//Relay 핀
int uv_port = 9; // 자외선
int neb_port = 10; // 미스트
// LED Relay Pin = 3 // define에서 선언

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  Serial1.setTimeout(50);
  Serial.begin(9600);
  Serial.setTimeout(50);
  pinMode(flow_fan_pwm, OUTPUT);
  pinMode(flow_fan1, OUTPUT);
  pinMode(flow_fan2, OUTPUT);

  pinMode(uv_port, OUTPUT);
  pinMode(neb_port, OUTPUT);

  pinMode(LED_CON_PIN, OUTPUT);
  digitalWrite(LED_CON_PIN, LOW);
  analogWrite(flow_fan_pwm, 0);

  //NEO_LED
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)



  uv_state = 0;
  neb_state = 0;
  led_state = 0;
  fan_speed = 0;
}

void loop() {


  readData();

  analogWrite(flow_fan_pwm, fan_speed);

  if (uv_state == 1) {
    digitalWrite(uv_port, HIGH);
  } else {
    digitalWrite(uv_port, LOW);
  }

  if (neb_state == 1) {
    digitalWrite(neb_port, HIGH);
  } else {
    digitalWrite(neb_port, LOW);
  }

  if (led_state == 1) {
    digitalWrite(LED_CON_PIN, HIGH);
    //    colorWipe(strip.Color(65, 165, 65), 50); // Green
        for (int i = 0; i < LED_COUNT; i++) {
          // strip.Color takes RGBW values, from 0,0,0 up to 255,255,255
          strip.setPixelColor(i, strip.Color(65, 165, 65, 1)); // white color.불이 점등
          strip.show();
          Serial.println(i);
        }
  } else {
    digitalWrite(LED_CON_PIN, LOW);
    //    colorWipe(strip.Color(0, 0, 0), 0); // Green
  }


  colorWipe(strip.Color(65, 165, 65), 50);
}// loop

//Communication with Due(Sensor Value)-------------------------------------------------
void readData() { // RTC, Sensor 보드에서 parseing 값 받아오는 함수
  // Data Parsing
  char input;
  if (Serial1.available()) {
    while (Serial1.available() > 20) {    //Data Bytes
      input = Serial1.read();
    }
    Serial1.readStringUntil('#');                     //Parsing Starter
    String inString = Serial1.readStringUntil(';');   //Parsing Terminater
    //@,0000,0000,0000,0000,;
    int i, count, strlength;
    strlength = inString.length();
    count = 0;
    for (i = 0; i < strlength; i++)
    {
      if (inString.charAt(i) == ',')      //Parsing Separater
        count++;
    }
    if (count == 5) {
      // data read
      int index1  = inString.indexOf(',');
      int index2  = inString.indexOf(',', index1 + 1);
      int index3  = inString.indexOf(',', index2 + 1);
      int index4  = inString.indexOf(',', index3 + 1);
      int index5  = inString.indexOf(',', index4 + 1);

      //Start  = inString.substring(0, index1).toInt();
      //0번째~
      uv_state = inString.substring(index1 + 1, index2 ).toInt();
      neb_state = inString.substring(index2 + 1, index3 ).toInt();
      led_state = inString.substring(index3 + 1, index4 ).toInt();
      fan_speed = inString.substring(index4 + 1, index5 ).toInt();

      // 확인용 출력
      Serial.print("#,");
      Serial.print(uv_state);
      Serial.print(",");
      Serial.print(neb_state);
      Serial.print(",");
      Serial.print(led_state);
      Serial.print(",");
      Serial.print(fan_speed);
      Serial.print(",");
      Serial.println(";");
    } //if (count == 2)
  } // if (Serial1.available())
} // void readPMS()


// NEO_PIX LED
void colorWipe(uint32_t color, int wait) {
  if (pixelInterval != wait)
    pixelInterval = wait;                   //  Update delay time
  strip.setPixelColor(pixelCurrent, color); //  Set pixel's color (in RAM)
  strip.show();                             //  Update strip to match
  pixelCurrent++;                           //  Advance current pixel
  if (pixelCurrent >= pixelNumber)          //  Loop the pattern from the first LED
    pixelCurrent = 0;
}
