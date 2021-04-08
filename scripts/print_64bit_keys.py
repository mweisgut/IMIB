#!/usr/bin/python

import numpy as np
import struct
import sys
import os

if len(sys.argv) != 2:
  print("Please pass the binary file of uint64 values")
  exit(1)

filename = sys.argv[1]

d = np.fromfile(filename, dtype=np.uint64)[1:]
print(d)
