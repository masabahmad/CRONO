#include <cstdio>
#include <cstdlib>
#include <pthread.h>
//#include "carbon_user.h"   /*For the Graphite Simulator*/
#include <time.h>
#include <sys/timeb.h>
#include <string.h>

#define MAX            100000000
#define INT_MAX        100000000
#define BILLION 1E9

//Thread Structure
typedef struct
{
   int*      local_min;
   int*      global_min;
   int*      Q;
   double*   PR;
   double**  W;
   int**     W_index;
   int       tid;
   int       P;
   int       N;
   int       DEG;
   pthread_barrier_t* barrier;
} thread_arg_t;

//Global Variables
pthread_mutex_t lock;           //single lock
pthread_mutex_t locks[4194304]; //upper limit for locks, can be increased
int local_min_buffer[1024];
double dp_tid[1024];            //dangling pages for each thread, reduced later by locks
int global_min_buffer;
int terminate = 0;  //terminate variable
int *test;          //test variable arrays for graph storage
int *exist;
int *test2;
int *dangling;
int *outlinks;      //array for outlinks
double dp = 0;      //dangling pointer variable
double *pgtmp;      //temporary pageranks
int nodecount = 0;
thread_arg_t thread_arg[1024];    //MAX threads
pthread_t   thread_handle[1024];  //pthread handlers

//Function declarations
int initialize_single_source(double* PR, int* Q, int source, int N, double initial_rank);
void init_weights(int N, int DEG, double** W, int** W_index);

//Primary Parallel Function
void* do_work(void* args)
{
   volatile thread_arg_t* arg = (thread_arg_t*) args;
   int tid                    = arg->tid;
   double* PR                 = arg->PR;
   int** W_index              = arg->W_index;
   const int N                = arg->N;
   int v                      = 0;      //variable for current vertex
   double r                   = 0.15;   //damping coefficient
   double d                   = 0.85;	  //damping coefficient
   double N_real              = N;
   double tid_d = tid;
   double P_d = arg->P;

   //Allocate work among threads
   double start_d = (tid_d) * (N_real/P_d);
   double stop_d = (tid_d+1.0) * (N_real/P_d);
   int i_start = start_d;// tid     * N / (arg->P);
   int i_stop  = stop_d;// (tid+1) * N / (arg->P);

   //Pagerank iteration count
   int iterations = 1;

   //Barrier before starting work
   pthread_barrier_wait(arg->barrier);

   while(iterations>0)
   {
      if(tid==0)
         dp=0;
      pthread_barrier_wait(arg->barrier);

      //for no outlinks, dangling pages calculation
      for(v=i_start;v<i_stop;v++)
      {
         if(dangling[v]==1)
         {
            dp_tid[tid] = dp_tid[tid] + d*(PR[v]/N_real);
            //printf("\n %f %f %f %f",dp,d,D[uu],N_real);
         }
      }
      pthread_mutex_lock(&lock);
      dp = dp + dp_tid[tid];
      pthread_mutex_unlock(&lock);
      //printf("\n Outlinks Done %f",dp);

      pthread_barrier_wait(arg->barrier);

      v=0;

      //Calculate Pageranks
      for(v=i_start;v<i_stop;v++)
      {
         if(exist[v]==1)   //if vertex exists
         {
            pgtmp[v] = r;//dp + (r)/N_real;     //dangling pointer usage commented out
            //printf("\n pgtmp:%f test:%d",pgtmp[uu],test[uu]);
            for(int j=0;j<test[v];j++)
            {
               //if inlink
               //printf("\nuu:%d id:%d",uu,W_index[uu][j]);
               pgtmp[v] = pgtmp[v] + (d*PR[W_index[v][j]]/outlinks[W_index[v][j]]);  //replace d with dp if dangling PRs required
            }
         }
				 if(pgtmp[v]>=1.0)
					 pgtmp[v] = 1.0;
      }
      //printf("\n Ranks done");

      pthread_barrier_wait(arg->barrier);

      //Put temporary pageranks into final pageranks
      for(v=i_start;v<i_stop;v++)
      {
         if(exist[v]==1)
         {
            PR[v] = pgtmp[v];
            //printf("\n %f",D[uu]);
         }
      }

      pthread_barrier_wait(arg->barrier);
      iterations--;
   }

   //printf("\n %d %d",tid,terminate);
   return NULL;
}


