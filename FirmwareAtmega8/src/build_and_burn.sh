#!/bin/sh
if avr-gcc -mmcu=atmega8 -Os main.c -o test.o;
  then
    avr-objcopy -j .text -j .data -O ihex  test.o  test.hex
    avrdude -pm8 -cusbasp -U flash:w:test.hex;
fi
