/*
    Travelling Salesman Problem (TSP), Uses Parallel Branch and Bound to find paths with least distances
    This program was originally written/modified by george kurian (MIT, Google), Hank Hoffmann (MIT)
	Modified later by Masab Ahmad (UConn)
*/

#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <list>
using std::vector;
using std::list;
//#include "carbon_user.h"        /*For the Graphite Simulator*/

// #define DEBUG           1
#define INF             100000000

//Default graph edge weights
int _WEIGHTS[5][5] = {
   { INF, 2,   3,   1,   INF },
   { INF, INF, 5,   7,   4   },
   { 3,   1,   INF, 6,   1   },
   { 8,   2,   4,   INF, 3   },
   { 7,   INF, 6,   1,   INF }
};

//Graph Structure
int** _weights;

// Globals
int _best_tour_cost ;
int _best_tid;
pthread_mutex_t _lock;

//Graph Creation
void initCities(int NUM_CITIES)
{
   _weights = new int*[NUM_CITIES];
   for (int i = 0; i < NUM_CITIES; i++)
      _weights[i] = new int[NUM_CITIES];

   if(NUM_CITIES == 5) {
      for(int i = 0; i < NUM_CITIES; i++) {
         for(int j = 0; j < NUM_CITIES; j++) {
            _weights[i][j] = _WEIGHTS[i][j];
         }
      }
   }
   else {
      for(int i = 0; i < NUM_CITIES; i++) {
         _weights[i][i] = INF;
         for(int j = i+1; j < NUM_CITIES; j++) {
            double v = drand48();                   //Uniform Random edge weights

            _weights[i][j] = INF;
            if(v < 0.8) {
               int length = (int) (v * 100) + 1;
               _weights[i][j] = length;
               _weights[j][i] = length;
            }
         }
      }
   }
}

class PartialTour
{
   public:
      PartialTour(int NUM_CITIES, int start_city);
      PartialTour(const PartialTour& tour);
      ~PartialTour();

      vector<int>& getPath()                    { return _path;       }
      vector<int>& getUnvisited()               { return _unvisited;  }
      const vector<int>& getPath() const        { return _path;       }
      const vector<int>& getUnvisited() const   { return _unvisited;  }
      int getCost() const                       { return _cost;       }
      int getLength() const                     { return _length;     }

      void setCost(int cost)                    { _cost = cost;       }

      void addCity(int unvisited_idx);
      void print() const;

   private:
      vector<int> _path;
      vector<int> _unvisited;
      int _cost;
      int _length;
};

   PartialTour::PartialTour(int NUM_CITIES, int start_city)
   : _cost(0)
     , _length(1)
{
   _path.push_back(start_city);
   for (int i = 0; i < NUM_CITIES; i++)
   {
      if (i != start_city)
         _unvisited.push_back(i);
   }
}

   PartialTour::PartialTour(const PartialTour& tour)
   : _path(tour.getPath())
   , _unvisited(tour.getUnvisited())
   , _cost(tour.getCost())
     , _length(tour.getLength())
{}

PartialTour::~PartialTour()
{}

   void
PartialTour::addCity(int idx)
{  
   assert (_length >= 1);

   int city = _path.back();
   int new_city = _unvisited[idx];

   _cost += _weights [city] [new_city];
   _length ++;
   _path.push_back(new_city);
   _unvisited[idx] = _unvisited.back();
   _unvisited.pop_back();
}

void
PartialTour::print() const
{
   printf("Cost: %d, Length: %d, Path: [", _cost, _length);
   for (vector<int>::const_iterator it = _path.begin(); it != _path.end(); it++)
      printf("%d, ", *it);
   printf("], Unvisited: [");
   for (vector<int>::const_iterator it = _unvisited.begin(); it != _unvisited.end(); it++)
      printf("%d, ", *it);
   printf("]\n");
}

typedef list<PartialTour*> WorkQueue;

//Thread Argument Structure
class ThreadData
{
   public:
      ThreadData(int tid, WorkQueue& work_queue, int NUM_CITIES);
      ~ThreadData();

