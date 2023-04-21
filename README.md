# Datalogger
ATMega328-based custom data logger hardware &amp; firmware files. Currently, this project reads location data from a GPS module and writes it to a .csv file on an SD card.

Details of design, testing, and issues can be found here https://notes.benjemmett.com/archives/375

## Warning!
This project is in a rough draft state and currently includes significant hardware design issues.

- Incorrect exposed pad connection in 5v linear regulator
- GPS connector pad order does not match the cable provided with the BN-220 GPS module
- Missing FTDI DTR pin connection for programming with the Arduino bootloader
- Incorrect SD Card detect wiring causes light to always be on

## Log File Format
The log file consists of the BN-220 data written directly to a csv file. Only the GGA entries are recorded.

The Log File name format consists of a record number, the date, and a file extension indicating the version number. The format is as follows, XXDDMMYY.cVV where:
- XX is a record number incremented on each startup
- DD is the day of the month
- MM is month
- YY is the year
- VV is a version number
For example, if the next record number is 42 and the current date is April 20, 2023 and the version number is 00 then the filename would be 42200423.c00

## Indicator Lights
The board includes 5 status LEDs numbered 0 to 4, where LED 0 is closest to the MCU. LED 0 indicates a critical error has occured.

Startup follows the sequence described and is indicated by the LED status lights.
1. Each LED blinks briefly in sequence from 0 to 4
1. LED_1 illumination indicates an SD card failure. Initialization is retried every 500ms. Check that the SD card is inserted correctly.
1. Three fast LED 1 blinks indicates successful SD card initialization.
1. LED_2 blinks indicate that GPS is being read and checked for synchronization before continuing.
1. LED_1 & LED_2 simultaneous blinks indicate that GPS data is being read until a valid date and time stamp is found.

During data collection
- LED_3 is illuminated while reading GPS data
- LED_4 is illuminated while writing data to the SD card
- LED_0 is illuminated if there is an SD card write error
