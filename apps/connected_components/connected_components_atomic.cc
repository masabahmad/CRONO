#include <cstdio>
#include <cstdlib>
#include <pthread.h>
//#include "carbon_user.h"     /* For the Graphite Simulator*/
#include <time.h>
#include <sys/timeb.h>
#include "../../common/mtx.h"
#include "../../common/barrier.h"

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
//pthread_mutex_t locks[4194304];    //locks for each vertex
int local_min_buffer[1024];
int global_min_buffer;
int P_global = 256;
//int *edges;
//int *exist;
//int largest=0;
int change = 0;
thread_arg_t thread_arg[1024];
pthread_t   thread_handle[1024];   //Max threads and pthread handlers

//Primary Parallel Function
void* do_work(void* args)
{
   //Thread Variables
   volatile thread_arg_t* arg = (thread_arg_t*) args;
   int tid                  = arg->tid;      //thread id
   int P                    = arg->P;        //Total threads
   int* D                   = arg->D;        //contains components
   //Output: Array D,with D[i] being the component
   //to which vertex i belongs
   //
   //int** W                = arg->W;
   int** W_index            = arg->W_index;  //Graph Structure
   int mod                  = 1;             //modularity
   int v                    = 0;             //current vertex
   int iterations           = 0;             //iterations  
   int start =  0;  //tid    * DEG / (arg->P);
   int stop  = 0;   //(tid+1) * DEG / (arg->P);
   double tid_d = tid;
   double P_d = P;
   double largest_d = largest;

   //Chunk work for threads via double precision
   double start_d = (tid_d) * (largest_d/P_d);
   double stop_d  = (tid_d+1.0) * (largest_d/P_d);
   start =  start_d;//tid    *  (largest+1) / (P);
   stop =  stop_d;//(tid+1) *  (largest+1) / (P);

   //pthread_barrier_wait(arg->barrier_total);
   barrier_wait();

   //Each component is its own, first phase
   for(v=start;v<stop;v++)
   {
      D[v] = v;
   }
   //printf("\n started P:%d %d",P, change); 
   //pthread_barrier_wait(arg->barrier_total);
   barrier_wait();

   //start connecting, second phase
   while(change<P)
   { 
      mod=0;
      iterations++;
      for(v=start;v<stop;v++)                  //for each vertex
      { 
         if(edges[v]!=0)                       //if vertex exists
         { 
            for(int i = 0; i < edges[v]; i++)   //for each edge
            {
               int neighbor = W_index[v][i];
               //pthread_mutex_lock(&locks[neighbor]);

               if((D[v] < D[neighbor]) && (D[neighbor] == D[D[neighbor]]))
               {
                  mod=1;                      //some change occured
                  D[D[neighbor]] = D[v];
               }				

               //pthread_mutex_unlock(&locks[neighbor]);
            }
         }
      }
      //printf("\n third phase");
      if(tid==0)
      {
        change=0; //iterations++;
      }
      //pthread_barrier_wait(arg->barrier_total);
      barrier_wait();
      //if(tid==0)                              //Reset Termination Condition
      //  change=0;

	  //Third phase, assign components
      for(v=start;v<stop;v++)
      {
         while(D[v] != D[D[v]])
         {
            D[v] = D[D[v]];
         }
      }

      //For termination Condition
      if(mod==0)
      {
        __sync_fetch_and_add(&change,1);
        //pthread_mutex_lock(&lock);
        // change++;
        //pthread_mutex_unlock(&lock);
       }

      //pthread_barrier_wait(arg->barrier_total);
      barrier_wait();
   }
   //printf("\n Iterations:%d",iterations);
   //pthread_barrier_wait(arg->barrier_total);
   return NULL;
}

