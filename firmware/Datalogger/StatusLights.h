#include <Arduino.h>

#define STATUS_LED_0 8
#define STATUS_LED_1 7
#define STATUS_LED_2 6
#define STATUS_LED_3 5
#define STATUS_LED_4 4

void setup_status_lights() {
  pinMode(STATUS_LED_0, OUTPUT);
  pinMode(STATUS_LED_1, OUTPUT);
  pinMode(STATUS_LED_2, OUTPUT);
  pinMode(STATUS_LED_3, OUTPUT);
  pinMode(STATUS_LED_4, OUTPUT);
}

void fast_blinks(uint8_t light, uint8_t count) {
  for (int i = 0; i < count; i++ ){
    digitalWrite(light, HIGH);
    delay(100);
    digitalWrite(light, LOW);
    delay(200);
  }
}

void slow_blinks(uint8_t light, uint8_t count) {
  for (int i = 0; i < count; i++ ){
    digitalWrite(light, HIGH);
    delay(500);
    digitalWrite(light, LOW);
    delay(250);
  }
}

void start_up_lights() {
  digitalWrite(STATUS_LED_0, HIGH);
  delay(200);
  digitalWrite(STATUS_LED_0, LOW);
  delay(10);
  digitalWrite(STATUS_LED_1, HIGH);
  delay(200);
  digitalWrite(STATUS_LED_1, LOW);
  delay(10);
  digitalWrite(STATUS_LED_2, HIGH);
  delay(200);
  digitalWrite(STATUS_LED_2, LOW);
  delay(10);
  digitalWrite(STATUS_LED_3, HIGH);
  delay(200);
  digitalWrite(STATUS_LED_3, LOW);
  delay(10);
  digitalWrite(STATUS_LED_4, HIGH);
  delay(200);
  digitalWrite(STATUS_LED_4, LOW);
  delay(10);
}
