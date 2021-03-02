#!/usr/bin/env python3

# call plot_results of plot_multi_json_results.py in each sub directory of the current directory.

import os
from plot_multi_json_results import plot_results

directories = [os.path.realpath(x[0]) for x in os.walk("./")]
directories.remove(os.path.realpath("./"))

for dir_name in directories:
    if not dir_name.rsplit("/", 1)[1].startswith(("dense", "sparse")):
        print("continue", dir_name)
        continue
    os.chdir(dir_name)
    print("Changed working dir to " + dir_name)
    plot_results()

exit(0)
