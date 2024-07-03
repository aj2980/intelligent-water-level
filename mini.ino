#define BLYNK_TEMPLATE_ID "TMPL3W1FErNG-"
#define BLYNK_TEMPLATE_NAME "Water level monitoring system"
#define BLYNK_AUTH_TOKEN "MiI9LNGHqfuqMGZxCToctu7NG8srLAPh"

char ssid[] = "F14";
char pass[] = "paras14298";

int emptyTankDistance = 20;
int fullTankDistance = 5;
int triggerPer = 5;

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define TRIGPIN    4
#define ECHOPIN    2
#define wifiLed    21
#define ButtonPin1 12
#define BuzzerPin  5

#define VPIN_BUTTON_1    V1
#define VPIN_BUTTON_2    V2

float duration;
float distance;
int waterLevelPer;
bool toggleBuzzer = HIGH;

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

void checkBlynkStatus() {
  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true) {
    digitalWrite(wifiLed, HIGH);
  }
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_BUTTON_1);
  Blynk.syncVirtual(VPIN_BUTTON_2);
}

void measureDistance(){
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIGPIN, LOW);
  duration = pulseIn(ECHOPIN, HIGH);
  distance = ((duration / 2) * 0.343)/10;

  if (distance > (fullTankDistance - 10)  && distance < emptyTankDistance ){
    waterLevelPer = map((int)distance ,emptyTankDistance, fullTankDistance, 0, 100);

    Blynk.virtualWrite(VPIN_BUTTON_1, waterLevelPer);
    Blynk.virtualWrite(VPIN_BUTTON_2, (String(distance) + " cm"));

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (waterLevelPer < triggerPer){
      if (toggleBuzzer == HIGH){
        digitalWrite(BuzzerPin, HIGH);
      }      
    }
    if (distance < fullTankDistance){
      if (toggleBuzzer == HIGH){
        digitalWrite(BuzzerPin, HIGH);
      } 
    }

    if (distance > (fullTankDistance + 5) && waterLevelPer > (triggerPer + 5)){
      toggleBuzzer = HIGH;
      digitalWrite(BuzzerPin, LOW);
    }        
  }
  delay(100);
}

void setup() {
  Serial.begin(115200);
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(wifiLed, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);
  pinMode(ButtonPin1, INPUT_PULLUP);
  digitalWrite(wifiLed, LOW);
  digitalWrite(BuzzerPin, LOW);

  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus);
  Blynk.config(auth);
  delay(1000);
}
void loop() {
  measureDistance();
  Blynk.run();
  timer.run();
}