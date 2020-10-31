# Mitchell Humphrey's Different Types of Compression

## p8a

first byte, longest code length
next byte, amount of items at code length 1
next byte, amount of items at code length 2
repeat until all values from 0 to first byte are stored

add zeros till first part to hear is full bytes

character, then code, looped from smallest to largest

amount of filler bytes at end of compressed message

compressed message

filler bits

## p8a-python

First implementation I wrote of p8a comrpession, writen in python. Really slow. Only worth looking at if you hate Cpp code.

# Other Libraries used

Uses [tclap](http://tclap.sourceforge.net/) for flag setting for p8b and above.

