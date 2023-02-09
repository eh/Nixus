# Nixus
A heavy reworking based on the original software by Dominic Bumbaca.

## My notes for flashing
minipro -p ATMEGA328@DIP28 -E
minipro -p ATMEGA328@DIP28 -e -c data -f ihex -w Nixus.ino.eep
minipro -p ATMEGA328@DIP28 -e -c code -f ihex -w Nixus.ino.hex
minipro -p ATMEGA328@DIP28 -e -c config -w Nixus.ino.fuses.conf
