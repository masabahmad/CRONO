Community Detection
=======================

Run ```make``` to generate the required executables in each subdirectory, then run using the syntax explained in each markdown file.

To compile community.cc
   ```g++ community.cc -o community -lm -lrt -lpthread -O2```

To run with P number of threads, I iterations, and an input file
   ```./community P I <input_file>```

  For the input file, use sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. roadNet-CA)
  https://snap.stanford.edu/data/#road

This is an approximate version of community detection as discussion in our CRONO paper.

**Notes**

Input threads must be in powers of 2.
The executables also output the time in seconds that the program took to run.
