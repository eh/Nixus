minipro -p ATMEGA328@DIP28 -E
minipro -p ATMEGA328@DIP28 -e -c data -f ihex -w ./build/arduino.avr.uno/Nixus.ino.eep
minipro -p ATMEGA328@DIP28 -e -c code -f ihex -w ./build/arduino.avr.uno/Nixus.ino.with_bootloader.hex
minipro -p ATMEGA328@DIP28 -e -c config -w ./Nixus.ino.fuses.conf
