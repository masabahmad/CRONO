Travelling Salesman Problem
===========================

Run ```make``` to generate the required executable, then run using the syntax below

Synthetic Graphs : tsp_synthetic.cc

To compile tsp_synthetic.cc
    ```g++ tsp_synthetic.cc -o tsp_synthetic -lm -lrt -lpthread -O2```
  
To run with P number of threads, C cities
    ```./tsp_synthetic P C```

e.g.
    ```./tsp_synthetic 2 16```

The executable then outputs the time in seconds that the program took to run.

The test folder contains a checker tsp program as well.
