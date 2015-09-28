#include <cstdio>
#include <cstdlib>
#include <pthread.h>
//#include "carbon_user.h"
#include <time.h>
#include <sys/timeb.h>

#define MAX            100000000
#define INT_MAX        100000000
// #define DEBUG              1
#define BILLION 1E9

int min = INT_MAX;
int min_index = 0;
pthread_mutex_t lock;
pthread_mutex_t locks[2097152];
int u = 0;


  void init_weights(int N, int DEG, float** W, int** W_index)
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

//function declarations
int initialize_single_source(int* D, float* Q, int source, int N);

//Thread Structure
typedef struct
{
  float*      Q;
  int*        D;
  float**     W;
  int**       W_index;
  float*      mod_gain;
  float*      total_mod_gain;
  int*        comm;
  float*      C;
  int*        d_count;
  int         tid;
  int         P;
  int         N;
  int         DEG;
  pthread_barrier_t* barrier_total;
  pthread_barrier_t* barrier;
} thread_arg_t;

//Global Variables
int Total_tid[1024] = {0};
int out_table[1024] = {0};
int in_table[1024] = {0};
int terminate = 0;
int iterations = 0;
int range=1;
int old_range =1;
int difference=0;
int pid=0;
int start = 64;
int P_global = 256;
int change = 0;
long long Total = 0;
thread_arg_t thread_arg[1024];
pthread_t   thread_handle[1024];
int up = -1;

//Main Work Function
void* do_work(void* args)
{
  volatile thread_arg_t* arg = (thread_arg_t*) args;

  volatile int* count      = arg->d_count;
  int tid                  = arg->tid;
  int P                    = arg->P;
  float* Q                 = arg->Q;
  int* D                   = arg->D;
  float** W                = arg->W;
  int** W_index            = arg->W_index;
  float* mod_gain          = arg->mod_gain;
  float* total_mod_gain    = arg->total_mod_gain;
  int* comm                = arg->comm;
  float* C                 = arg->C;
  const int N              = arg->N;
  const int DEG            = arg->DEG;
  int local_count          = 0;
  int uu = 0;
  float modularity = 0;
  P = start;
  int i;
  int index = 0;
  int index_id = 0;
  float sum_tot = 0;
  float sum_in = 0;

  float total_edges = N*DEG;
  float mod_gain_temp = 0;
  float mod_gain_temp_temp = 0;
  float inv_total_edges = 2/total_edges;

  int a = 0;
  int start =  0;  //tid    * DEG / (arg->P);
  int stop  = 0;   //(tid+1) * DEG / (arg->P);

  start =  tid    *  (N) / (P);
  stop =  (tid+1) *  (N) / (P);

  //put each node in its own community

  //pthread_barrier_wait(arg->barrier_total);

  for(uu=start;uu<stop;uu++)
  {
    for(int i=0;i<DEG;i++)
    {
      comm[uu] = uu;
    }
  }

  uu = 0;
  up = 0;

  pthread_barrier_wait(arg->barrier_total);

  //static for now
  //can be done more for dynamic, need a while loop here for thay
  //initialization of communities
  while(local_count<iterations)	
  {
    for(uu=start;uu<stop;uu++)
    //while(uu<N-2)
    {
      //pthread_mutex_lock(&lock);
      //up++;
      //uu = up;
      //printf(" %d ",uu);
      //pthread_mutex_unlock(&lock);
  
      for(i = 0; i < DEG; i++)
      {
        //pthread_mutex_lock(&locks[neighbor]);

        int tempo = (inv_total_edges)*(inv_total_edges);
        tempo = tempo*2;
        int subtr = sum_tot*W[uu][i];
        subtr = subtr*tempo;
        subtr = W[uu][i] - subtr;
        mod_gain_temp_temp = (inv_total_edges)*(subtr);
        //total_mod_gain[uu] = total_mod_gain[uu] + mod_gain[uu];

        if(mod_gain_temp_temp>mod_gain_temp) 
        {
          mod_gain_temp = mod_gain_temp_temp;
          index = W_index[uu][i];
          index_id = i;
        }	

        //pthread_mutex_unlock(&locks[neighbor]);
      }
      mod_gain[uu] = mod_gain_temp;
      comm[uu] = index;   //cvk


      //update individual  sums
      //pthread_mutex_lock(&lock);
      sum_tot = sum_tot + W[uu][i];
      sum_in = sum_in + W[uu][i];
      //pthread_mutex_unlock(&lock);	
    }

    pthread_barrier_wait(arg->barrier_total);

    //reconstruct
    for(uu=start;uu<stop;uu++)
    {
      for(i=0;i<DEG;i++)
      {
        int neighbor = W_index[uu][i];
        pthread_mutex_lock(&locks[neighbor]);
          W_index[uu][i] = comm[neighbor];
          W[uu][i] = comm[uu] - comm[neighbor];
        pthread_mutex_unlock(&locks[neighbor]);
      }
    }

    pthread_barrier_wait(arg->barrier_total);

    //reduction heuristic approximate model, not very accurate though
    if(tid==0)
    {
      for(i=stop;i<N;i++)
      {
        for(int j=1;j<P;j++)
        {
          comm[i] = comm[stop-1];
        }
      }
    }

    pthread_barrier_wait(arg->barrier_total);

    /*if(tid==0)
    {
      for(i=0;i<N;i++)
      {
        printf(" %d ",comm[i]);
      }
    }*/

    local_count++;
    //printf("\n %d",local_count);

  }
  pthread_barrier_wait(arg->barrier_total);

  return NULL;
}


