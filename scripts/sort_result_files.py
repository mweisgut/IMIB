#!/usr/bin/env python3

import os
import glob
import shutil

file_paths = sorted(glob.glob("*json"))

for fname in file_paths:
    fragments = fname.split("_")
    dir_name = "_".join(fragments[:4])
    if not os.path.exists(dir_name):
        os.makedirs(dir_name)

    shutil.move(fname, dir_name)

print("Done.")
exit(0)
