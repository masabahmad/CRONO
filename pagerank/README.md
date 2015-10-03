PageRank
========

Run ```make``` to generate executables, then use the syntax explained below

To run with P number of threads
    ```./pagerank P```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

**Notes**

Input number of threads must be in powers of 2.

The executable then outputs the time in seconds that the program took to run.
It also outputs a file that contains the pageranks (normalized to 1).

Some very small differences in pageranks might occur due to floating point round offs within the program.
