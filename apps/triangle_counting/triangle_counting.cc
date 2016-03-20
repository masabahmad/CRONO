#include <cstdio>
#include <cstdlib>
#include <pthread.h>
//#include "carbon_user.h"     /* For the Graphite Simulator*/
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
   //int**     W;
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
void init_weights(int N, int DEG, int** W, int** W_index);

//Global Variables
pthread_mutex_t lock;
pthread_mutex_t locks[4194304];      //Lock for each vertex
int local_min_buffer[1024];
int Total_tid[1024] = {0};           //To store triangles per thread
int global_min_buffer;
int P_global = 256;
int *edges;
int *exist;
int largest=0;
long long Total_Tri = 0;                 //Stores total triangles
thread_arg_t thread_arg[1024];       //Max threads and pthread handlers
pthread_t   thread_handle[1024];

//Primary Parallel Function
void* do_work(void* args)
{
   //Thread Variables
   volatile thread_arg_t* arg = (thread_arg_t*) args;
   int tid                  = arg->tid;
   int P                    = arg->P;
   volatile int* Q          = arg->Q;
   int* D                   = arg->D;         //stores edges
   //int** W                = arg->W;
   int** W_index            = arg->W_index;   //Graph connections
   const int N              = arg->N;
   int v                    = 0;              //for each vertex
   double P_d = P;
   double tid_d = tid;
   double largest_d = largest;

   int start =  0;  //tid    * DEG / (arg->P);
   int stop  = 0;   //(tid+1) * DEG / (arg->P);

   //Chunk work into threads
   double start_d = (tid_d) * (largest_d/P_d);
   double stop_d = (tid_d+1.0) * (largest_d/P_d);
   start = start_d; //tid    *  (largest) / (P);
   stop  = stop_d; //(tid+1) *  (largest) / (P);
   //printf("\n %d %d %d",tid,start,stop);

   pthread_barrier_wait(arg->barrier_total);

   //Look at all edges
   for(v=start;v<stop;v++)
   {
      if(exist[v]==1)
      {
         for(int i = 0; i < edges[v]; i++)
         {
            int neighbor = W_index[v][i];
            if(neighbor>=N)
               continue;

            pthread_mutex_lock(&locks[neighbor]);

            D[W_index[v][i]]++;   //Add edges
            Q[W_index[v][i]] = 0;

            pthread_mutex_unlock(&locks[neighbor]);
         }
      }
   }

   pthread_barrier_wait(arg->barrier_total);

   //Find triangles for each thread
   for(v=start;v<stop;v++)
   {
      if(exist[v]==1)
      {
         unsigned int ret = -1;
         /*while (D[uu] != 0) 
           {
           D[uu] >>= 1;
           ret++;
           }*/
         ret = D[v]/3;
         D[v]=ret;
         if(D[v]>=1)
         {
            //pthread_mutex_lock(&lock);
            Total_tid[tid] = Total_tid[tid]+D[v];
            //pthread_mutex_unlock(&lock);
         }
      }
   }

   pthread_barrier_wait(arg->barrier_total);

   //The master thread sums up all triangles
   if(tid==0)
   {
      for(int i=0;i<P;i++)
      {
         Total_Tri = Total_Tri + Total_tid[i];
      }
   }

   pthread_barrier_wait(arg->barrier_total);

   return NULL;
}


