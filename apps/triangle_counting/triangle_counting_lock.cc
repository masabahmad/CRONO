#include <cstdio>
#include <cstdlib>
#include <pthread.h>
//#include "carbon_user.h"     /* For the Graphite Simulator*/
#include <time.h>
#include <sys/timeb.h>
#include <getopt.h>
#include "../../common/mtx.h"

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
pthread_mutex_t *locks;
//pthread_mutex_t locks[4194304];      //Lock for each vertex
int local_min_buffer[1024];
double Total_tid[1024] = {0};           //To store triangles per thread
int global_min_buffer;
int P_global = 256;
//int **W;
//int **W_index;
//int *D;
//int *Q;
//int *edges;
//int *exist;
//int *unique;
//int largest=0;
//int degree=0;
double Total_Tri = 0;                 //Stores total triangles
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
   //const int N              = arg->N;
   int v                    = 0;              //for each vertex
   int local_count          = 0;
   double P_d = P;
   double tid_d = tid;
   double largest_d = largest + 1.0;

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
      if(edges[v]!=0)
      {
         for(int i = 0; i < edges[v]; i++)
         {
            int neighbor = W_index[v][i];
            if(neighbor>=largest)
               continue;
            pthread_mutex_lock(&locks[neighbor]);
            D[neighbor]++;   //Add edges
            Q[neighbor] = 0;
            pthread_mutex_unlock(&locks[neighbor]);
         }
      }
   }

   pthread_barrier_wait(arg->barrier_total);
   //printf("\n Done with First Phase");
   //for(int i=0;i<largest;i++)
   //{
   //   printf("\n %d",D[i]);
   //}
   
   //Find triangles for each thread
   for(v=start;v<stop;v++)
   {
      if(edges[v]!=0)
      {
				//if(v<50)
				//	printf("\n %d",D[v]);
         ////unsigned int ret = -1;
         float temp = D[v];
         /*while (D[uu] != 0) 
           {
           D[uu] >>= 1;
           ret++;
           }*/
         //ret = temp/3;
         ////D[v]=ret;
         ////if(D[v]>=1)
         //{
            //pthread_mutex_lock(&lock);
            local_count = local_count+temp;
            //pthread_mutex_unlock(&lock);
         //}
      }
   }
   Total_tid[tid] = local_count;
   //printf("\n \n %f",Total_tid[tid]);

   pthread_barrier_wait(arg->barrier_total);

   //The master thread sums up all triangles
   if(tid==0)
   {
      for(int i=0;i<P;i++)
      {
         Total_Tri = Total_Tri + Total_tid[i];
      }
      Total_Tri = Total_Tri/3;
   }

   pthread_barrier_wait(arg->barrier_total);

   return NULL;
}


int main(int argc, char** argv)
{
	 /*int select_opt=0;
	 int ca;
	
  while ((ca = getopt(argc, argv, "s:::t::m::")) != -1)
     switch(ca)
		 {
			 case 's':
         select_opt = 0;
				 break;
			 case 't':
				 select_opt = 1;
				 break;
			 case 'm':
				 select_opt = 2;
				 break;
			 default:
				 select_opt = 0;
				 break;
		 }*/
	 
   //char filename[100];
   FILE *file0 = NULL;
   int N = 0;
   int DEG = 0;
   int select = atoi(argv[1]);

   //For reading graph from text file
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
   int *D; int *Q;
   if(posix_memalign((void**) &D, 64, (N+1) * sizeof(int)))
   {   
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }   
   if(posix_memalign((void**) &Q, 64, (N+1) * sizeof(int)))
   {   
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }

   int d_count = N;
   pthread_barrier_t barrier_total;
   pthread_barrier_t barrier;

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
            //exist[number0]=1; exist[number1]=1;
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
   locks = (pthread_mutex_t*) malloc((largest+16) * sizeof(pthread_mutex_t));

   for(int i=0; i<largest+1; i++)
   {
      if(select==0)
      {
         //exist[i]=1;
         edges[i]=DEG;
      }
      //if(exist[i]==1)
         pthread_mutex_init(&locks[i], NULL);
   }

   //Initialize 1-d arrays
   if(select!=2)
   {
     initialize_single_source(D, Q, 0, largest);
   }
   else
     initialize_single_source(D, Q, 0, largest);

	 //Print Graph
   /*printf("\n Printing Graph \n");
   for(int i=0;i<largest+1;i++)
   {
		 if(exist[i]==1)
       for(int j=0;j<edges[i];j++)
       {
         printf(" %d %d ", i, W_index[i][j]);
       }
       printf("\n");
   }
   printf("\n");*/

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

   long long int count = Total_Tri;
   printf("\nTriangles=%lld \n",count); //%lld for long long int

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

