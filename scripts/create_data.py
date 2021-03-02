#!/usr/bin/env python3

import numpy as np
import os
import struct
import sys


def create_benchmark_data(file_name, value_count, equality_lookup_count, range_lookup_count):
    value_type = None
    file_name_parts = file_name.split("_")
    file_name_prefix = file_name_parts[0]
    value_type_suffix = file_name_parts[-1]

    print(file_name_prefix, value_type_suffix)

    if value_type_suffix == "uint32":
        value_type = np.uint32
    elif value_type_suffix == "uint64":
        value_type = np.uint64
    else:
        exit("The passed file does not contain 'uint32' or 'uint64' as suffix.")

    if not os.path.exists(file_name):
        exit("Passed file does not exist.")

    data = np.fromfile(file_name, dtype=value_type)[1:]
    if len(data) < value_count:
        exit("The passed data contains less values than the target value count.")
    data_cropped = data[:value_count]

    data_file_name = file_name_prefix + "_" + str(len(data_cropped)) + "_" + value_type_suffix
    with open(data_file_name, "wb") as file:
        file.write(struct.pack("Q", len(data_cropped)))
        data_cropped.tofile(file)

    eq_lookups = np.random.choice(data_cropped, equality_lookup_count)

    eq_lu_file_name = data_file_name + "_equality_lookups_" + str(len(eq_lookups))
    with open(eq_lu_file_name, "wb") as file:
        file.write(struct.pack("Q", len(eq_lookups)))
        eq_lookups.tofile(file)

    rg_lookups = np.random.choice(data_cropped, range_lookup_count)

    rg_lu_file_name = data_file_name + "_range_lookups_" + str(len(rg_lookups))
    with open(rg_lu_file_name, "wb") as file:
        file.write(struct.pack("Q", len(rg_lookups)))
        rg_lookups.tofile(file)


if len(sys.argv) != 5:
    exit(
        "Usage: "
        + sys.argv[0]
        + " <source file> <number of values> <number of equality lookups> <number of range lookups>"
    )

create_benchmark_data(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]))
