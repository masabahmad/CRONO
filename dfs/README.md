Depth First Search
==================

Run ```make``` in each subdirectory to generate the required executables, then run using the syntax explained in each markdown file

To compile dfs.cc
   ```g++ dfs.cc -o dfs -lm -lrt -lpthread -O2```

To run with P number of threads, and an input file
   ```./dfs P <input_file>```

  The input file can be used as:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

**Notes**

Input threads must be in powers of 2.
The executable then outputs the time in seconds that the program took to run.
