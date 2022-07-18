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
double dp_tid[1024];            //dangling pages for each thread, reduced later by locks
int terminate = 0;  //terminate variable
int *test;          //test variable arrays for graph storage
int *exist;
int *test2;
int *dangling;
int *outlinks;      //array for outlinks
double dp = 0;      //dangling pointer variable
double *pgtmp;      //temporary pageranks
thread_arg_t thread_arg[1024];    //MAX threads
pthread_t   thread_handle[1024];  //pthread handlers

//Function declarations
int initialize_single_source(double* PR, int source, int N, double initial_rank);
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
   int DEG                    = arg->DEG;
   //double tid_d = tid;
   int P = arg->P;

   //Allocate work among threads
   //double start_d = (tid_d) * (N_real/P_d);
   //double stop_d = (tid_d+1.0) * (N_real/P_d);
   int i_start = (double)tid     * (double)(N)/(double)P; 
   int i_stop  = (double)(tid+1) * (double)(N)/(double)P;  
   //printf("\n TID: %d %d %d",tid, i_start, i_stop); 

   //Pagerank iteration count
   int iterations = 1;

   //Barrier before starting work
   pthread_barrier_wait(arg->barrier);

   while(iterations>0)
   {
      if(tid==0)
         dp=d;
      pthread_barrier_wait(arg->barrier);

      //for no outlinks, dangling pages calculation
      for(v=i_start;v<i_stop;v++)
      {
         if(dangling[v]==1)
         {
            pthread_mutex_lock(&lock);
            dp = dp + d*(PR[v]/N_real);
            pthread_mutex_unlock(&lock);
            //dp_tid[tid] = dp_tid[tid] + d*(PR[v]/N_real);
            //printf("\n %f %f %f %f",dp,d,D[uu],N_real);
         }
      }
      //pthread_mutex_lock(&lock);
      //dp = dp + dp_tid[tid];
      //pthread_mutex_unlock(&lock);
      //pthread_barrier_wait(arg->barrier);
      
      //if(tid==0)
      //  printf("\n Outlinks Done %f",dp);

      pthread_barrier_wait(arg->barrier);

      v=0;

      //Calculate Pageranks
      for(v=i_start;v<i_stop;v++)
      {
         if(exist[v]==1)   //if vertex exists
         {
            pgtmp[v] = r;//dp + (r)/N_real;     //dangling pointer usage commented out
            //printf("\n pgtmp:%f test:%d",pgtmp[uu],test[uu]);
            for(int j=0;j<=DEG;j++)
            {
               if(W_index[v][j]>N)
								 break;
               //if inlink
               //printf("\nuu:%d id:%d",uu,W_index[uu][j]);
               pgtmp[v] = pgtmp[v] + (dp*PR[W_index[v][j]]/outlinks[W_index[v][j]]);  //replace d with dp if dangling PRs required
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
   int N = 0;                         //Total vertices
   int DEG = 0;                       //Edges per vertex
   const int select = atoi(argv[1]);  //0 for synthetic, 1 for file read
   char filename[100];

   //For graph through file input
   if(select==1)
   {
      strcpy(filename,argv[3]);
      file0 = fopen(filename,"r");
   }

   int lines_to_check=0;      //file processing variables
   char c;
   int number0;
   int number1;

   //Read Max Vertices and Edges
   if(select==1)
   {
      N = 0;//2097152; //can be read from file if needed, this is a default upper limit
      DEG = 0;//26;     //also can be reda from file if needed, upper limit here again
      FILE *file_gr = NULL;
      const char *filename0 = argv[3];
      file_gr = fopen(filename0,"r");
      char ch0;
      int number_of_lines0 = 0;
      while(EOF != (ch0=getc(file_gr)))
      {
        if(ch0=='\n')
          number_of_lines0++;
        if(number_of_lines0>3)
        {
          int f0 = fscanf(file_gr, "%d %d", &number0, &number1);
          if(f0 != 2 && f0 !=EOF)
          {
            printf ("Error: Read %d values, expected 2. Parsing failed.\n",f0);
            exit (EXIT_FAILURE);
          }
          if(number0>N)
            N = number0;
          if(number1>N)
            N = number1;
        }
      }
      fclose(file_gr); //Now N has the largest Vertex ID
      N++;
      
      int *temp;
      number_of_lines0 = 0;
      if(posix_memalign((void**) &temp, 64, N * sizeof(int)))
      {
        fprintf(stderr, "Allocation of memory failed\n");
        exit(EXIT_FAILURE);
      }
      for(int i=0;i<N;i++)
        temp[i] = 0;
      file_gr = fopen(filename0,"r");
      while(EOF != (ch0=getc(file_gr)))
      {
        if(ch0=='\n')
          number_of_lines0++;
        if(number_of_lines0>3)
        {
          int f0 = fscanf(file_gr, "%d %d", &number0, &number1);
          if(f0 != 2 && f0 !=EOF)
          {
            printf ("Error: Read %d values, expected 2. Parsing failed.\n",f0);
            exit (EXIT_FAILURE);
          }
          temp[number0]++;
        }
      }
      fclose(file_gr);
      for(int i=0;i<N;i++)
			{
        if(temp[i]>DEG)
          DEG = temp[i];
      }
      free(temp);
      printf("\n.gr graph with parameters: Vertices:%d Degree:%d LinesInFile:%d",N,DEG,number_of_lines0);
   }

   const int P = atoi(argv[2]);  //number of threads
   
	 if(select==0)
   {
      N = atoi(argv[3]);
      DEG = atoi(argv[4]);
      printf("\nUniform Random Graph with Parameters: N:%d DEG:%d\n",N,DEG);
   }

   //Memory allocations
   double* PR;
   if(posix_memalign((void**) &PR, 64, (N+1) * sizeof(double)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &test, 64, (N+10) * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &exist, 64, (N+1) * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &test2, 64, (N+1) * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &dangling, 64, (N+1) * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &pgtmp, 64, (N+1) * sizeof(double)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &outlinks, 64, (N+1) * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   pthread_barrier_t barrier;

   double** W = (double**) malloc((N+1)*sizeof(double*));
   int** W_index = (int**) malloc((N+1)*sizeof(int*));
   for(int i = 0; i <= N; i++)
   {
      //W[i] = (int *)malloc(sizeof(int)*N);
      int ret = posix_memalign((void**) &W[i], 64, (DEG+1)*sizeof(double));
      int re1 = posix_memalign((void**) &W_index[i], 64, (DEG+1)*sizeof(int));
      if (ret != 0 || re1!=0)
      {
         fprintf(stderr, "Could not allocate memory\n");
         exit(EXIT_FAILURE);
      }
   }

   //Memory initialization
   for(int i=0;i<=N;i++)
   {
      for(int j=0;j<=DEG;j++)
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
	 printf("\nMemory Initialized");

   //If graph read from file
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

            //inter = 1;//test[number1];
						//if(lines_to_check>5532000)
						//printf("\n%d\n",number0);
            W[number0][test[number1]] = 0; //drand48();
            W_index[number1][test[number1]] = number0;
            //test[number1]++;
            outlinks[number0]++;
            exist[number0]=1; exist[number1]=1;
            test2[number0]=1;
            dangling[number1]=1;

         }   
      }
			free(test);
      printf("\nFile Read");
      for(int i=0;i<=N;i++)
      { 
         if(test2[i]==1 && dangling[i]==1)
            dangling[i]=0;
      }
      printf("\nLargest Vertex: %d",N);
   }

   //If graph to be generated synthetically
   if(select==0)
   {
		 int div = N;
		 if(DEG>=N)
			 div = DEG;
      init_weights(N, DEG, W, W_index);
      for(int i=0;i<=N;i++)
      {
         outlinks[i] = rand()%(div); //random outlinks
         if(outlinks[i]==0)
            outlinks[i] = N;
      }
   }

   //Synchronization parameters
   pthread_barrier_init(&barrier, NULL, P);
   pthread_mutex_init(&lock, NULL);

   for(int i=0; i<=N; i++)
   {
      if(select==0)
      {
         exist[i]=1;
         if(i%100==0)
         {
            dangling[i]=1;
         }
         //test[i] = DEG;
      }
      //pthread_mutex_init(&locks[i], NULL);
   }

   //Initialize PageRanks
   initialize_single_source(PR, 0, N, 0.15);
   printf("\nInitialization Done");

   //Thread arguments
   for(int j = 0; j < P; j++) {
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

   for(int i = 0; i <= N; i++) {
      if(exist[i]==1)
         fprintf(f1,"pr(%d) = %f\n", i,PR[i]);
   }
   printf("\n");
   fclose(f1);

   return 0;
}

int initialize_single_source(double*  PR,
      int   source,
      int   N,
      double initial_rank)
{
   for(int i = 0; i <= N; i++)
   {
      PR[i] = 0.15;//initial_rank;
      pgtmp[i] = 0.15;//initial_rank;
   }

   //  D[source] = 0;
   return 0;
}

void init_weights(int N, int DEG, double** W, int** W_index)
{
   // Initialize to -1
   for(int i = 0; i <= N; i++)
      for(int j = 0; j < DEG; j++)
         W_index[i][j]= -1;

   // Populate Index Array
   for(int i = 0; i <= N; i++)
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
               W_index[i][j] = N;
         }
         else
         {
         }
         if(W_index[i][j]>N)
         {
            W_index[i][j] = N;
         }
      }
   }

   // Populate Cost Array
   for(int i = 0; i <= N; i++)
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
