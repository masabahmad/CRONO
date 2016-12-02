PageRank
========

Run ```make``` to generate executables, then use the syntax explained below

The first argument to the executable specifies whether you want to read the graph from a file (1), or generate a synthetic one internally (0).

**Input Graph from File**

To run with P number of threads, and an input file,
    ```./pagerank 1 P <input_file>```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

**Generate and Input using the Synthetic Graph Generator**

To run with P number of threads, N vertices, and DEG edges per vertex
   ```./pagerank 0 P N DEG```

**Notes**

The executable then outputs the time in seconds that the program took to run.
It also outputs a file that contains the pageranks (normalized to 1).

```pagerank_lock.cc``` updates dangling page values via locks instead of using a distributed data structure. This is meant for architectural simulation works trying to improve lock accesses.

Some very small differences in pageranks might occur due to floating point round offs within the program.