//Main 
int main(int argc, char** argv)
{

   FILE *file0 = NULL;
   FILE *f = NULL;
   int N = 0;                         //Total vertices
   int DEG = 0;                       //Edges per vertex
   const int select = atoi(argv[1]);  //0 for synthetic, 1 for file read
   char filename[100];

   //For graph through file input
   if(select==1)
   {
      //printf("Please Enter The Name Of The File You Would Like To Fetch\n");
      //scanf("%s", filename);
      strcpy(filename,argv[3]);
      //filename = argv[2];
      f = fopen(filename,"r");
   }

   int lines_to_check=0;      //file processing variables
   char c;
   int number0;
   int number1;
   int inter = -1;

   //For graph through file input, upper limits
   if(select==1)
   {
      N = 2097152; //4194304; //can be read from file if needed, this is a default upper limit
      DEG = 16;     //also can be reda from file if needed, upper limit here again
   }

   const int P = atoi(argv[2]);  //number of threads
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

   //Memory allocations
   double* PR;
   int* Q;
   if(posix_memalign((void**) &PR, 64, N * sizeof(double)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &Q, 64, N * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &test, 64, N * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &exist, 64, N * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &test2, 64, N * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &dangling, 64, N * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &pgtmp, 64, N * sizeof(double)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &outlinks, 64, N * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   pthread_barrier_t barrier;

   double** W = (double**) malloc(N*sizeof(double*));
   int** W_index = (int**) malloc(N*sizeof(int*));
   for(int i = 0; i < N; i++)
   {
      //W[i] = (int *)malloc(sizeof(int)*N);
      int ret = posix_memalign((void**) &W[i], 64, DEG*sizeof(double));
      int re1 = posix_memalign((void**) &W_index[i], 64, DEG*sizeof(int));
      if (ret != 0 || re1!=0)
      {
         fprintf(stderr, "Could not allocate memory\n");
         exit(EXIT_FAILURE);
      }
   }

   //Memory initialization
   for(int i=0;i<N;i++)
   {
      for(int j=0;j<DEG;j++)
      {
         W[i][j] = 1000000000;
         W_index[i][j] = INT_MAX;
      }
      test[i]=0;
      exist[i]=0;
      test2[i]=0;
      dangling[i]=0;
      outlinks[i]=0;
   }

   //If graph read from file
   if(select==1)
   {
      int lines=0;
      for(c=getc(f); c!=EOF; c=getc(f))
      {
         if(c=='\n')
            lines++;
      }
      fclose(f);

      file0 = fopen(filename,"r");

      for(c=getc(file0); c!=EOF; c=getc(file0))
      {
         if(c=='\n')
            lines_to_check++;

         if(lines_to_check>3 && lines_to_check<lines)
         {   
            int f0 = fscanf(file0, "%d %d", &number0,&number1);
            if(f0 != 2 && f0 != EOF)
            {
               printf ("Error: Read %d values, expected 2. Parsing failed.\n",f0);
               exit (EXIT_FAILURE);
            }

            inter = test[number1]; 

            W[number0][inter] = 0; //drand48();
            W_index[number1][inter] = number0;
            test[number1]++;
            outlinks[number0]++;
            exist[number0]=1; exist[number1]=1;
            test2[number0]=1;
            dangling[number1]=1;
            if(number0 > nodecount)
               nodecount = number0;
            if(number1 > nodecount)
               nodecount = number1;
         }   
      }
      nodecount++;
      for(int i=0;i<N;i++)
      { 
         if(test2[i]==1 && dangling[i]==1)
            dangling[i]=0;
      }
      //printf("\n\nFile Read %d",lines_to_check);

      // Calculate total nodes, in order to calculate an initial weight.
      /*for(int i=0;i<N;i++) 
        {
        if (test1[i]==1) 
        nodecount++;
        }*/
      printf("\nLargest Vertex: %d",nodecount);
      N = nodecount;
   }

   //If graph to be generated synthetically
   if(select==0)
   {
		 int div = N;
		 if(DEG>=N)
			 div = DEG;
      init_weights(N, DEG, W, W_index);
      for(int i=0;i<N;i++)
      {
         outlinks[i] = rand()%(div); //random outlinks
         if(outlinks[i]==0)
            outlinks[i] = N;
      }
   }

   //Synchronization parameters
   pthread_barrier_init(&barrier, NULL, P);
   pthread_mutex_init(&lock, NULL);

   for(int i=0; i<N; i++)
   {
      if(select==0)
      {
         exist[i]=1;
         if(i%100==0)
         {
            dangling[i]=1;
         }
         test[i] = DEG;
      }
      pthread_mutex_init(&locks[i], NULL);
   }

   //Initialize PageRanks
   initialize_single_source(PR, Q, 0, N, 0.15);
   printf("\nInitialization Done");

   //Thread arguments
   for(int j = 0; j < P; j++) {
      thread_arg[j].local_min  = local_min_buffer;
      thread_arg[j].global_min = &global_min_buffer;
      thread_arg[j].Q          = Q;
      thread_arg[j].PR         = PR;
      thread_arg[j].W          = W;
      thread_arg[j].W_index    = W_index;
      thread_arg[j].tid        = j;
      thread_arg[j].P          = P;
      thread_arg[j].N          = N;
      thread_arg[j].DEG        = DEG;
      thread_arg[j].barrier    = &barrier;
   }

   //Start CPU clock
   struct timespec requestStart, requestEnd;
   clock_gettime(CLOCK_REALTIME, &requestStart);

   // Enable Graphite performance and energy models
   //CarbonEnableModels();

   //Spawn Threads
   for(int j = 1; j < P; j++) {
      pthread_create(thread_handle+j,
            NULL,
            do_work,
            (void*)&thread_arg[j]);
   }
   do_work((void*) &thread_arg[0]);  //Spawn main

   //Join threads
   for(int j = 1; j < P; j++) { //mul = mul*2;
      pthread_join(thread_handle[j],NULL);
   }

   // Disable Graphite performance and energy models
   //CarbonDisableModels();

   //Read clock and print time
   clock_gettime(CLOCK_REALTIME, &requestEnd);
   double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
   printf( "\nTime:%lf seconds\n", accum );

   //printf("\ndistance:%d \n",D[N-1]);

   //Print pageranks to file
   FILE *f1 = fopen("file.txt", "w");

   for(int i = 0; i < N; i++) {
      if(exist[i]==1)
         fprintf(f1,"pr(%d) = %f\n", i,PR[i]);
   }
   printf("\n");
   fclose(f1);

   return 0;
}

int initialize_single_source(double*  PR,
      int*  Q,
      int   source,
      int   N,
      double initial_rank)
{
   for(int i = 0; i < N; i++)
   {
      PR[i] = 0.15;//initial_rank;
      pgtmp[i] = 0.15;//initial_rank;
      Q[i] = 0;
   }

   //  D[source] = 0;
   return 0;
}

void init_weights(int N, int DEG, double** W, int** W_index)
{
   // Initialize to -1
   for(int i = 0; i < N; i++)
      for(int j = 0; j < DEG; j++)
         W_index[i][j]= -1;

   // Populate Index Array
   for(int i = 0; i < N; i++)
   {
      int max = DEG;
      for(int j = 0; j < DEG; j++)
      {
         if(W_index[i][j] == -1)
         {
            int neighbor = rand()%(i+max*2);
            if(neighbor<j)
               W_index[i][j] = neighbor;
            else
               W_index[i][j] = N-1;
         }
         else
         {
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
         /*if(v > 0.8 || W_index[i][j] == -1)
           {       W[i][j] = MAX;
           W_index[i][j] = -1;
           }

           else*/ if(W_index[i][j] == i)
         W[i][j] = 0;

         else
            W[i][j] = 0;//(double) (v) + 1;
      }
   }
}
