#!/usr/bin/env python3

import glob
import pandas as pd
import subprocess
import sys

ITERATION_PER_CASE = 12


def run_benchmark(key_type, iterations, data_file, eq_lookup_file, rg_lookup_file, result_file_prefix):
    subprocess.run(
        ["./imiBench", key_type + "_t", str(iterations), data_file, eq_lookup_file, rg_lookup_file, result_file_prefix]
    )
    # subprocess.run(
    #     ["echo", key_type + "_t", str(iterations), data_file, eq_lookup_file, rg_lookup_file, result_file_prefix]
    # )


def main():
    checkout_commits = False
    TID_uint32_dense_commit = None
    TID_uint64_dense_commit = None
    TID_uint32_sparse_commit = None
    TID_uint64_sparse_commit = None

    if len(sys.argv) == 5:
        checkout_commits = True
        TID_uint32_dense_commit = sys.argv[1]
        TID_uint64_dense_commit = sys.argv[2]
        TID_uint32_sparse_commit = sys.argv[3]
        TID_uint64_sparse_commit = sys.argv[4]
    elif len(sys.argv) != 1:
        exit(
            "Usage: "
            + sys.argv[0]
            + " [<commit: 32 bit tid, dense> <commit: 64 bit tid, dense>"
            + " <commit: 32 bit tid, sparse> <commit: 64 bit tid, sparse>]"
        )

    # STEP 1: stored files with index in a dataframe
    tuples = []

    # example file names:
    # sparse_uint64_8000000_asc_sorted
    # sparse_uint64_8000000_eq_1000000
    # sparse_uint64_8000000_rg_2000000_select_0.0001
    # sparse_uint64_8000000_shuffled
    file_paths = sorted(glob.glob("../resources/data/*uint*"))
    for file_path in file_paths:
        file_name = file_path.split("/")[-1]
        fragments = file_name.split("_")
        tuples.append((fragments[0], fragments[1], int(fragments[2]), fragments[3], file_path))

    df = pd.DataFrame(tuples, columns=["distribution", "key_type", "size", "description", "file_path"])

    distributions = df["distribution"].unique()
    sizes = df["size"].unique()
    key_types = df["key_type"].unique()
    descriptions = df["description"].unique()

    df.set_index(["distribution", "size", "key_type", "description"], inplace=True)

    # print(df.to_string())

    sizes.sort()
    key_types[::-1].sort()
    print("distributions: ", distributions)
    print("sizes: ", sizes)
    print("key_types: ", key_types)
    print("descriptions: ", descriptions)

    # STEP 2: run benchmarks
    existing_result_file_prefixes = []
    existing_result_files = sorted(glob.glob("./*.json"))
    for file_name in existing_result_files:
        fragments = file_name.split("/")[1].split("_")
        existing_result_file_prefixes.append("_".join(fragments[:5]))

    for distribution in distributions:
        # currently supported key types: "uint32", "uint64"
        for value_type in ["uint64"]:
            if checkout_commits:
                if value_type == "uint32" and distribution == "dense":
                    subprocess.run(["git", "checkout", TID_uint32_dense_commit])
                elif value_type == "uint64" and distribution == "dense":
                    subprocess.run(["git", "checkout", TID_uint64_dense_commit])
                elif value_type == "uint32" and distribution == "sparse":
                    subprocess.run(["git", "checkout", TID_uint32_sparse_commit])
                elif value_type == "uint64" and distribution == "sparse":
                    subprocess.run(["git", "checkout", TID_uint64_sparse_commit])
                else:
                    exit(
                        "Fail: unsupported combination (value type|distribution): ("
                        + value_type
                        + "|"
                        + distribution
                        + ")"
                    )

            subprocess.run(["git", "submodule", "update", "--init", "--recursive"])
            subprocess.run(["ninja"])

            for ordering in ["shuffled", "asc"]:  # dsc is not included here.
                for key_type in key_types:
                    for size in sizes:
                        data_file = df.loc[(distribution, size, key_type, ordering)]["file_path"]
                        eq_lookup_file = df.loc[(distribution, size, key_type, "eq")]["file_path"]
                        rg_lookup_file = df.loc[(distribution, size, key_type, "rg")]["file_path"]
                        result_file_prefix = (
                            distribution + "_" + ordering + "_" + key_type + "_" + value_type + "_" + str(size).zfill(9)
                        )
                        if result_file_prefix in existing_result_file_prefixes:
                            print("skipped " + result_file_prefix)
                            continue
                        run_benchmark(
                            key_type, ITERATION_PER_CASE, data_file, eq_lookup_file, rg_lookup_file, result_file_prefix
                        )


if __name__ == "__main__":
    main()