      int getTid() const          { return _tid; }
      WorkQueue& getWorkQueue()   { return _work_queue; }
      PartialTour* getBestTour()  { return _best_tour;  }
      int getNumCities() const    { return _NUM_CITIES; }

   private:
      int _tid;
      WorkQueue& _work_queue;
      PartialTour* _best_tour;
      int _NUM_CITIES;
};

   ThreadData::ThreadData(int tid, WorkQueue& work_queue, int NUM_CITIES)
   : _tid(tid)
   , _work_queue(work_queue)
     , _NUM_CITIES(NUM_CITIES)
{
   _best_tour = new PartialTour(NUM_CITIES, 0);
   _best_tour->setCost(INF);
}

ThreadData::~ThreadData()
{}

//Primary Parallel Function
   void
doWork(int tid, WorkQueue& work_queue, PartialTour* tour, PartialTour* best_tour, int NUM_CITIES)
{
   int cost      = tour->getCost();
   int length    = tour->getLength();
   int city      = tour->getPath().back();

#if DEBUG
   printf("Working - Tour: length = %d, cost = %d\n", length, cost);
#endif

   if (length == NUM_CITIES)
   {
      cost += _weights [city] [0];
      tour->setCost(cost);

#if DEBUG
      printf("Updating best cost, was %d, now %d\n", best_tour->getCost(), cost);
      best_tour->print();
#endif

      if (cost < best_tour->getCost())
      {
         *best_tour = *tour;

         pthread_mutex_lock(&_lock);
         if (cost < _best_tour_cost)
         {
            _best_tour_cost = cost;
            _best_tid = tid;
#ifdef DEBUG
            printf("Best Tour: Cost(%i), Tid(%i)\n", _best_tour_cost, _best_tid);
#endif
         }
         pthread_mutex_unlock(&_lock);
      }
   }

   else
   {
      vector<int>& unvisited = tour->getUnvisited();

#if DEBUG
      tour->print();
#endif

      if (cost >= _best_tour_cost) // Pruning
         return;

      for (vector<int>::iterator it = unvisited.begin(); it != unvisited.end(); it ++)
      {
         int new_city = *it;
         int new_cost = cost + _weights [city] [new_city];

         if (new_cost < _best_tour_cost)  // Pruning happens here
         {
            PartialTour* new_tour = new PartialTour(*tour);
#if DEBUG
            printf("   Adding city %d to path\n", new_city);
#endif
            new_tour->addCity(it - unvisited.begin());
            work_queue.push_front(new_tour);
         }
      }
   }
}

//Master Parallel Function
void* threadWork(void* targ)
{
   ThreadData* arg         = (ThreadData*) targ;
   int tid                 = arg->getTid();
   WorkQueue& work_queue   = arg->getWorkQueue();
   PartialTour* best_tour  = arg->getBestTour();
   int NUM_CITIES          = arg->getNumCities();

#ifdef DEBUG
   printf("Starting Thread: %i\n", tid);
#endif

   int iter = 0; 
   while (!work_queue.empty())
   {
#if DEBUG
      printf("Starting iter %d, queue length = %i\n", iter, (int) work_queue.size());
#endif

      PartialTour* tour = work_queue.front();
      work_queue.pop_front();
      doWork(tid, work_queue, tour, best_tour, NUM_CITIES);

      delete tour;

#if DEBUG
      printf("Best Tour:\n");
      best_tour->print();
      printf("\n");
#endif

      iter ++;
   }

#ifdef DEBUG
   printf("Ending Thread: %i\n", tid);
#endif

   return NULL;
}

   int
getNumLevels(int NUM_THREADS, int NUM_CITIES)
{
   int num_permutations = 1;
   for (int i = NUM_CITIES-1; i >= 1; i--)
   {
      num_permutations *= i;
      if (num_permutations >= NUM_THREADS)
         return (NUM_CITIES-i);
   }
   fprintf(stderr, "Too many threads\n");
   return 0;
}

   void
