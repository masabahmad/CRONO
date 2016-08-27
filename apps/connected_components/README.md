Connected Components
=======================

Run ```make``` to generate the required executable, then run using the syntax explained below

The first argument to the executable specifies whether you want to read the graph from a file (1), or generate a synthetic one internally (0).

**Input Graph from File**

To run with P number of threads, and an input file of .gr format,
   ```./connected_components_lock 1 P <input_file>```

To run a matrix format file (.mtx)
    ```./connected_components_lock 2 P <input_file>```

  The input file can be used as:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)  https://snap.stanford.edu/data/

**Generate and Input using the Synthetic Graph Generator**

To run with P number of threads, N vertices, and DEG edges per vertex
   ```./connected_components_lock 0 P N DEG```

**Notes**

The executable outputs the time in seconds that the program took to run.

The file utilizing atomic instructions has the same arguments as specified above.

A small code snippet getting the total number of unique components is also included.
