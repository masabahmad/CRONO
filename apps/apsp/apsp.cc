/*
  Program adopted from Parallel MiBench
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
//#include "carbon_user.h"  /*For the Graphite Simulator*/
#include <time.h>
#include <sys/timeb.h>

#define MAX            100000000
#define INT_MAX        100000000
#define BILLION 1E9

//Thread Argument Structure
typedef struct
{
   int*      local_min;
   int*      global_min;
   int*      Q;
   int*      D;
   int**     W;
   int**     W_index;
   int*      d_count;
   int       tid;
   int       P;
   int       N;
   int       DEG;
   pthread_barrier_t* barrier_total;
   pthread_barrier_t* barrier;
} thread_arg_t;

//Function Declarations
int initialize_single_source(int* D, int* Q, int source, int N);
void relax(int u, int i, volatile int* D, int** W, int** W_index, int N);
int get_local_min(volatile int* Q, volatile int* D, int start, int stop, int N, int** W_index, int** W, int u);
void init_weights(int N, int DEG, int** W, int** W_index);

//Global Variables
int min = INT_MAX;    //For local mins
int min_index = 0;
pthread_mutex_t lock;
pthread_mutex_t locks[4194304];  //unused
int u = 0;                       //next best vertex
int local_min_buffer[1024];
int global_min_buffer;
int start = 64;
int P_global = 256;
thread_arg_t thread_arg[1024];
pthread_t   thread_handle[1024];  //MAX threads and pthread handlers

//Primary Parallel Function
void* do_work(void* args)
{
   volatile thread_arg_t* arg = (thread_arg_t*) args;

   //volatile int* Q          = arg->Q;
   //int* D                   = arg->D;
   int** W                  = arg->W;       //Graph weights
   int** W_index            = arg->W_index; //Graph connections
   const int N              = arg->N;       //Total Vertices
   const int DEG            = arg->DEG;     //Edges per Vertex
   int v                    = 0;            //current vertex
   P_global                 = start;

   int node = 0;

   pthread_barrier_wait(arg->barrier_total);

   while(node<N)
   {
      //Memory allocations
      int *D;
      int *Q;
      if (posix_memalign((void**) &D, 64, N * sizeof(int))) 
      {
         fprintf(stderr, "Allocation of memory failed\n");
         exit(EXIT_FAILURE);
      }
      if ( posix_memalign((void**) &Q, 64, N * sizeof(int)))
      {
         fprintf(stderr, "Allocation of memory failed\n");
         exit(EXIT_FAILURE);
      }
      //Initialize distance arrays
      initialize_single_source(D, Q, node, N);

      //Relax all edges, Bellman-Ford type
      for(v=0;v<N;v++)
      {
         for(int i = 0; i < DEG; i++)
         {
            if((D[W_index[v][i]] > (D[v] + W[v][i])))
               D[W_index[v][i]] = D[v] + W[v][i];

            Q[v]=0; //Current vertex checked
         }
      }

       pthread_mutex_lock(&lock);   //Vertex Capture
       node++;
       pthread_mutex_unlock(&lock);
   }

   pthread_barrier_wait(arg->barrier_total);
   return NULL;
}

