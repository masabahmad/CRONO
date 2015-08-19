#include <cstdio>
#include <cstdlib>
#include <pthread.h>
//#include "carbon_user.h"
#include <time.h>
#include <sys/timeb.h>

#define MAX            100000000
#define INT_MAX        100000000
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

//change the number of locks to approx or greater N
pthread_mutex_t locks[4194304];
int u = -1;


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





int initialize_single_source(int* D, int* Q, int source, int N);
void relax(int u, int i, volatile int* D, int** W, int** W_index, int N);
int get_local_min(volatile int* Q, volatile int* D, int start, int stop, int N, int** W_index, int** W, int u);

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
  pthread_barrier_t* barrier;
} thread_arg_t;

int local_min_buffer[1024];
int global_min_buffer;
int terminate = 0;
int range=1;
int old_range =1;
int difference=0;
int pid=0;
int *test;
int *id;
int P_max=8;
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
  int** W                  = arg->W;
  int** W_index            = arg->W_index;
  const int N              = arg->N;
  const int DEG            = arg->DEG;
  int local_count          = N;
  int i, j, po;
	int uu = 0;

  int cntr = 0;
  int i_start =  0;  //tid    * DEG / (arg->P);
  int i_stop  = 0;   //(tid+1) * DEG / (arg->P);
	int start = 0;
	int stop = 1;
	int neighbor=0;

	pthread_barrier_wait(arg->barrier);

while(terminate==0){
  while(terminate==0)
  {
	  for(uu=start;uu<stop;uu++)
		{

		if(test[uu]==0)
			continue;

    for(int i = 0; i < DEG; i++)
    {
			if(uu<N)
        neighbor = W_index[uu][i];

			if(neighbor>=N)
				break;
			
			pthread_mutex_lock(&locks[neighbor]);

			if(uu>=N)
				terminate=1;

			if((D[W_index[uu][i]] > (D[uu] + W[uu][i])))
				D[W_index[uu][i]] = D[uu] + W[uu][i];
      //Q[uu]=0;
	  
			pthread_mutex_unlock(&locks[neighbor]);
    }
		}

   pthread_barrier_wait(arg->barrier);
		
	 if(tid==0)
		{
			//range heuristic here
			 old_range=range;
		   range = range*DEG; //change this for range heuristic e.g. range = range+DEG;
       
			 if(old_range==1)
				 old_range=0;
       
			 if(range>=N)
				 range=N;
			
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

		pthread_barrier_wait(arg->barrier);
	
		//start = old_range  +  (difference/P)*(tid);            //(tid    * range)  / (arg->P)    + old_range;
		//stop  = old_range  +  (difference/P)*(tid+1);            //((tid+1) * range)  / (arg->P)   + old_range;
	  start = tid * (range/P);
		stop = (tid+1) * (range/P);

		if(stop>range)
		 stop=range;	

		//{ pthread_mutex_lock(&lock);
       if(start==N || uu>N-1)
				 terminate=1;
		//} pthread_mutex_unlock(&lock);
    
    pthread_barrier_wait(arg->barrier);
		
		//printf("\n TID:%d   start:%d stop:%d terminate:%d",tid,start,stop,terminate);
	}
	pthread_barrier_wait(arg->barrier);
	if(tid==0)
	{
		cntr++;
		if(cntr<P_max)
		{
			terminate=0;
			old_range=1;
			range=1;
			difference=0;
			pid=0;
		}
	}
	start=0;
	stop=1;
	pthread_barrier_wait(arg->barrier);
}

  return NULL;
}


