# Setup
The index benchmark cases were executed on a machine with four Intel(R) Xeon(R) CPU E7-4880 v2 CPUs on which each socket serves as a NUMA node.
Each NUMA node has 512 GB of DDR3-1600 memory.
A single CPU is equipped with 15 cores, resulting in 30 hardware threads.
Benchmarks were bound to a single \ac{numa} node using `numactl -N 2 -m 2`.
The index benchmark was compiled with `clang 9.0.1-12` and the options `-O3` `-march=native`.
We used the C++ standard library version `libstdc++.so.6.0.28`.

# Evaluation Results
The content of this directory is complementary to our submission of the GvDB21 workshops.
We provide the descriptions of all benchmark cases and the evaluation graphs as PDF files in this directory.

In terms of lookup duration evaluations, we used one million lookup operations in the respective benchmark cases.
For the range lookups, we used a selectivity of 0.01%.

# Benchmark Cases
The developed benchmark framework contains several benchmark cases which were used for our evaluation of the index implementations.
The benchmark cases are explicitly designed to evaluate the index operations *Insert*, *Delete*, *Equality Lookup*, *Range Lookup*, *Bulk Insert*, and *Bulk Load*.

### Insert
In this benchmark case, the execution time required to insert a given sequence of index entries using the insert operation is measured.
Before the insert operations are executed and measured, the data structure is initialized using the constructor.
In addition to the execution time aspect, the memory consumption required by the data structure to store the index entries is measured.
For this, the allocated memory is measured before the initialization of the data structure and after inserting the index entries.
The difference of the allocated memory is considered as memory consumption of the data structure.

### Delete
In this benchmark case, the execution time required to delete a given sequence of index entries using the delete operation is measured.
Before the delete operations are executed and measured, the data structure is initialized using the constructor and the entire sequence of index entries to be deleted is inserted using the insert operation.
These preparations are not included in the time measurement.
The order in which the entries are deleted corresponds to the insertion order.

### Equality Lookup
In this benchmark case, the time required to execute a given sequence of equality lookups is measured.
Before executing and measuring the equality lookups, the data structure is initialized using the constructor and a given sequence of index entries is inserted using the insert operation.
These preparations are not included in the time measurement.

### Range Lookup
In this benchmark case, the time required to execute a given sequence of range lookups is measured.
Before executing and measuring the range lookups, the data structure is initialized using the constructor and a given sequence of index entries is inserted using the insert operation.
These preparations are not included in the time measurement.

### Bulk Load
In this benchmark case, the execution time required to insert a given sequence of index entries using the bulk load operation is measured.
Before the execution and measurement, the data structure is initialized using the constructor.
In addition to the execution time consideration, the memory consumption required by the data structure to store the index entries is measured.
Therefore, similar to the \emph{Insert} benchmark case, the allocated memory is measured before the initialization of the data structure and after inserting the index entries.
The difference of the allocated memory values is considered as memory consumption of the data structure. 

### Bulk Insert
This case is similar to case \emph{Bulk Load}, with the difference that in this case, the bulk insert operation is used instead of the bulk load operation.