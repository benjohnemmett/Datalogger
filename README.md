# Datalogger
ATMega328-based custom data logger hardware &amp; firmware files. Currently, this project reads location data from a GPS module and writes it to a .csv file on an SD card. It is designed to work with a BN-220 GPS module.

Details of design, testing, and issues can be found here https://notes.benjemmett.com/archives/375

## Warning!
This project is in a rough draft state and currently includes significant hardware design issues.

- Incorrect exposed pad connection in 5v linear regulator
- GPS connector pad order does not match the cable provided with the BN-220 GPS module
- Missing FTDI DTR pin connection for programming with the Arduino bootloader
- Incorrect SD Card detect wiring causes light to always be on

## Log File Format
The log file consists of the BN-220 data written directly to a csv file. It can operate in two modes: 
1. Record only GGA messages
2. Record all messages

The Log File name format consists of a the date and a record number followed by the ".csv" extension. The format is as follows, DDMMYYXX.csv where:
- DD is the day of the month
- MM is month
- YY is the year
- XX is a record number incremented on each startup in the range [0-99]
For example, if the next record number is 42 and the current date is April 20, 2023 and the version number is 00 then the filename would be 20042342.csv

## Indicator Lights
The board includes 5 status LEDs numbered 0 to 4, where LED 0 is closest to the MCU. LED 0 indicates a critical error has occured.

Startup sequence is as follows:
1. Indicate DIP switch position.
  1. If DIP 1 is enabled, LED_3 blinks 3 times
  1. If DIP 2 is enabled, LED_4 blinks 3 times
1. Each LED blinks one time briefly in order from LED_0 to LED_4
1. Slow LED_1 toggling indicates an SD card failure. SD Card initialization is retried every 500ms until successful.
1. Three fast LED_1 blinks indicates successful SD card initialization.
1. LED_2 blinks indicate that GPS is being read and checked for synchronization before continuing.
1. LED_1 & LED_2 simultaneous blinks indicate that GPS data is being read until a valid date and time stamp is found.

During data collection:
- LED_3 is illuminated while reading GPS data
- LED_4 is illuminated while writing data to the SD card
- LED_0 is illuminated if there is an SD card write error
