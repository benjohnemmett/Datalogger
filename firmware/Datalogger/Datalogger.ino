/*
  This sketch reads data from a GPS module over UART and writes it to a file on an SD card

   SD card attached to SPI bus:
   ** MOSI - pin 11
   ** MISO - pin 12
   ** CLK - pin 13
   ** CS - pin 10 (PB2)
*/

#include <SPI.h>
#include <SD.h>
#include "GpsUtils.h"
#include "StatusLights.h"

#define CHIP_SELECT_PIN 10
#define CARD_DETECT_IN 9

String fileName = "gps_data_2.csv";
File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  setup_status_lights();
  
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
  delay(500);
}

void loop() {

  myFile = SD.open("gps_da2a.csv", FILE_WRITE);
  
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


String getStringEntry(String string, uint8_t entry_number) {
  int next_comma_index = 0;
  int last_comma_index;
  
  for (uint8_t i = 0; i < entry_number; i++) {
    last_comma_index = next_comma_index;
    next_comma_index = string.indexOf(',', last_comma_index+1);
  }

  last_comma_index = next_comma_index;
  next_comma_index = string.indexOf(',', last_comma_index+1);

  return string.substring(last_comma_index+1, next_comma_index);
}

bool stringIsGgaData(String gps_string) {
  return gps_string.startsWith(PREFIX_GNGGA);
}

bool gpsIsSynchronized(String gps_data) {
  if (!stringIsGgaData(gps_data)) {
    return false;
  }

  String quality_string = getStringEntry(gps_data, GGA_QUALITY_COL);
  long quality = quality_string.toInt();

  return quality > 0;
}