//Main
int main(int argc, char** argv)
{
   int N = 0;  //Graph vertices
   int DEG = 0; //edges per vertex
   FILE *file0 = NULL;
   int select = atoi(argv[1]);

   //char filename[100];
   //If graph to be read from file
   if(select==1)
   {
      const char *filename = argv[3];
      //printf("Please Enter The Name Of The File You Would Like To Fetch\n");
      //scanf("%s", filename);
      file0 = fopen(filename,"r");
   }
   
   //Matrix .mtx file
   if(select==2)
   {
     const char *filename = argv[3];
     mtx(filename);
     //select = 1;
     file0 = fopen(conv_file,"r");
   }

   int lines_to_check=0;
   char c;
   int number0;
   int number1;
   int inter = -1; 

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
      printf("\n .gr graph with parameters: Vertices:%d Degree:%d \n",N,DEG);
   }
   
   if(select==2)
   {
     N = largest;
     DEG = degree;
     select = 1;
   }

   const int P1 = atoi(argv[2]);   //Max threads to be spawned

   int P = P1;
   P_global = P1;

   //If graph to be generated synthetically
   if(select==0)
   {
      N = atoi(argv[3]);      //Number of Vertices
      DEG = atoi(argv[4]);    //Edges per vertex
      printf("\nGraph with Parameters: N:%d DEG:%d\n",N,DEG);
   }

   if (DEG > N)
   {
      fprintf(stderr, "Degree of graph cannot be grater than number of Vertices\n");
      exit(EXIT_FAILURE);
   }

   //Memory allocations
   if(select!=2) {
   if(posix_memalign((void**) &edges, 64, (N+2) * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   //if(posix_memalign((void**) &exist, 64, (N+2) * sizeof(int)))
   //{
   //   fprintf(stderr, "Allocation of memory failed\n");
   //   exit(EXIT_FAILURE);
   //}
   }
   
   
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
  
  
   int d_count = N;
   pthread_barrier_t barrier_total;
   pthread_barrier_t barrier;

   //Graph data structures
   if(select!=2) {
   W = (int**) malloc((N+2)*sizeof(int*));
   W_index = (int**) malloc((N+2)*sizeof(int*));
   for(int i = 0; i < N+2; i++)
   {
      //W[i] = (int *)malloc(sizeof(int)*N);
      if(posix_memalign((void**) &W[i], 64, (DEG+1)*sizeof(int)))
      {
         fprintf(stderr, "Allocation of memory failed\n");
         exit(EXIT_FAILURE);
      }
      if(posix_memalign((void**) &W_index[i], 64, (DEG+1)*sizeof(int)))
      {
         fprintf(stderr, "Allocation of memory failed\n");
         exit(EXIT_FAILURE);
      }
   }

   //Initialize graph structures
   for(int i=0;i<N+2;i++)
   {
      for(int j=0;j<DEG+1;j++)
      {
         //W[i][j] = 1000000000;
         W_index[i][j] = INT_MAX;
      }
      edges[i]=0;
      //exist[i]=0;
   }
   }//select!=2

   //Read graph from file
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
            //exist[number0]=1; exist[number1]=1;
         }
      }
      printf("\nFile Read, Largest Vertex:%d",largest);
   }

   //Generate graph synthetically
   if(select==0)
   {
      init_weights(N, DEG, W, W_index);
      largest = N;
   }

   //Synchronization Initializations
   pthread_barrier_init(&barrier_total, NULL, P);
   pthread_barrier_init(&barrier, NULL, P);
   pthread_mutex_init(&lock, NULL);

   for(int i=0; i<largest+1; i++)
   {
      if(select==0)
      {
         //exist[i] = 1;
         edges[i] = DEG;
      }
      //if(exist[i]==1)
      //   pthread_mutex_init(&locks[i], NULL);
   }

   //Initialize arrays
   initialize_single_source(D, Q, 0, largest);

   PMAX = P; //for atomic barrier

   //Thread arguments
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

   //CPU clock
   struct timespec requestStart, requestEnd;
   clock_gettime(CLOCK_REALTIME, &requestStart);

   // Enable Graphite performance and energy models
   //CarbonEnableModels();

   //Spawn threads
   for(int j = 1; j < P; j++) {
      pthread_create(thread_handle+j,
            NULL,
            do_work,
            (void*)&thread_arg[j]);
   }
   do_work((void*) &thread_arg[0]);  //main spawns itself

   //Join threads
   for(int j = 1; j < P; j++) { //mul = mul*2;
      pthread_join(thread_handle[j],NULL);
   }

   // Disable Graphite performance and energy models
   //CarbonDisableModels();

   printf("\nThreads Joined!");

   clock_gettime(CLOCK_REALTIME, &requestEnd);
   double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
   printf( "\nTime Taken:\n%lf seconds\n", accum );

	 FILE * pfile;
	    pfile = fopen("myfile.txt","w");
   for(int j=0;j<largest-1;j++){
     if(edges[j]!=0)
     fprintf(pfile,"\n%d",D[j]);	
     }
	 fclose(pfile);
/*
   //Code Snippet taken from Jaiganesh, Jayadharini, Texas Tech University
   N = largest;
   int Unique[N];
   for (int i = 0; i < N; i++) Unique[i] = 0;
     for (int i = 0; i < N; i ++)
     {
       if (Unique[D[i]] == 0)
         Unique[D[i]] = 1;
     } 
     
   int count = 0;
   for (int i = 0; i < N; i++)
     count += Unique[i];
   printf("\nUnique Components Count:%d",count);
   */

   return 0;
}

int initialize_single_source(int*  D,
      int*  Q,
      int   source,
      int   N)
{
   for(int i = 0; i < N; i++)
   {
      D[i] = 0;
      Q[i] = 1;
   }

   //D[source] = 0;
   return 0;
}

void init_weights(int N, int DEG, int** W, int** W_index)
{
   // Initialize to -1
   for(int i = 0; i < N; i++)
      for(int j = 0; j < DEG; j++)
         W_index[i][j]= -1;

   // Populate Index Array
   for(int i = 0; i < N; i++)
   {
      //int last = 0;
      for(int j = 0; j < DEG; j++)
      {
         if(W_index[i][j] == -1)
         {        
            //int neighbor = i+j;
            W_index[i][j] = rand()%(N-1);

            /*if(neighbor > last)
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
            }*/
         }
         else
         {
            //last = W_index[i][j];
            W_index[i][j] = i;
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
