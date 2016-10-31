Triangle Counting
=================

Run ```make``` to generate the required executables, then use the syntax below to run the benchmark

The first argument to the executable specifies whether you want to read the graph from a file (1), or generate a synthetic one internally (0).

**Input Graph from File**

To run with P number of threads, and an .gr type (vertex edge) input file,
    ```./triangle_counting_lock 1 P <input_file>```

To run a matrix format file (.mtx)
    ```./triangle_counting_lock 2 P <input_file>```
  
  The input file can be used as:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

**Generate and Input using the Synthetic Graph Generator**

To run with P number of threads, N vertices, and DEG edges per vertex
   ```./triangle_counting_lock 0 P N DEG```

**Notes**

The executable then outputs the time in seconds that the program took to run, as well as the number of triangles counted.

The file utilizing atomic instructions has the same arguments as specified above.

The file sorted_neighbors_tri_lock.cc contains a variation of the algorithm that required sorted neighbors. This is run the same way as above, although lines 96 and 97 may need to be commented out to allow graphs having forward progressing triangle connections.
