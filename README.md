# Nixus
A heavy reworking based on the original software by Dominic Bumbaca.

## Schematic
The schematic in this repo is the only one I can find.
Mine might be a newer revision.

Additional pinouts:
```
Programming header:
1 GND
2 nc
3 VCC
4 RX   pin 2 (RXD/PD0/PCINT16)
5 TX   pin 3 (TXD/PD1/PCINT17)
6 nc

Header near crystal: (jumpered 1-2, 3-4)
1 ?    pin 17 (MOSI/PB3/OC2A/PCINT3)
2 GND
3 ?    pin 18 (MISO/PB4/PCINT4)
4 GND
```

## My notes for flashing
```
minipro -p ATMEGA328@DIP28 -E
minipro -p ATMEGA328@DIP28 -e -c data -f ihex -w Nixus.ino.eep
minipro -p ATMEGA328@DIP28 -e -c code -f ihex -w Nixus.ino.hex
minipro -p ATMEGA328@DIP28 -e -c config -w Nixus.ino.fuses.conf
```

## Some info
https://oshwlab.com/djbum66/6_digit_split_board-514e56bb6b50426f8f3e46bc48594656
