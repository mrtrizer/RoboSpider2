#!/bin/bash
sudo avrdude -c usbasp -p m8 -U lfuse:w:l`cat fuse.txt`:m 
