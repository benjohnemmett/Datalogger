# Datalogger
ATMega328-based custom data logger hardware &amp; firmware files. Currently, this project reads location data from a GPS module and writes it to a .csv file on an SD card.

Details of design, testing, and issues can be found here https://notes.benjemmett.com/archives/375

## Warning!
This project is in a rough draft state and currently includes significant hardware design issues.

- Incorrect exposed pad connection in 5v linear regulator
- GPS connector pad order does not match the cable provided with the BN-220 GPS module
- Missing FTDI DTR pin connection for programming with the Arduino bootloader
- Incorrect SD Card detect wiring causes light to always be on
