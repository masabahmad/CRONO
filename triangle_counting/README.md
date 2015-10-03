Triangle Counting
=================

Run ```make``` to generate the required executables, then use the syntax below to run the benchmark

To run with P number of threads
    ```./triangle_counting P <input_file>```
  
  The input file can be used as:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

**Notes**

Input threads must be in powers of 2.

The executable then outputs the time in seconds that the program took to run, as well as the number of triangles counted.
