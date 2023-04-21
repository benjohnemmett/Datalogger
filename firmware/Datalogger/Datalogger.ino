/*
  This sketch reads data from a GPS module over UART and writes it to a file on an SD card

   SD card attached to SPI bus:
   ** MOSI - pin 11
   ** MISO - pin 12
   ** CLK - pin 13
   ** CS - pin 10 (PB2)
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

String file_name = "no_name.csv";
String file_extension = "c00"; // File extension corresponds with the firmware version
uint8_t record_number;
File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  setup_status_lights();

  uint8_t last_record_number = EEPROM.read(EEPROM_ADDR_RECORD_NUMBER);
  record_number = (last_record_number + 1) % MAX_RECORD_NUMBER;
  EEPROM.write(EEPROM_ADDR_RECORD_NUMBER, record_number);
  Serial.print("Beginning record number ");
  Serial.println(record_number);
  
  pinMode(CHIP_SELECT_PIN, OUTPUT);

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
      card_mounted = 1;
    }
  }
  
  fast_blinks(STATUS_LED_1, 3);
  delay(500);
  Serial.println("initialization done.");

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
    digitalWrite(STATUS_LED_1, status_toggle); // blink LED 1 & 2 until date & time are found
    digitalWrite(STATUS_LED_2, status_toggle);
    status_toggle = !status_toggle;
  }
  // There is an 8 char filename limit XXDDMMYY.cVV
  // https://forum.arduino.cc/t/sd-card-filename-issue/491760/4
  // XX is incremented on each startup to avoid naming overlap -> https://docs.arduino.cc/learn/programming/eeprom-guide
  // VV is a version number
  String date_time_string = getDateFromRmcString(data_string);
  file_name = getRecordNumberAsString() + date_time_string + "." + file_extension;
  Serial.print("Using filename ");
  Serial.println(file_name);
  
  digitalWrite(STATUS_LED_1, 0);
  digitalWrite(STATUS_LED_2, 0);
  delay(500);
}

void loop() {
  myFile = SD.open(file_name, FILE_WRITE);
  
  if (myFile) {
    digitalWrite(STATUS_LED_0, 0);
    digitalWrite(STATUS_LED_3, 1);
    String data_string = Serial.readStringUntil('\n');

    if (data_string.length() > 0) {
      if (stringIsGgaData(data_string)) {
        digitalWrite(STATUS_LED_4, 1);
        myFile.print(data_string);
        digitalWrite(STATUS_LED_4, 0);
      }
    }
    myFile.close();
    digitalWrite(STATUS_LED_3, 0);
  } else {
    digitalWrite(STATUS_LED_0, 1);
  }

  delay(10);
}

String getRecordNumberAsString() {
  String record_string = String(record_number);
  if (record_number < 10) {
    record_string = "0" + record_string;
  }
  return record_string;
}