int main(int argc, char** argv)
{
  // Start the simulator
  //CarbonStartSim(argc, argv);

  const int P1 = atoi(argv[1]);
  const int N = atoi(argv[2]);
  const int DEG = atoi(argv[3]);
  iterations = 4;
  //const int iterations = atoi(argv[4]);

	int P = P1;
	P_global = P1;
	start = P1;
	old_range = change;
	range = change;


        if (DEG > N)
        {
                fprintf(stderr, "Degree of graph cannot be grater than number of Vertices\n");
                exit(EXIT_FAILURE);
        }

  int* D;
  float* C;
  float* Q;
  int* comm;
  float* mod_gain;
  float* total_mod_gain;
  int* deg_node;
  posix_memalign((void**) &D, 64, N * sizeof(int));
  posix_memalign((void**) &Q, 64, N * sizeof(float));
  posix_memalign((void**) &deg_node, 64, N * sizeof(int));

  posix_memalign((void**) &comm, 64, N * sizeof(int));
  posix_memalign((void**) &C, 64, N * sizeof(float));
  posix_memalign((void**) &mod_gain, 64, N * sizeof(float));
  posix_memalign((void**) &total_mod_gain, 64, N * sizeof(float));
  int d_count = N;
  pthread_barrier_t barrier_total;
  pthread_barrier_t barrier;

  float** W = (float**) malloc(N*sizeof(float*));
  int** W_index = (int**) malloc(N*sizeof(int*));
  for(int i = 0; i < N; i++)
  {
    W[i] = (float*)malloc(sizeof(float)*N);
    int ret = posix_memalign((void**) &W[i], 64, DEG*sizeof(float));
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

  pthread_barrier_init(&barrier_total, NULL, P);
  pthread_barrier_init(&barrier, NULL, P);

  pthread_mutex_init(&lock, NULL);

  for(int i=0; i<2097152; i++)
    pthread_mutex_init(&locks[i], NULL);
 
  initialize_single_source(D, Q, 0, N);

  for(int j = 0; j < P; j++) {
    thread_arg[j].Q          = Q;
    thread_arg[j].D          = D;
    thread_arg[j].W          = W;
    thread_arg[j].W_index    = W_index;
    thread_arg[j].comm       = comm;
    thread_arg[j].C          = C;
    thread_arg[j].mod_gain   = mod_gain;
    thread_arg[j].total_mod_gain = total_mod_gain;
    thread_arg[j].d_count    = &d_count;
    thread_arg[j].tid        = j;
    thread_arg[j].P          = P;
    thread_arg[j].N          = N;
    thread_arg[j].DEG        = DEG;
    thread_arg[j].barrier_total = &barrier_total;
		thread_arg[j].barrier    = &barrier;
  }
  int mul = 2;
  // Enable performance and energy models
  //CarbonEnableModels();

  struct timespec requestStart, requestEnd;
  clock_gettime(CLOCK_REALTIME, &requestStart);

  for(int j = 1; j < P; j++) {
    pthread_create(thread_handle+j,
                   NULL,
                   do_work,
                   (void*)&thread_arg[j]);
  }
  do_work((void*) &thread_arg[0]);

  printf("Threads Returned!");

  for(int j = 1; j < P; j++) { //mul = mul*2;
    pthread_join(thread_handle[j],NULL);
  }

	printf("Threads Joined!");

  clock_gettime(CLOCK_REALTIME, &requestEnd);
  double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
  printf( "%lf\n", accum );

  // Enable performance and energy models
  //CarbonDisableModels();
  //printf("\ndistance:%d \n",D[N-1]);

    //for(int i = 0; i < N; i++) {
    //  printf(" %d ", D[i]);
    //}
    //printf("\n");


		//printf("\n TOTAL=%d",Total);

  // Stop the simulator
  //CarbonStopSim();
  return 0;
}

int initialize_single_source(int*  D,
                             float*  Q,
                             int   source,
                             int   N)
{
  for(int i = 0; i < N+1; i++)
  {
    D[i] = 0;
    Q[i] = 0;
  }

  return 0;
}

