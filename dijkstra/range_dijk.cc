#include <cstdio>
#include <cstdlib>
#include <pthread.h>
//#include "easyperf.h"
//#include "carbon_user.h"
#include <time.h>
#include <sys/timeb.h>

#define MAX            100000000
#define INT_MAX        100000000
// #define DEBUG              1
#define BILLION 1E9


int _W[8][8] =
{
  {0,   2,      1,      17,     MAX,    MAX,    MAX,    MAX},
  {2,   0,      MAX,    MAX,    2,      6,      MAX,    MAX},
  {1,   MAX,    0,      MAX,    MAX,    MAX,    MAX,    8},
  {17,  MAX,    MAX,    0,      MAX,    2,      1,      9},
  {MAX, 2,      MAX,    MAX,    0,      4,      MAX,    MAX},
  {MAX, 6,      MAX,    2,      4,      0,      5,      MAX},
  {MAX, MAX,    MAX,    1,      MAX,    5,      0,      3},
  {MAX, MAX,    8,      9,      MAX,    MAX,    3,      0}
};

int min = INT_MAX;
int min_index = 0;
pthread_mutex_t lock;
pthread_mutex_t locks[4194304];
int u = 0;


  void init_weights(int N, int DEG, int** W, int** W_index)
  {
          int range = DEG + (N - DEG)/16;
  
          // Initialize to -1
          for(int i = 0; i < N; i++)
                  for(int j = 0; j < DEG; j++)
                          W_index[i][j]= -1;
  
          // Populate Index Array
          for(int i = 0; i < N; i++)
          {
                  int last = 0;
                  int min = 0;
                  int max = DEG;
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





int initialize_single_source(int* D, int* Q, int* S, int source, int N);
void relax(int u, int i, volatile int* D, int* S, int** W, int** W_index, int N);
int get_local_min(volatile int* Q, volatile int* D, int start, int stop, int N, int** W_index, int** W, int u);

typedef struct
{
  int*      local_min;
  int*      global_min;
  int*      Q;
  int*      D;
	int*      S;
  int**     W;
  int**     W_index;
  int*      d_count;
  int       tid;
  int       P;
  int       N;
  int       DEG;
  pthread_barrier_t* barrier;
} thread_arg_t;

int local_min_buffer[1024];
int global_min_buffer;
int terminate = 0;
int range=1;
int old_range =1;
int difference=0;
int pid=0;
thread_arg_t thread_arg[1024];
pthread_t   thread_handle[1024];

void* do_work(void* args)
{
  volatile thread_arg_t* arg = (thread_arg_t*) args;

  volatile int* count      = arg->d_count;
  volatile int* global_min = arg->global_min;
  volatile int* local_min  = arg->local_min;
  int tid                  = arg->tid;
  int P                    = arg->P;
  volatile int* Q          = arg->Q;
  int* D                   = arg->D;
	int* S                   = arg->S;
  int** W                  = arg->W;
  int** W_index            = arg->W_index;
  const int N              = arg->N;
  const int DEG            = arg->DEG;
  int local_count          = N;
  int i, j, po;
	int uu = 0;

  int a = 0;
  int i_start =  0;  //tid    * DEG / (arg->P);
  int i_stop  = 0;   //(tid+1) * DEG / (arg->P);
	int start = 0;
	int stop = 1;
  //int difference =0;

	pthread_barrier_wait(arg->barrier);

  while(terminate==0)
  {
	  for(uu=start;uu<stop;uu++)
		{
    for(int i = 0; i < DEG; i++)
    {
      int neighbor = W_index[uu][i];
			//
			pthread_mutex_lock(&locks[neighbor]);

			if(uu>=N)
				terminate=1;


			if((D[W_index[uu][i]] > (D[uu] + W[uu][i]) && W_index[uu][i]!=-1 && W[uu][i]!=INT_MAX/* && Q[uu]==1*/))
			{	
				D[W_index[uu][i]] = D[uu] + W[uu][i];
				S[W_index[uu][i]] = uu;
			}
     Q[uu]=0;// po=u;
		//if(W_index[uu][i]==-1)
		 //W[uu][i]=INT_MAX;
	  
			pthread_mutex_unlock(&locks[neighbor]);
    }
		}

   //pthread_barrier_wait(arg->barrier);
		
	 if(tid==0)
		{  //pthread_mutex_lock(&lock);
			 old_range=range;
		   range = range*DEG;
       
			 if(old_range==1)
				 old_range=0;
       
			 if(range>=N)
				 range=N;
       //pthread_mutex_unlock(&lock);
			 //printf("\nold:%d new:%d",old_range,range);
			
			 difference = range-old_range;
			if(difference<P)
			{   
					pid=difference;
		  }   
			else
				  pid=P;
			if(pid==0)
				pid=P;
		}

	 //pthread_mutex_lock(&lock);
	 //if(u<=N)
     //u++;
	 //pthread_mutex_unlock(&lock);
	
    /*pthread_mutex_lock(&lock);
		if(uu>=N-1 || range>=N)
		{   
			//pthread_mutex_lock(&lock);
			terminate=1;
			//pthread_mutex_unlock(&lock);
		} 	
	  pthread_mutex_unlock(&lock);*/

		pthread_barrier_wait(arg->barrier);
	
		start = old_range  +  (difference/P)*(tid);            //(tid    * range)  / (arg->P)    + old_range;
		stop  = old_range  +  (difference/P)*(tid+1);            //((tid+1) * range)  / (arg->P)   + old_range;
	  
		if(stop>range)
		 stop=range;	

    //if(tid==0)
		{ pthread_mutex_lock(&lock);
       if(start==N || uu>N-1)
				 terminate=1;
		} pthread_mutex_unlock(&lock);

    //pthread_barrier_wait(arg->barrier);
		
		//printf("\n TID:%d   start:%d stop:%d terminate:%d",tid,start,stop,terminate);
	}
  //printf("\n %d %d",tid,terminate);
  return NULL;
}


int main(int argc, char** argv)
{ //int mul = W_index[0][0];
  /*if (argc != 3)
  {
    fprintf(stderr, "Usage: ./dijkstra <Number of Processors> <Number of Vertices>\n");
    exit(EXIT_FAILURE);
  }*/
//printf("main start\n");
  // Start the simulator
  //CarbonStartSim(argc, argv);

  const int P = atoi(argv[1]);
  const int N = atoi(argv[2]);
  const int DEG = atoi(argv[3]);
//printf("P: %d",P);
//printf("N: %d",N);
//printf("DEG: %d",DEG);

        if (DEG > N)
        {
                fprintf(stderr, "Degree of graph cannot be grater than number of Vertices\n");
                exit(EXIT_FAILURE);
        }

  int* D;
  int* Q;
	int* S;
  posix_memalign((void**) &D, 64, N * sizeof(int));
  posix_memalign((void**) &Q, 64, N * sizeof(int));
	posix_memalign((void**) &S, 64, N * sizeof(int));
  int d_count = N;
  pthread_barrier_t barrier;

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
  init_weights(N, DEG, W, W_index);
  /*for(int i = 0;i<N;i++)
  {
        for(int j = 0;j<N;j++)
        {
                printf(" %d ",W[i][j]);
        }
        printf("\n");
  }*/

  pthread_barrier_init(&barrier, NULL, P);
  pthread_mutex_init(&lock, NULL);
	for(int i=0; i<N; i++)
		pthread_mutex_init(&locks[i], NULL);
  
	initialize_single_source(D, Q, S, 0, N);

  for(int j = 0; j < P; j++) {
    thread_arg[j].local_min  = local_min_buffer;
    thread_arg[j].global_min = &global_min_buffer;
    thread_arg[j].Q          = Q;
    thread_arg[j].D          = D;
		thread_arg[j].S          = S;
    thread_arg[j].W          = W;
    thread_arg[j].W_index    = W_index;
    thread_arg[j].d_count    = &d_count;
    thread_arg[j].tid        = j;
    thread_arg[j].P          = P;
    thread_arg[j].N          = N;
    thread_arg[j].DEG        = DEG;
    thread_arg[j].barrier    = &barrier;
  }
int mul = 2;
  // Enable performance and energy models
  //CarbonEnableModels();

struct timespec requestStart, requestEnd;
clock_gettime(CLOCK_REALTIME, &requestStart);

         //For the easyperf performance counter API
         //uint64_t start[4], end[4];
         //unsigned long i;
		     //perf_init(4, I7_L2_MISS, I7_L3_REFS, I7_L3_MISS, EV_INSTR);
				 //perf_read_all(start);

  for(int j = 1; j < P; j++) {
    pthread_create(thread_handle+j,
                   NULL,
                   do_work,
                   (void*)&thread_arg[j]);
  }
  do_work((void*) &thread_arg[0]);

  for(int j = 1; j < P; j++) { //mul = mul*2;
    pthread_join(thread_handle[j],NULL);
  }

	//For the easyperf performance counter API
	//perf_read_all(end);
	//printf("\nL2-Misses: %10lu, \nL3-Refs: %10lu, \nL3-Misses: %10lu, \nInst.: %10lu", end[0] - start[0], end[1] - start[1], end[2] - start[2], end[3] - start[3]);
  //perf_close();

	clock_gettime(CLOCK_REALTIME, &requestEnd);
	  double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
		  printf( "\n%lf\n", accum );

  // Enable performance and energy models
  //CarbonDisableModels();
  //printf("\ndistance:%d \n",D[N-1]);

    /*for(int i = 0; i < N; i++) {
      printf(" %d ", D[i]);
    }
    printf("\n");
*/
  // Stop the simulator
  //CarbonStopSim();
  return 0;
}

int initialize_single_source(int*  D,
                             int*  Q,
														 int*  S,
                             int   source,
                             int   N)
{
  for(int i = 0; i < N+1; i++)
  {
    D[i] = INT_MAX;
    Q[i] = 1;
		S[i] = INT_MAX;
  }

  D[source] = 0;
	S[source] = 0;
  return 0;
}

int get_local_min(volatile int* Q, volatile int* D, int start, int stop, int N, int** W_index, int** W, int u)
{
  int min = INT_MAX;
  int min_index = N;

  for(int i = start; i < stop; i++) 
  {
   if(W_index[u][i]==-1 || W[u][i]==INT_MAX)
     continue;
    if(D[i] < min && Q[i]) 
    {
      min = D[i];
      min_index = W_index[u][i];
    }
  }
  return min_index;
}

void relax(int u, int i, volatile int* D, int* S, int** W, int** W_index, int N)
{
  if((D[W_index[u][i]] > (D[u] + W[u][i]) && (W_index[u][i]!=-1 && W_index[u][i]<N && W[u][i]!=INT_MAX)))
    D[W_index[u][i]] = D[u] + W[u][i];
}
