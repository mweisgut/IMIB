#!/usr/bin/env python3

from json_utils import json_to_dataframe
import json
import sys


def main():
    if len(sys.argv) != 3:
        exit("Usage: " + sys.argv[0] + " <json input file> <csv output file>")

    with open(sys.argv[1]) as json_file:
        jdata = json.load(json_file)

    df = json_to_dataframe(jdata)
    df = df.drop(columns=["cases.executions"])
    df.dropna(inplace=True)
    df.reset_index(drop=True, inplace=True)
    df.to_csv(sys.argv[2])


main()
