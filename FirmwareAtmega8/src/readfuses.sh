#!/bin/bash
sudo avrdude -c usbasp -p m8 -U hfuse:r:hfuse.txt:h -U lfuse:r:lfuse.txt:h
