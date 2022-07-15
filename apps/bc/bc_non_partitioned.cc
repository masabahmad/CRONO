/*
  Program adopted from Parallel MiBench All Pairs Shortest Path
  Assumption : An entity (UAV) using this program practically needs shortest paths before making a decision.
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
   int*      sigma;
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
int min = INT_MAX;
int min_index = 0;         //For local mins
pthread_mutex_t lock;
pthread_mutex_t *locks;
int u = 0;                  //next best vertex
int local_min_buffer[1024];
int global_min_buffer;
int start = 64;
int P_global = 256;  

int* avg;          //avg shortest paths passing
double* delta;     //centralities

thread_arg_t thread_arg[1024];
pthread_t   thread_handle[1024];   //MAX threads and pthread handlers

//Primary Parallel Function
void* do_work(void* args)
{
   volatile thread_arg_t* arg = (thread_arg_t*) args;

   int tid                  = arg->tid;
   int P                    = arg->P;
   //volatile int* Q          = arg->Q;
   //int* D                   = arg->D;
   int** W                  = arg->W;       //Graph weights
   int** W_index            = arg->W_index; //Graph connections
   const int N              = arg->N;       //Total Vertices
   const int DEG            = arg->DEG;     //Edges per Vertex
   int v                    = 0;            //current vertex
   int* sigma               = arg->sigma;   //shortest paths passing
   P_global = start;
   double tid_d = tid;
   double P_d = P;
   double N_d = N;

   /*int* D;
     int* Q;

     int p0 = posix_memalign((void**) &D, 64, N * sizeof(int));
     int p1 = posix_memalign((void**) &Q, 64, N * sizeof(int));

     for(int i=0;i<N;i++)
     {
     D[i] = INT_MAX;
     Q[i] = 1;
     }
     D[0]=0;*/

   //divide work among threads via double precision
   double start_d = tid_d * (N_d/P_d);
   double stop_d  = (tid_d+1.0) * (N_d/P_d);
   int i_start =  start_d;// tid    * N / (arg->P);
   int i_stop  =  stop_d;//(tid+1) * N / (arg->P);
   int node = 0;

   pthread_barrier_wait(arg->barrier_total);

   //all_pairs shortest path first here
   while(node<N)
   {
      int *D;
      int *Q;

      if(posix_memalign((void**) &D, 64, N * sizeof(int)))
      {
         fprintf(stderr, "Allocation of memory failed\n");
         exit(EXIT_FAILURE);
      }
      if(posix_memalign((void**) &Q, 64, N * sizeof(int)))
      {
         fprintf(stderr, "Allocation of memory failed\n");
         exit(EXIT_FAILURE);
      }

      initialize_single_source(D, Q, node, N);

      for(v=0;v<N;v++)
      {   //Bellman Ford type 
         for(int i = 0; i < DEG; i++)
         {
            if((D[W_index[v][i]] > (D[v] + W[v][i])))
            {
               int neighbor = W_index[v][i];
               D[neighbor] = D[v] + W[v][i];
               pthread_mutex_lock(&locks[neighbor]); 
               sigma[neighbor]++;
               pthread_mutex_unlock(&locks[neighbor]);
               //printf(" %d ",sigma[tid][neighbor]);
            }  
            Q[v]=0;
         }
      }

       //vertex capture
       pthread_mutex_lock(&lock);
       node++;
       //printf("\n %d",next_source);
       pthread_mutex_unlock(&lock);
   }

   pthread_barrier_wait(arg->barrier_total);
   //Betweenness centrality stuff here
   //Sum dependencies and then avg for approx

   for(int j=i_start;j<i_stop;j++)
   {
      //for(int k=0;k<P;k++)
      //{
         avg[j] = avg[j] + sigma[j]; //sigma is not read only after the barrier and hence no locks required
      //}
      //avg[j] = avg[j]/P;
      if(avg[j]==0 || avg[j]>N)
         avg[j]=1;
      //printf(" %d ",avg[j]);
   }
   pthread_barrier_wait(arg->barrier_total);

   //Master thread reduces centralities
   if(tid==0)
   {
      for(int j=0;j<N;j++)
      {
         float div0 = avg[j];
         float div1 = avg[N-j-1];
         float div = div0/div1;
         delta[j] = delta[j] + (div*(delta[N-j-1]+1));
      }
      //printf("\n %f",delta[1023]);
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

   int* sigma;
   //int* D;
   //int* Q;
   //posix_memalign((void**) &D, 64, N * sizeof(int));
   //posix_memalign((void**) &Q, 64, N * sizeof(int));

   //Memory allocations for the input graph
   if(posix_memalign((void**) &avg, 64, N * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &delta, 64, N * sizeof(double)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &sigma, 64, N * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }

   int d_count = N;
   pthread_barrier_t barrier_total;
   pthread_barrier_t barrier;

   //Memory allocations for the input graph
   int** W = (int**) malloc(N*sizeof(int*));
   int** W_index = (int**) malloc(N*sizeof(int*));
   //int** sigma = (int**) malloc(P*sizeof(int*));
   for(int i = 0; i < N; i++)
   {
      //W[i] = (int *)malloc(sizeof(int)*N);
      delta[i]=0;
      avg[i]=0;
      int ret = posix_memalign((void**) &W[i], 64, DEG*sizeof(int));
      int re1 = posix_memalign((void**) &W_index[i], 64, DEG*sizeof(int));
      //int re2 = posix_memalign((void**) &sigma[i], 64, N*sizeof(int));
      if (ret != 0 || re1!=0)
      {
         fprintf(stderr, "Could not allocate memory\n");
         exit(EXIT_FAILURE);
      }
   }
   
   //Initialize random graph
   init_weights(N, DEG, W, W_index);
   for(int i = 0;i<1;i++)
   {
      for(int j = 0;j<N;j++)
      {
         sigma[j]=0;
      }
      //printf("\n");
   }

   //Synchronization Variables' Initialization
   pthread_barrier_init(&barrier_total, NULL, P1);
   pthread_barrier_init(&barrier, NULL, P1);
   locks = (pthread_mutex_t*) malloc((N+16) * sizeof(pthread_mutex_t));
   pthread_mutex_init(&lock, NULL);

   for(int i=0; i<N; i++)
      pthread_mutex_init(&locks[i], NULL);

   //initialize_single_source(D, Q, 0, N);

   //Thread Arguments
   for(int j = 0; j < P1; j++) {
      thread_arg[j].local_min  = local_min_buffer;
      thread_arg[j].global_min = &global_min_buffer;
      //thread_arg[j].Q          = Q;
      //thread_arg[j].D          = D;
      thread_arg[j].W          = W;
      thread_arg[j].W_index    = W_index;
      thread_arg[j].sigma      = sigma;
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
   do_work((void*) &thread_arg[0]);  //Main thread spawns itself

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

   //printf("\ndistance:%d \n",D[N-1]);

   /*for(int i = 0; i < N; i++) {
     printf("\n %f ", delta[i]);
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
      D[i] = INT_MAX;   //all distances infinite
      Q[i] = 1;
   }

   D[source] = 0;       //source distance 0
   return 0; 
}

//Get local min vertex to jump to in the next iteration
int get_local_min(volatile int* Q, volatile int* D, int start, int stop, int N, int** W_index, int** W, int u)
{
   int min = INT_MAX;
   int min_index = N;

   for(int i = start; i < stop; i++) 
   {
      if(D[i] < min && Q[i])         //if current edge has the smallest distance
      {
         min = D[i];
         min_index = W_index[u][i];
      }
   }
   return min_index;                 //return smallest edge
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
            //W_index[i][j] = rand()%DEG;
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
            W[i][j] = (int) (v*10) + 1;
         //printf("   %d  ",W[i][j]);
      }
      //printf("\n");
   }
}
