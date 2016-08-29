Community Detection
=======================

Run ```make``` to generate the required executable, then run using the syntax below.

The first argument to the executable specifies whether you want to read the graph from a file (1), or generate a synthetic one internally (0).

**Input Graph from File**

To run with P number of threads, I iterations, and an input file
   ```./community_lock 1 P I <input_file>```

To run a matrix format file (.mtx)
    ```./community_lock 2 P I <input_file>```

  For the input file, use sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. roadNet-CA)
  https://snap.stanford.edu/data/#road

**Generate and Input using the Synthetic Graph Generator**

To run with P number of threads, I iterations, N vertices, and DEG edges per vertex
   ```./community_lock 0 P I N DEG```

**Notes**

Number of iterations required comes from the approximate louvian algorithm, more iterations means higher accuracy of detection.

The executable outputs the time in seconds that the program took to run.

The file utilizing atomic instructions has the same arguments as specified above. Although the weights type is converted from float to int.
