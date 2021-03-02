#!/usr/bin/env python3

import numpy as np
import struct

np.random.seed(1337)
d = np.random.choice(range(16000000), size=10000, replace=False)

eq_lookups = np.random.choice(d, 100)
rg_lookups = np.random.choice(d, 100)

d32 = d.astype(dtype=np.uint32)
d64 = d.astype(dtype=np.uint64)
eq_lookups32 = eq_lookups.astype(dtype=np.uint32)
eq_lookups64 = eq_lookups.astype(dtype=np.uint64)
rg_lookups32 = rg_lookups.astype(dtype=np.uint32)
rg_lookups64 = rg_lookups.astype(dtype=np.uint64)

print("len(d32) = ", len(d32))
print("len(d64) = ", len(d64))
print("len(eq_lookups32) = ", len(eq_lookups32))
print("len(eq_lookups64) = ", len(eq_lookups64))
print("len(rg_lookups32) = ", len(rg_lookups32))
print("len(rg_lookups64) = ", len(rg_lookups64))

with open("random_10K_float_convertible_uint32", "wb") as f:
    f.write(struct.pack("Q", len(d32)))
    d32.tofile(f)

with open("random_10K_float_convertible_uint64", "wb") as f:
    f.write(struct.pack("Q", len(d64)))
    d64.tofile(f)

with open("random_10K_float_convertible_uint32_equality_lookups", "wb") as f:
    f.write(struct.pack("Q", len(eq_lookups32)))
    eq_lookups32.tofile(f)

with open("random_10K_float_convertible_uint64_equality_lookups", "wb") as f:
    f.write(struct.pack("Q", len(eq_lookups64)))
    eq_lookups64.tofile(f)

with open("random_10K_float_convertible_uint32_range_lookups", "wb") as f:
    f.write(struct.pack("Q", len(rg_lookups32)))
    rg_lookups32.tofile(f)

with open("random_10K_float_convertible_uint64_range_lookups", "wb") as f:
    f.write(struct.pack("Q", len(rg_lookups64)))
    rg_lookups64.tofile(f)
