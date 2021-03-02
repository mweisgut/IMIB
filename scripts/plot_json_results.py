#!/usr/bin/env python3

from json_utils import json_to_dataframe
import json
import matplotlib.pyplot as plt
import numpy as np
import sys

old_name_replacements = {
    "Art": "ART",
    "BTreeGoogleAbseil": "Abseil B-Tree",
    "BTreeTlx": "B+ Tree TLX",
    "HashMapRobinHoodFlat": "RH Flat Map",
    "HashMapRobinHoodNode": "RH Node Map",
    "HashMapTslRobin": "TSL Robin Map",
    "HashMapTslSparse": "TSL Sparse Map",
    "SkipList": "Skip List",
    "SimpleVector": "Simple Vector",
    "SortedVector": "Sorted Vector",
    "UnorderedMap": "STD Hash Map",
}


def main():
    if len(sys.argv) < 2:
        exit("Usage: " + sys.argv[0] + " <json result file> [print]")

    print_only = False
    if len(sys.argv) == 3:
        if sys.argv[2] != "print":
            exit("Unsupported option: " + sys.argv[2])
        else:
            print_only = True

    with open(sys.argv[1]) as json_file:
        jdata = json.load(json_file)

    df = json_to_dataframe(jdata)

    # columns
    c_case_name = "cases.case_name"
    c_index_name = "cases.index_name"
    c_executions = "cases.executions"
    c_iter = "configuration.iterations"
    c_threads = "configuration.threads"
    c_duration = "cases.executions.duration_ns"
    c_index_size = "cases.executions.index_size_bytes"

    df = df.drop(columns=[c_executions, c_iter, c_threads])
    df.dropna(inplace=True)
    df.reset_index(drop=True, inplace=True)
    print(df.to_string())
    df_med = df.groupby([c_case_name, c_index_name], as_index=False).median()
    df_med.set_index(c_case_name, inplace=True)
    df_med.replace(0, np.nan, inplace=True)
    df_med[c_index_name] = df_med[c_index_name].replace(old_name_replacements)
    if print_only:
        print(df_med.to_string())
        exit()

    benchmark_cases = df_med.index.unique()

    # print(benchmark_cases)
    # print(df_med.loc[benchmark_cases[0]])

    for b_case in benchmark_cases:
        # duration
        plt.figure()
        case_df = df_med.loc[b_case]
        ax = case_df.plot.bar(x=c_index_name, y=c_duration, legend=False)
        ax.set_xlabel("Data structure")
        ax.set_ylabel("Duration in nanoseconds")
        plt.tight_layout()
        plt.savefig(b_case + "_duration.pdf")
        # memory footprint
        if not case_df[c_index_size].isnull().values.all():
            plt.clf()  # clear the entire current figure
            ax = case_df.plot.bar(x=c_index_name, y=c_index_size, legend=False)
            ax.set_xlabel("Data structure")
            ax.set_ylabel("Size in bytes")
            plt.tight_layout()
            plt.savefig(b_case + "_index_size.pdf")


main()
