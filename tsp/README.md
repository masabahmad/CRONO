Travelling Salesman Problem
===========================

Run ```make``` to generate the required executable, then run using the syntax below

Synthetic Graphs : tsp.cc

To compile tsp.cc
    ```g++ tsp.cc -lm -lrt -lpthread -O2```
  
To run with P number of threads, C cities
    ```./a.out P C```

e.g.
    ```./a.out 2 16```

The executable then outputs the time in seconds that the program took to run.
