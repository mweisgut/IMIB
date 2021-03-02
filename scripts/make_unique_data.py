#!/usr/bin/env python3

import numpy as np
import struct
import os

data_set_dir = "resources/data/"

# number of unique values per data set:
# ('books_unique_values_uint32', 147143363)
# ('fb_unique_values_uint64', 200000000)
# ('fb_unique_values_uint32', 200000000)
# ('osm_cellids_unique_values_uint64', 199946728)
# ('wiki_ts_unique_values_uint64', 90437011)
# thus, we use a uniform length of 90M

np.random.seed(1337)


def make_unique(file_name):
    if not os.path.exists(data_set_dir + file_name + "_90M_uint64") and os.path.exists(
        data_set_dir + file_name + "_200M_uint64"
    ):
        d = np.fromfile(data_set_dir + file_name + "_200M_uint64", dtype=np.uint64)[1:]
        ud = np.unique(d)
        print(file_name, "unique values", "uint64", len(ud))
        np.random.shuffle(ud)
        u90m = d[:90000000]
        print(file_name, "unique shuffled values", "uint64", len(u90m))

        with open(data_set_dir + file_name + "_90M_uint64", "wb") as file:
            file.write(struct.pack("Q", len(u90m)))
            u90m.tofile(file)

    if not os.path.exists(data_set_dir + file_name + "_90M_uint32") and os.path.exists(
        data_set_dir + file_name + "_200M_uint32"
    ):
        d = np.fromfile(data_set_dir + file_name + "_200M_uint32", dtype=np.uint32)[1:]
        ud = np.unique(d)
        print(file_name, "unique values", "uint32", len(ud))
        np.random.shuffle(ud)
        u90m = d[:90000000]
        print(file_name, "unique shuffled values", "uint32", len(u90m))

        with open(data_set_dir + file_name + "_90M_uint32", "wb") as file:
            file.write(struct.pack("Q", len(u90m)))
            u90m.tofile(file)


make_unique("books")
make_unique("fb")
make_unique("osm_cellids")
make_unique("wiki_ts")
