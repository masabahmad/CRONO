#include <cstdio>
#include <cstdlib>

volatile int passed = 0; // Number of barriers, passed by all threads.
int bar=0;
int PMAX=0;

void barrier_wait()
{
    int passed_old = passed; // Should be evaluated before incrementing *bar*!

    if(__sync_fetch_and_add(&bar,1) == (PMAX - 1)) 
    {   
        // The last thread, faced barrier.
        bar = 0;
        // *bar* should be reseted strictly before updating of barriers counter.
        __sync_synchronize(); 
        passed++; // Mark barrier as passed.
    }   
    else
    {   
        // Not the last thread. Wait others.
        while(passed == passed_old) {}; 
        // Need to synchronize cache with other threads, passed barrier.
        __sync_synchronize();
    }   
}