int main(int argc, char** argv)
{
  // Start the simulator
  //CarbonStartSim(argc, argv);
	
	char filename[100];
	printf("Please Enter The Name Of The File You Would Like To Fetch\n");
	scanf("%s", filename);
	FILE *file0 = fopen(filename,"r");

	int lines_to_check=0;
	char c;
	int number0;
	int number1;
	int starting_node = 0;
	int previous_node = 0;
	int check = 0;
	int inter = -1;
	int N = 4194304; //can be read from file if needed, this is a default upper limit
	int DEG = 8;     //also can be reda from file if needed, upper limit here again

  const int P = atoi(argv[1]);
        
	if (DEG > N)
  {
                fprintf(stderr, "Degree of graph cannot be grater than number of Vertices\n");
                exit(EXIT_FAILURE);
  }

  int* D;
  int* Q;
  
	posix_memalign((void**) &D, 64, N * sizeof(int));
  posix_memalign((void**) &Q, 64, N * sizeof(int));
	posix_memalign((void**) &test, 64, N * sizeof(int));
	posix_memalign((void**) &id, 64, N * sizeof(int));
  int d_count = N;
  pthread_barrier_t barrier;

  int** W = (int**) malloc(N*sizeof(int*));
  int** W_index = (int**) malloc(N*sizeof(int*));
  for(int i = 0; i < N; i++)
  {
    int ret = posix_memalign((void**) &W[i], 64, DEG*sizeof(int));
    int re1 = posix_memalign((void**) &W_index[i], 64, DEG*sizeof(int));
    if (ret != 0 || re1!=0)
    {
       fprintf(stderr, "Could not allocate memory\n");
       exit(EXIT_FAILURE);
    }
  }

	for(int i=0;i<N;i++)
	{
		for(int j=0;j<DEG;j++)
		{
			W[i][j] = INT_MAX;
			W_index[i][j] = INT_MAX;
		}
		test[i]=0;
		id[0] = 0;
	}


for(c=getc(file0); c!=EOF; c=getc(file0))
  {
    if(c=='\n')
      lines_to_check++;

    if(lines_to_check>3)
    {   
      fscanf(file0, "%d %d", &number0,&number1);
      //printf("\n%d %d",number0,number1);

      test[number0] = 1; 
			id[number0] = number0;
      if(number0==previous_node)
			{
			  inter++;
			}
			else
				inter=0;

			W[number0][inter] = inter;
			W_index[number0][inter] = number1;
			previous_node = number0;
    }   
  }


  //init_weights(N, DEG, W, W_index);
  /*for(int i = 0;i<N;i++)
  {
        for(int j = 0;j<DEG;j++)
        {
                printf(" %d ",W[i][j]);
        }
        printf("\n");
  }*/

  pthread_barrier_init(&barrier, NULL, P);
  pthread_mutex_init(&lock, NULL);
	for(int i=0; i<N; i++)
		pthread_mutex_init(&locks[i], NULL);
  
	initialize_single_source(D, Q, 0, N);

  for(int j = 0; j < P; j++) {
    thread_arg[j].local_min  = local_min_buffer;
    thread_arg[j].global_min = &global_min_buffer;
    thread_arg[j].Q          = Q;
    thread_arg[j].D          = D;
    thread_arg[j].W          = W;
    thread_arg[j].W_index    = W_index;
    thread_arg[j].d_count    = &d_count;
    thread_arg[j].tid        = j;
    thread_arg[j].P          = P;
    thread_arg[j].N          = N;
    thread_arg[j].DEG        = DEG;
    thread_arg[j].barrier    = &barrier;
  }

  // Enable performance and energy models
  //CarbonEnableModels();

  //for clock time
  struct timespec requestStart, requestEnd;
  clock_gettime(CLOCK_REALTIME, &requestStart);

  //create threads
  for(int j = 1; j < P; j++) {
    pthread_create(thread_handle+j,
                   NULL,
                   do_work,
                   (void*)&thread_arg[j]);
  }
  do_work((void*) &thread_arg[0]);

	//join threads
  for(int j = 1; j < P; j++) { //mul = mul*2;
    pthread_join(thread_handle[j],NULL);
  }
  
	//read clock for time
	clock_gettime(CLOCK_REALTIME, &requestEnd);
	  double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
		  printf( "%lf\n", accum );

  // Enable performance and energy models
  //CarbonDisableModels();
  //printf("\ndistance:%d \n",D[N-1]);

	//for distance values check
    for(int i = 0; i < 2097152; i++) 
		{
			if(test[i]==1)
        printf(" %d ", D[i]);
    }
    printf("\n");

  // Stop the simulator
  //CarbonStopSim();
  return 0;
}

int initialize_single_source(int*  D,
                             int*  Q,
                             int   source,
                             int   N)
{
  for(int i = 0; i < N+1; i++)
  {
		D[i] = INT_MAX;
    D[i] = i;
    Q[i] = 1;
  }

  D[source] = 0;
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

void relax(int u, int i, volatile int* D, int** W, int** W_index, int N)
{
  if((D[W_index[u][i]] > (D[u] + W[u][i]) && (W_index[u][i]!=-1 && W_index[u][i]<N && W[u][i]!=INT_MAX)))
    D[W_index[u][i]] = D[u] + W[u][i];
}
