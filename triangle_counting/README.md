Triangle Counting
=================

Run ```make``` to generate the required executables, then use the syntax below to run the benchmark

**Input Graph from File**

To run with P number of threads, and an input file,
    ```./triangle_counting 1 P <input_file>```
  
  The input file can be used as:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

**Generate and Input using the Synthetic Graph Generator**

To run with P number of threads, N vertices, and DEG edges per vertex
   ```./triangle_counting 0 P N DEG```

**Notes**

Input threads must be in powers of 2.

The executable then outputs the time in seconds that the program took to run, as well as the number of triangles counted.
