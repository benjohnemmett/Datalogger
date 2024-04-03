/*
  This sketch reads data from a GPS module over UART and writes it to a file on an SD card

   SD card attached to SPI bus:
   ** MOSI - pin 11
   ** MISO - pin 12
   ** CLK - pin 13
   ** CS - pin 10 (PB2)

   DIP SWITCHES
   ** DIP1 - PD2 - Arduino pin 2
   ** DIP2 - PD3 - Arduino pin 3

   Arduino IDE Using USBasp programmer
    - Arduino
        - Board -> Arduino Uno
        - Port -> None
        - Programmer -> USBasp
    - Sketch -> Upload using programmer
        - Cmd + Shift + U
*/

#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>
#include "GpsUtils.h"
#include "StatusLights.h"

#define CHIP_SELECT_PIN 10
#define CARD_DETECT_IN 9
#define EEPROM_ADDR_RECORD_NUMBER 0
#define MAX_RECORD_NUMBER 100

#define DIP_1 3
#define DIP_2 2

enum WriteMode { WRITE_ALL, ONLY_GGA };

String file_name = "no_name.csv";
String file_extension = "csv";
uint8_t record_number;
File myFile;
WriteMode write_mode = ONLY_GGA;

void setup() {
  Serial.begin(9600);
  setup_status_lights();

  uint8_t last_record_number = EEPROM.read(EEPROM_ADDR_RECORD_NUMBER);
  record_number = (last_record_number + 1) % MAX_RECORD_NUMBER;
  EEPROM.write(EEPROM_ADDR_RECORD_NUMBER, record_number);
  Serial.print("Beginning record number ");
  Serial.println(record_number);
  
  pinMode(CHIP_SELECT_PIN, OUTPUT);
  
  if (getDipOneValue()) {
    Serial.println("write_mode = WRITE_ALL");
    fast_blinks(STATUS_LED_3, 3);
    write_mode = WRITE_ALL;
  }

  if (getDipTwoValue()) {
    fast_blinks(STATUS_LED_4, 3);
  }

  start_up_lights();
  delay(500);

  bool card_mounted = false;
  while(!card_mounted) {
    Serial.println("Initializing SD card...");
    if (!SD.begin(CHIP_SELECT_PIN)) {
      digitalWrite(STATUS_LED_1, HIGH);
      Serial.println("initialization failed!");
      
      delay(500);
      digitalWrite(STATUS_LED_1, LOW);
    } else {
      card_mounted = true;
    }
  }
  
  fast_blinks(STATUS_LED_1, 3);
  delay(500);
  Serial.println("initialization done.");

  if (write_mode == ONLY_GGA) {
    Serial.println("Getting GPS data..");
    uint8_t status_toggle = 1;
    String data_string = Serial.readStringUntil('\n');
    while(!gpsIsSynchronized(data_string)) {
      data_string = Serial.readStringUntil('\n');
      digitalWrite(STATUS_LED_2, status_toggle);
      status_toggle = !status_toggle;
    }
  
    data_string = Serial.readStringUntil('\n');
    while (!stringIsRmcData(data_string)) {
      data_string = Serial.readStringUntil('\n');
      digitalWrite(STATUS_LED_1, status_toggle); // Blink LED 1 & 2 until date & time are found
      digitalWrite(STATUS_LED_2, status_toggle);
      status_toggle = !status_toggle;
    }
    
    // There is an 8 char filename limit DDMMYYXX.cVV
    // https://forum.arduino.cc/t/sd-card-filename-issue/491760/4
    // VV is the version number
    String date_time_string = getDateFromRmcString(data_string);
    file_name = date_time_string + getRecordNumberAsString() + "." + file_extension;
  }
  else {
    file_name = "data_" + getRecordNumberAsString() + "." + file_extension;
  }
  Serial.print("Using filename ");
  Serial.println(file_name);
  
  digitalWrite(STATUS_LED_1, 0);
  digitalWrite(STATUS_LED_2, 0);
  delay(500);
}

void loop() {
  String data_string = Serial.readStringUntil('\n');
  digitalWrite(STATUS_LED_0, 0);
  digitalWrite(STATUS_LED_3, 1);

  switch (write_mode) {
    case WRITE_ALL:
      writeDataToCard(data_string);
      break;
    case ONLY_GGA:
      if (stringIsGgaData(data_string)) {
        writeDataToCard(data_string);
      }
      break;
  }

  digitalWrite(STATUS_LED_3, 0);
  delay(10);
}

void writeDataToCard(String data_string) {
  myFile = SD.open(file_name, FILE_WRITE);
  if (myFile) {
    if (data_string.length() > 0) {
      digitalWrite(STATUS_LED_4, 1);
      myFile.print(data_string);
      digitalWrite(STATUS_LED_4, 0);
    }
    myFile.close();
  } else {
    digitalWrite(STATUS_LED_0, 1);
  }
}

String getRecordNumberAsString() {
  String record_string = String(record_number);
  if (record_number < 10) {
    record_string = "0" + record_string;
  }
  return record_string;
}

uint8_t getDipOneValue() {
  return !digitalRead(DIP_1);
}

uint8_t getDipTwoValue() {
  return !digitalRead(DIP_2);
}
