Breadth First Search
==================

Run ```make``` to generate the required executables, the run using the syntax explained below.

The first argument to the executable specifies whether you want to read the graph from a file (1), or generate a synthetic one internally (0).

**Input Graph from File**

To run with P number of threads, and an input file 
   ```./bfs 1 P <input_file>```

The input file can be used as: sample.txt OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph) https://snap.stanford.edu/data/

**Generate and Input using the Synthetic Graph Generator**

To run with P number of threads, N vertices, and DEG edges per vertex
   ```./bfs 0 P N DEG```

**Notes**

The executable outputs the time in seconds that the program took to run.
