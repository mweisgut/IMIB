# In-Memory Index Benchmark (IMIB)
IMIB is a benchmark framework for evaluating single-attribute in-memory secondary indexes in their lookup speed, maintenance cost, and memory consumption.
It contains various benchmark cases explicitly implemented to evaluate the latency of equality and range lookups, inserts, deletes, and bulk operations (bulk inserts and bulk loads) of included index implementations.
Furthermore, the benchmark cases for evaluating inserts, bulk inserts, and bulk loads also measure the corresponding index implementation's allocated memory.

## Included Implementations
| Implementation     |   Data structure  | C++ class                        |
|--------------------|:-----------------:|----------------------------------|
| [Unsync ART](https://github.com/flode/ARTSynchronized)     |     radix tree    | `ART_unsynchronized::Tree`       |
| [MP Judy](https://github.com/mpictor/judy-template)        |     radix tree    | `judyLArray`                     |
| [TLX B+ Tree](https://github.com/tlx/tlx)    |      B+ tree      | `tlx::btree_map`                 |
| [Abseil B-Tree](https://github.com/abseil/abseil-cpp)  |       B-tree      | `absl::btree_map`                |
| [BB-Tree](https://github.com/flippingbits/bb-tree)        | k-ary search tree | `BBTree`                         |
| [PG Skip List](https://github.com/petegoodliffe/skip_list)   |     skip list     | `goodliffe::multi_skip_list`     |
| [RH Flat Map](https://github.com/martinus/robin-hood-hashing)    |      hash map     | `robin_hood::unordered_flat_map` |
| [RH Node Map](https://github.com/martinus/robin-hood-hashing)    |      hash map     | `robin_hood::unordered_node_map` |
| [TSL Robin Map](https://github.com/Tessil/robin-map)  |      hash map     | `tsl::robin_map`                 |
| [TSL Sparse Map](https://github.com/Tessil/sparse-map) |      hash map     | `tsl::sparse_map`                | 
| [STD Hash Map](https://en.cppreference.com/w/cpp/container/unordered_map)   |      hash map     | `std::unordered_map`             |

## Requirements
- [`clang`](https://clang.llvm.org) compiler (tested with version 9)
- [`cmake`](https://cmake.org) (minimum 3.9)
- [`ninja`](https://ninja-build.org)
- [`jemalloc`](http://jemalloc.net/)
- [`autoconf`](https://www.gnu.org/software/autoconf/)

The *Unsync ART* additionally requires the [`tbb`](https://github.com/oneapi-src/oneTBB) library.

## Build Instructions
```
git submodule update --init --recursive
mkdir build
cd build/
cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -GNinja
ninja
```

## Execution
Run the test suite:
```
./imiTest
```
Run the benchmark:
```
./imiBench <key type> <iterations> <data binary file> <equality lookup file> <range lookup file> <result file prefix>
```
Generate unsigned integer datasets:
```
./scripts/generate_uint_data.py <data size> <subset count> <equality lookup count> <range lookup selectivities> <range lookup count>
```

## Benchmark sparse/dense unsigned integer entry keys
**Generate the datasets from the project root directory:**
```
./scripts/generate_uint_data.py 10000000 5 1000000 0.0001 1000000
```
The above command generates five index entry datasets, with the largest having 10 million entries. The resulting datasets have 2, 4, 6, 8, and 10 million entries. Besides, a dataset specifying equality lookup keys is generated. This contains 1 million search keys. Also, a dataset specifying ranges for range lookups is generated. This contains 1 million search key ranges. Each resulting range lookup has a selectivity of 0.0001 (0.01%).

**Run the benchmark script to execute the `imiBench` binary with the various sized datasets:**
```
cd build/
../scripts/bench_sparse_dense.py
```
The benchmark script runs the `imiBench` binary for the datasets that are present in `resources/data`. For example, if only index entries with dense 64-bit unsigned integer keys are to be used, only files with the prefix `dense_uint64` should be present in this directory.

**Visualize the Results:**
If multiple json result files are generated for all four combinations of sparse/dense ascending/shuffled datasets, graphs showing for all datasets and the four various data characteristic combinations can be visualized with the following script. This has to be executed in the directory with the json files (assuming the `build` directory in this example):
```
../scripts/plot_multi_json_results.py subplots
```