generatePartialTours(PartialTour* tour, int num_levels, vector<PartialTour*>& partial_tour_vec)
{
   int length = tour->getLength();
   if (length <= num_levels)
   {
      vector<int>& unvisited = tour->getUnvisited();
      for (vector<int>::iterator it = unvisited.begin(); it != unvisited.end(); it++)
      {
         PartialTour* new_tour = new PartialTour(*tour);
         new_tour->addCity(it - unvisited.begin());
         generatePartialTours(new_tour, num_levels, partial_tour_vec);
      }
      delete tour;
   }
   else // (length > num_levels)
   {
      partial_tour_vec.push_back(tour);
   }
}

   void
dividePartialTours(const vector<PartialTour*>& partial_tour_vec, vector<WorkQueue>& work_queue_vec)
{
   int queue_id = 0;
   for (vector<PartialTour*>::const_iterator it = partial_tour_vec.begin(); it != partial_tour_vec.end(); it++)
   {
      PartialTour* tour = *it;
      work_queue_vec[queue_id].push_back(tour);
      queue_id = (queue_id + 1) % (work_queue_vec.size());
   }
} 

int main(int argc, char** argv)
{
   //Input Arguments
   int NUM_THREADS = atoi(argv[1]);
   int NUM_CITIES  = atoi(argv[2]);

   printf("Starting TSP [Num Threads: %i, Num Cities: %i]\n", NUM_THREADS, NUM_CITIES);

   // Initialize globals
   _best_tour_cost = INF;
   _best_tid = -1;
   //Lock for upper distance bound
   pthread_mutex_init(&_lock, NULL);

   //Initialize Graph
   initCities(NUM_CITIES);

   vector<PartialTour*> partial_tour_vec;
   vector<WorkQueue> work_queue_vec(NUM_THREADS);

   int num_levels = getNumLevels(NUM_THREADS, NUM_CITIES);

#ifdef DEBUG
   printf("Num Levels: %d\n\n", num_levels);
#endif

   generatePartialTours(new PartialTour(NUM_CITIES, 0), num_levels, partial_tour_vec);

#ifdef DEBUG
   printf("Partial Tours [START]:\n");
   for (vector<PartialTour*>::iterator it = partial_tour_vec.begin(); it != partial_tour_vec.end(); it ++)
      (*it)->print();
   printf("Partial Tours [END]:\n");
#endif

   dividePartialTours(partial_tour_vec, work_queue_vec);

#ifdef DEBUG
   for (int i = 0 ; i < NUM_THREADS; i++)
   {
      printf("\nThread: %i\n", i);
      printf("Partial Tours [START]:\n");
      for (WorkQueue::iterator it = work_queue_vec[i].begin(); it != work_queue_vec[i].end(); it ++)
         (*it)->print();
      printf("Partial Tours [END]:\n\n");
   }
#endif

   ThreadData* thread_data[NUM_THREADS];
   pthread_t   thread_handle[NUM_THREADS];

   for (int i = 0; i < NUM_THREADS; i++)
      thread_data[i] = new ThreadData(i, work_queue_vec[i], NUM_CITIES);

   // Enable Graphite performance and power models
   //CarbonEnableModels();

   //Create threads
   for (int i = 1; i < NUM_THREADS; i++)
   {
      int ret = pthread_create(&thread_handle[i], NULL, threadWork, (void*) thread_data[i]);
      if (ret != 0)
      {
         fprintf(stderr, "Could not spawn thread");
         exit(EXIT_FAILURE);
      }
   }
   threadWork((void*) thread_data[0]);    //master thread spawns itself

   //Join threads
   for (int i = 1; i < NUM_THREADS; i++)
   {
      pthread_join(thread_handle[i], NULL);
   }

   // Disable Graphite performance and power models
   //CarbonDisableModels();

   //Print TSP Data
   for (int i = 0 ; i < NUM_THREADS; i++)
   {
      printf("Thread: %i, Final Best Tour:\n", i);
      thread_data[i]->getBestTour()->print();
      printf("\n");
   }

   printf("Overall Best Tour:\n");
   thread_data[_best_tid]->getBestTour()->print();

   return 0;
}
