#!/usr/bin/env python2.7

#######
## bindump_to_list.py
## (c) 2016 David Gschwend
##
## Usage: python2.7 bindump_to_list.py file.bin
#######

from __future__ import print_function   # print without newline
import os
import argparse
import struct
import time
import numpy as np
import math

# Parse arguments

parser = argparse.ArgumentParser(description='Print a binary float32 memory dump in human-readable format')

### Positional arguments
parser.add_argument('bin_file', help='Binary dump of float32 memory region')

### Optional arguments
parser.add_argument('-x', '--hex', action='store_true', help='Use Hex Address')
parser.add_argument('-c', '--cols', type=int, help='Number of Columns', default=1)
parser.add_argument('-p', '--precision', type=int, help='Number of Places after Comma', default=3)
parser.add_argument('-i', '--intwidth', type=int, help='Number of Places before Comma', default=3)

args = vars(parser.parse_args())

filename = args["bin_file"]
addrformat = "X" if args["hex"] else "d"
cols = args["cols"]
precision = args["precision"]
intwidth = args["intwidth"]

# Read Binary Contents
print("Using input file: {}".format(filename))
binary = ""
try:
    with open(filename, "rb") as f:
        binary = f.read();
except:
    print("Could not open file {}".format(filename))
    raise

# Interpret Binary File as List of Floats
num_floats = len(binary)/struct.calcsize('f')
print("Interpreting as {} float32 values".format(num_floats))

# Convert to List of Floats
floats = []
try: 
    floats = struct.unpack('f'*num_floats, binary)
except:
    print("Could not convert to floats!")
    raise
    
# Print to stdout
printbase = (16 if addrformat=="X" else 10)
addrwidth = max(int(math.ceil(math.log(4*num_floats, printbase))), 6)
datawidth = intwidth + precision + 1

hdrformat = "{:%ds} {:>%ds}"%(addrwidth, datawidth)
addrformat = "\n{:0%d%s} "%(addrwidth, addrformat)
dataformat = "{:%d.%df} "%(datawidth, precision)

print("\nValue Dump:\n")

print(hdrformat.format("ADDR", "VALUES"))
addr = 0
for f in floats:
    if (addr % cols == 0):
        print(addrformat.format(addr*4), end="")
    print(dataformat.format(f), end="")
    addr += 1
