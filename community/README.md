Community Detection
=======================

Run ```make``` to generate the required executables in each subdirectory, then run using the syntax explained in each markdown file.

To run with P number of threads, I iterations, and an input file
   ```./community P I <input_file>```

  For the input file, use sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. roadNet-CA)
  https://snap.stanford.edu/data/#road

**Notes**

Input threads must be in powers of 2.

Number of iterations required comes from the approximate louvian algorithm, more iterations means higher accuracy of detection.

The executables also output the time in seconds that the program took to run.