int main(int argc, char** argv)
{
   //Input arguments
   const int P1 = atoi(argv[1]);
   const int N = atoi(argv[2]);
   const int DEG = atoi(argv[3]);

   int P = 256;
   start = P1;
   P = P1;


   if (DEG > N)
   {
      fprintf(stderr, "Degree of graph cannot be grater than number of Vertices\n");
      exit(EXIT_FAILURE);
   }

   //int* D;
   //int* Q;
   //posix_memalign((void**) &D, 64, N * sizeof(int));
   //posix_memalign((void**) &Q, 64, N * sizeof(int));
   int d_count = N;
   pthread_barrier_t barrier_total;
   pthread_barrier_t barrier;

   //Memory allocations for the input graph
   int** W = (int**) malloc(N*sizeof(int*));
   int** W_index = (int**) malloc(N*sizeof(int*));
   for(int i = 0; i < N; i++)
   {
      //W[i] = (int *)malloc(sizeof(int)*N);
      int ret = posix_memalign((void**) &W[i], 64, DEG*sizeof(int));
      int re1 = posix_memalign((void**) &W_index[i], 64, DEG*sizeof(int));
      if (ret != 0 || re1!=0)
      {
         fprintf(stderr, "Could not allocate memory\n");
         exit(EXIT_FAILURE);
      }
   }
   
   //Initialize random graph
   init_weights(N, DEG, W, W_index);

   //Synchronization Variables' Initialization
   pthread_barrier_init(&barrier_total, NULL, P1);
   pthread_barrier_init(&barrier, NULL, P1);
   pthread_mutex_init(&lock, NULL);
   for(int i=0; i<2097152; i++)
      pthread_mutex_init(&locks[i], NULL);

   //Thread Arguments
   for(int j = 0; j < P1; j++) {
      thread_arg[j].local_min  = local_min_buffer;
      thread_arg[j].global_min = &global_min_buffer;
      //thread_arg[j].Q          = Q;
      //thread_arg[j].D          = D;
      thread_arg[j].W          = W;
      thread_arg[j].W_index    = W_index;
      thread_arg[j].d_count    = &d_count;
      thread_arg[j].tid        = j;
      thread_arg[j].P          = P;
      thread_arg[j].N          = N;
      thread_arg[j].DEG        = DEG;
      thread_arg[j].barrier_total = &barrier_total;
      thread_arg[j].barrier    = &barrier;
   }

   //Measure CPU time
   struct timespec requestStart, requestEnd;
   clock_gettime(CLOCK_REALTIME, &requestStart);

   // Enable Graphite performance and energy models
   //CarbonEnableModels();

   //Spawn Threads
   for(int j = 1; j < P1; j++) {
      pthread_create(thread_handle+j,
            NULL,
            do_work,
            (void*)&thread_arg[j]);
   }
   do_work((void*) &thread_arg[0]);

   printf("\nThreads Returned!");

   //Join Threads
   for(int j = 1; j < P1; j++) { //mul = mul*2;
      pthread_join(thread_handle[j],NULL);
   }

   // Disable Graphite performance and energy models
   //CarbonDisableModels();

   printf("\nThreads Joined!");

   clock_gettime(CLOCK_REALTIME, &requestEnd);
   double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
   printf( "\nTime: %lf seconds\n", accum );

   /*for(int i = 0; i < N; i++) {
     printf(" %d ", D[i]);
     }
     printf("\n");
     */
   return 0;
}

//Distance initializations
int initialize_single_source(int*  D,
      int*  Q,
      int   source,
      int   N)
{
   for(int i = 0; i < N; i++)
   {
      D[i] = INT_MAX;  //all distances infinite
      Q[i] = 1;
   }

   D[source] = 0;      //source distance 0
   return 0;
}

//Get local min vertex to jump to in the next iteration
int get_local_min(volatile int* Q, volatile int* D, int start, int stop, int N, int** W_index, int** W, int u)
{
   int min = INT_MAX;
   int min_index = N;

   for(int i = start; i < stop; i++) 
   {
      if(D[i] < min && Q[i])  //if current edge has the smallest distance
      {
         min = D[i];
         min_index = W_index[u][i];
      }
   }
   return min_index;          //return smallest edge
}

//Relax : updates distance based on the current vertex
void relax(int u, int i, volatile int* D, int** W, int** W_index, int N)
{
   if((D[W_index[u][i]] > (D[u] + W[u][i]) && (W_index[u][i]!=-1 && W_index[u][i]<N && W[u][i]!=INT_MAX)))
      D[W_index[u][i]] = D[u] + W[u][i];
}

//Graph initializer
void init_weights(int N, int DEG, int** W, int** W_index)
{
   // Initialize to -1
   for(int i = 0; i < N; i++)
      for(int j = 0; j < DEG; j++)
         W_index[i][j]= -1;

   // Populate Index Array
   for(int i = 0; i < N; i++)
   {
      int last = 0;
      for(int j = 0; j < DEG; j++)
      {
         if(W_index[i][j] == -1)
         {
            int neighbor = i + j;//rand()%(max);
            if(neighbor > last)
            {
               W_index[i][j] = neighbor;
               last = W_index[i][j];
            }
            else
            {
               if(last < (N-1))
               {
                  W_index[i][j] = (last + 1);
                  last = W_index[i][j];
               }
            }
         }
         else
         {
            last = W_index[i][j];
         }
         if(W_index[i][j]>=N)
         {
            W_index[i][j] = N-1;
         }
      }
   }

   // Populate Cost Array
   for(int i = 0; i < N; i++)
   {
      for(int j = 0; j < DEG; j++)
      {
         double v = drand48();
         /*if(v > 0.8 || W_index[i][j] == -1)
           {       W[i][j] = MAX;
           W_index[i][j] = -1;
           }

           else*/ if(W_index[i][j] == i)
         W[i][j] = 0;

         else
            W[i][j] = (int) (v*100) + 1;
         //printf("   %d  ",W_index[i][j]);
      }
      //printf("\n");
   }
}