int main(int argc, char** argv)
{
   //char filename[100];
   FILE *file0 = NULL;
   int N = 0;
   int DEG = 0;
   const int select = atoi(argv[1]);

   //For reading graph from file
   if(select==1)
   {
      const char *filename = argv[3];
      //printf("Please Enter The Name Of The File You Would Like To Fetch\n");
      //scanf("%s", filename);
      file0 = fopen(filename,"r");
   }

   int lines_to_check=0;
   char c;
   int number0;
   int number1;
   int inter = -1; 

   if(select==1)
   {
      N = 2097152; //can be read from file if needed, this is a default upper limit
      DEG = 12;     //also can be reda from file if needed, upper limit here again
   }

   const int P1 = atoi(argv[2]);

   int P = P1;
   P_global = P1;

   if(select==0)
   {
      N = atoi(argv[3]);
      DEG = atoi(argv[4]);
      printf("\nGraph with Parameters: N:%d DEG:%d\n",N,DEG);
   }
   if (DEG > N)
   {
      fprintf(stderr, "Degree of graph cannot be grater than number of Vertices\n");
      exit(EXIT_FAILURE);
   }

   //Memory Allocations
   int* D;
   int* Q;
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
   if(posix_memalign((void**) &edges, 64, N * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &exist, 64, N * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   int d_count = N;
   pthread_barrier_t barrier_total;
   pthread_barrier_t barrier;

   int** W = (int**) malloc(N*sizeof(int*));
   int** W_index = (int**) malloc(N*sizeof(int*));
   for(int i = 0; i < N; i++)
   {
      //W[i] = (int *)malloc(sizeof(int)*N);
      if(posix_memalign((void**) &W[i], 64, DEG*sizeof(int)))
      {
         fprintf(stderr, "Allocation of memory failed\n");
         exit(EXIT_FAILURE);
      }
      if(posix_memalign((void**) &W_index[i], 64, DEG*sizeof(int)))
      {
         fprintf(stderr, "Allocation of memory failed\n");
         exit(EXIT_FAILURE);
      }
   }

   for(int i=0;i<N;i++)
   {
      for(int j=0;j<DEG;j++)
      {
         //W[i][j] = 1000000000;
         W_index[i][j] = INT_MAX;
      }
      edges[i]=0;
      exist[i]=0;
   }

   //For real graphs
   if(select==1)
   {
      for(c=getc(file0); c!=EOF; c=getc(file0))
      {
         if(c=='\n')
            lines_to_check++;

         if(lines_to_check>3)
         {
            int f0 = fscanf(file0, "%d %d", &number0,&number1);
            if(f0 != 2 && f0 != EOF)
            {
               printf ("Error: Read %d values, expected 2. Parsing failed.\n",f0);
               exit (EXIT_FAILURE);
            }
            //printf("\n%d %d",number0,number1);
            if(number0>largest)
               largest=number0;
            if(number1>largest)
               largest=number1;
            inter = edges[number0];

            //W[number0][inter] = drand48();
            W_index[number0][inter] = number1;
            //previous_node = number0;
            edges[number0]++;
            exist[number0]=1; exist[number1]=1;
         }
      }
      printf("\nFile Read, Largest Vertex:%d",largest);
   }

   //For synthetic graphs
   if(select==0)
   {
      init_weights(N, DEG, W, W_index);
      largest = N;
   }

   //Initialize Synchronization Variables
   pthread_barrier_init(&barrier_total, NULL, P);
   pthread_barrier_init(&barrier, NULL, P);
   pthread_mutex_init(&lock, NULL);

   for(int i=0; i<largest+1; i++)
   {
      if(select==0)
      {
         exist[i]=1;
         edges[i]=DEG;
      }
      if(exist[i]==1)
         pthread_mutex_init(&locks[i], NULL);
   }

   //Initialize 1-d arrays
   initialize_single_source(D, Q, 0, N);

   //Thread Arguments
   for(int j = 0; j < P; j++) {
      thread_arg[j].local_min  = local_min_buffer;
      thread_arg[j].global_min = &global_min_buffer;
      thread_arg[j].Q          = Q;
      thread_arg[j].D          = D;
      //thread_arg[j].W          = W;
      thread_arg[j].W_index    = W_index;
      thread_arg[j].d_count    = &d_count;
      thread_arg[j].tid        = j;
      thread_arg[j].P          = P;
      thread_arg[j].N          = N;
      thread_arg[j].DEG        = DEG;
      thread_arg[j].barrier_total = &barrier_total;
      thread_arg[j].barrier    = &barrier;
   }

   struct timespec requestStart, requestEnd;
   clock_gettime(CLOCK_REALTIME, &requestStart);

   // Enable Graphite performance and energy models
   //CarbonEnableModels();

   //Start Threads
   for(int j = 1; j < P; j++) {
      pthread_create(thread_handle+j,
            NULL,
            do_work,
            (void*)&thread_arg[j]);
   }
   do_work((void*) &thread_arg[0]);

   //Join Threads
   for(int j = 1; j < P; j++) { //mul = mul*2;
      pthread_join(thread_handle[j],NULL);
   }

   // Disable Graphite performance and energy models
   //CarbonDisableModels();
   
   printf("\nThreads Joined!");

   //Print Time Taken
   clock_gettime(CLOCK_REALTIME, &requestEnd);
   double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
   printf( "\nTime Taken:\n%lf seconds", accum );

   printf("\nTriangles=%lld \n",Total_Tri);

   return 0;
}

int initialize_single_source(int*  D,
      int*  Q,
      int   source,
      int   N)
{
   for(int i = 0; i < N+1; i++)
   {
      D[i] = 0;
      Q[i] = 1;
   }

   //D[source] = 0;
   return 0;
}

//Graph Generator
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
            int neighbor = i+j;
            //W_index[i][j] = i+j;//rand()%(DEG);

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
