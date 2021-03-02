#!/usr/bin/env python3

import numpy as np
import struct
import sys

target_dir = "resources/data/"
np.random.seed(1337)


# generates base data sets with 'value_count' values. Additionally, subset_count - 1 additional sub-datasets
# are created for each base dataset. The sub-datasets are created in descending order of size.
# example: base data set has 100 values and the subset count is 10. Additionally to the base data set, which
# is counted as one subset, 9 additional subsets are created with the sizes 90, 80, 70, 60, 50, 40, 30, 20, 10.
def generate_and_store_uints(
    value_count, data_type, subset_count, equality_lookup_count, range_lookup_selectivities, range_lookup_count
):
    if data_type == np.uint32:
        max_uint = 4294967295
        # dtype_str = "uint32"
    elif data_type == np.uint64:
        max_uint = 18446744073709551615
        # dtype_str = "uint64"
    else:
        exit("The passed file does not contain 'uint32' or 'uint64' as suffix.")

    # DENSE DATA
    sorted_data = np.arange(start=1, stop=value_count + 1, dtype=data_type)
    create_and_save_subsets(
        sorted_data,
        subset_count,
        target_dir + "dense",
        equality_lookup_count,
        range_lookup_selectivities,
        range_lookup_count,
    )

    # SPARSE DATA
    rng = np.random.default_rng(1337)
    random_data = rng.integers(max_uint, size=int(value_count * 1.1), dtype=data_type)
    random_data = np.unique(random_data)
    assert len(random_data) > value_count
    sorted_data = random_data[:value_count]
    assert len(sorted_data) == value_count
    create_and_save_subsets(
        sorted_data,
        subset_count,
        target_dir + "sparse",
        equality_lookup_count,
        range_lookup_selectivities,
        range_lookup_count,
    )


def is_sorted_asc(a):
    for i in range(a.size - 1):
        if a[i + 1] < a[i]:
            return False
    return True


def is_sorted_dsc(a):
    for i in range(a.size - 1):
        if a[i + 1] > a[i]:
            return False
    return True


def generate_equality_lookups(data, count):
    return np.random.choice(data, count)


def generate_range_lookups(data, selectivity, count):
    print("selectivity", selectivity)
    range_lookup_values = []
    range_length = int(selectivity * len(data))
    print("range length for selectivity " + str(selectivity) + ": " + str(range_length))
    for iteration in range(count):
        idx_first = None
        idx_second = None
        while idx_second is None:
            idx_first = np.random.randint(len(data))
            if (idx_first + range_length - 1) < len(data):
                idx_second = idx_first + range_length - 1
            elif (idx_first - range_length + 1) >= 0:
                idx_second = idx_first - range_length + 1

        range_lookup_values.append(data[idx_first])
        range_lookup_values.append(data[idx_second])

    return np.array(range_lookup_values)


def create_and_save_subsets(
    sorted_data, subset_count, out_file_prefix, equality_lookup_count, range_lookup_selectivities, range_lookup_count
):
    subset_step_size = int(len(sorted_data) / subset_count)
    print("subset step size", subset_step_size)
    print("Generating " + str(subset_count) + " subsets (" + out_file_prefix + ")")
    for subset_index in range(subset_count):
        if subset_index > 0:
            sorted_data = sorted_data[:-subset_step_size]
        print("Generating subsets with size " + str(len(sorted_data)))
        assert is_sorted_asc(sorted_data)
        save_data(
            sorted_data, out_file_prefix + "_" + str(sorted_data.dtype) + "_" + str(len(sorted_data)) + "_asc_sorted"
        )

        eq_lookups = generate_equality_lookups(sorted_data, equality_lookup_count)
        save_data(
            eq_lookups,
            out_file_prefix + "_" + str(eq_lookups.dtype) + "_" + str(len(sorted_data)) + "_eq_" + str(len(eq_lookups)),
        )

        for selectivity in range_lookup_selectivities:
            rg_lookups = generate_range_lookups(sorted_data, selectivity, range_lookup_count)
            save_data(
                rg_lookups,
                out_file_prefix
                + "_"
                + str(rg_lookups.dtype)
                + "_"
                + str(len(sorted_data))
                + "_rg_"
                + str(len(rg_lookups))
                + "_select_"
                + str(selectivity),
            )

        dsc_sorted_data = np.copy(sorted_data)[::-1]
        # assert is_sorted_dsc(dsc_sorted_data)
        # save_data(
        #     dsc_sorted_data,
        #     out_file_prefix + "_" + str(dsc_sorted_data.dtype) + "_" + str(len(dsc_sorted_data)) + "_dsc_sorted",
        # )
        np.random.shuffle(dsc_sorted_data)
        save_data(
            dsc_sorted_data,
            out_file_prefix + "_" + str(dsc_sorted_data.dtype) + "_" + str(len(dsc_sorted_data)) + "_shuffled",
        )


def save_data(data, file_name):
    with open(file_name, "wb") as file:
        file.write(struct.pack("Q", len(data)))
        data.tofile(file)
    print("File was saved: " + file_name)


def main():
    if not len(sys.argv) in [6]:
        exit(
            "Usage: "
            + sys.argv[0]
            + " <data size> <subset count> <equality lookup count> <range lookup selectivities> <range lookup count>"
        )

    # parse selectivities
    selectivities = []
    for selectivity_str in sys.argv[4].split(","):
        selectivities.append(float(selectivity_str))

    generate_and_store_uints(
        int(sys.argv[1]),
        np.uint32,
        subset_count=int(sys.argv[2]),
        equality_lookup_count=int(sys.argv[3]),
        range_lookup_selectivities=selectivities,
        range_lookup_count=int(sys.argv[5]),
    )
    generate_and_store_uints(
        int(sys.argv[1]),
        np.uint64,
        subset_count=int(sys.argv[2]),
        equality_lookup_count=int(sys.argv[3]),
        range_lookup_selectivities=selectivities,
        range_lookup_count=int(sys.argv[5]),
    )


if __name__ == "__main__":
    main()
