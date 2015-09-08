#include <cstdio>
#include <cstdlib>
#include <pthread.h>
//#include "carbon_user.h"
#include <time.h>
#include <sys/timeb.h>
#include <string.h>

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


  void init_weights(int N, int DEG, double** W, int** W_index)
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
                                  int neighbor = rand()%(i+max*2);
																	W_index[i][j] = neighbor;
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
                                  W[i][j] = (double) (v) + 1;
                          //printf("   %d  ",W_index[i][j]);
                  }
                  //printf("\n");
          }
  }





int initialize_single_source(double* D, int* Q, int source, int N, double initial_rank);
void relax(int u, int i, volatile int* D, int** W, int** W_index, int N);
int get_local_min(volatile int* Q, volatile int* D, int start, int stop, int N, int** W_index, int** W, int u);

typedef struct
{
  int*      local_min;
  int*      global_min;
  int*      Q;
  double*      D;
  double**     W;
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
//int iterations=64;
int *test;
int *test1;
int *test2;
int *test3;
int *outlinks;
double dp = 0;
double *pgtmp;
int nodecount = 0;
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
  double* D                   = arg->D;
  double** W                  = arg->W;
  int** W_index            = arg->W_index;
  const int N              = arg->N;
  const int DEG            = arg->DEG;
  int local_count          = N;
  int i, j, po;
	int uu = 0;
	double r = 0.15;
  double d = 0.85;	
	double DEGREE = DEG;
	double sum = 0;
	double N_real = N;

  int cntr = 0;
  int i_start =  tid     * N / (arg->P);
  int i_stop  =  (tid+1) * N / (arg->P);
	int start = 0;
	int stop = 1;
	double PR = 0;
	int iterations = 15;
  //int difference =0;
  printf("\n Start");

	pthread_barrier_wait(arg->barrier);

	while(iterations>0)
	{

		if(tid==0)
			dp=0;
		pthread_barrier_wait(arg->barrier);

	//for no outlinks
	for(uu=i_start;uu<i_stop;uu++)
	{
		if(test3[uu]==1)
		{
      pthread_mutex_lock(&lock);
       dp = dp + d*(D[uu]/N_real);
				//printf("\n %f %f %f %f",dp,d,D[uu],N_real);
		  pthread_mutex_unlock(&lock);
		}
	}
	//printf("\n Outlinks Done %f",dp);

	pthread_barrier_wait(arg->barrier);

	uu=0;

	for(uu=i_start;uu<i_stop;uu++)
	{
		if(test1[uu]==1)
		{
    pgtmp[uu] = r;//dp + (r)/N_real;
		//printf("\n pgtmp:%f test:%d",pgtmp[uu],test[uu]);
		for(int j=0;j<test[uu];j++)
		{
      //if inlink
			//printf("\nuu:%d id:%d",uu,W_index[uu][j]);
			pgtmp[uu] = pgtmp[uu] + (d*D[W_index[uu][j]]/outlinks[W_index[uu][j]]);
		}
		}
	}
  //printf("\n Ranks done");
	
	pthread_barrier_wait(arg->barrier);

	for(uu=i_start;uu<i_stop;uu++)
	{
		D[uu] = pgtmp[uu];
	}

	pthread_barrier_wait(arg->barrier);
	iterations--;
	}

  //printf("\n %d %d",tid,terminate);
  return NULL;
}


int main(int argc, char** argv)
{ //int mul = W_index[0][0];
  // Start the simulator
  //CarbonStartSim(argc, argv);

	char filename[100];
	//printf("Please Enter The Name Of The File You Would Like To Fetch\n");
	//scanf("%s", filename);
	strcpy(filename,argv[2]);
	//filename = argv[2];
	FILE *f = fopen(filename,"r");

  int lines_to_check=0;
	char c;
	int number0;
	int number1;
	int starting_node = 0;
	int previous_node = 0;
	int check = 0;
	int inter = -1;
	int N = 2000000; //4194304; //can be read from file if needed, this is a default upper limit
	int DEG = 12;     //also can be reda from file if needed, upper limit here again

  const int P = atoi(argv[1]);

        if (DEG > N)
        {
                fprintf(stderr, "Degree of graph cannot be grater than number of Vertices\n");
                exit(EXIT_FAILURE);
        }

  double* D;
  int* Q;
  posix_memalign((void**) &D, 64, N * sizeof(double));
  posix_memalign((void**) &Q, 64, N * sizeof(int));
  posix_memalign((void**) &test, 64, N * sizeof(int));
  posix_memalign((void**) &test1, 64, N * sizeof(int));
	posix_memalign((void**) &test2, 64, N * sizeof(int));
	posix_memalign((void**) &test3, 64, N * sizeof(int));
	posix_memalign((void**) &pgtmp, 64, N * sizeof(double));
	posix_memalign((void**) &outlinks, 64, N * sizeof(int));
	int d_count = N;
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

	printf("\nRead");
  for(int i=0;i<N;i++)
	{
		for(int j=0;j<DEG;j++)
		{
			W[i][j] = 1000000000;
			W_index[i][j] = INT_MAX;
		}
		test[i]=0;
		test1[i]=0;
		test2[i]=0;
		test3[i]=0;
		outlinks[i]=0;
	}

	int lines=0;
	for(c=getc(f); c!=EOF; c=getc(f))
	{
		if(c=='\n')
			lines++;
	}
	fclose(f);

	FILE *file0 = fopen(filename,"r");

  for(c=getc(file0); c!=EOF; c=getc(file0))
  {
    if(c=='\n')
      lines_to_check++;

    if(lines_to_check>3 && lines_to_check<lines)
    {   
      fscanf(file0, "%d %d", &number0,&number1);

      inter = test[number1]; 

			W[number0][inter] = 0; //drand48();
			W_index[number1][inter] = number0;
			previous_node = number0;
			test[number1]++;
			outlinks[number0]++;
			test1[number0]=1; test1[number1]=1;
			test2[number0]=1;
			test3[number1]=1;
			if(number0 > nodecount)
				nodecount = number0;
			if(number1 > nodecount)
				nodecount = number1;
    }   
  }
	nodecount++;
	for(int i=0;i<N;i++)
	{ 
		if(test2[i]==1 && test3[i]==1)
			test3[i]=0;
	}
	//printf("\n\nFile Read %d",lines_to_check);

  // Calculate total nodes, in order to calculate an initial weight.
  /*for(int i=0;i<N;i++) 
	{
    if (test1[i]==1) 
			nodecount++;
  }*/
	printf("Nodes: %d",nodecount);
  N = nodecount;

  pthread_barrier_init(&barrier, NULL, P);
  pthread_mutex_init(&lock, NULL);
	for(int i=0; i<N; i++)
		pthread_mutex_init(&locks[i], NULL);
  
	//double nodecount_f = nodecount;
	initialize_single_source(D, Q, 0, N, 0.15);
  printf("\nInit");
  
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

  for(int j = 1; j < P; j++) { //mul = mul*2;
    pthread_join(thread_handle[j],NULL);
  }

	clock_gettime(CLOCK_REALTIME, &requestEnd);
	  double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
		  printf( "%lf\n", accum );

  // Enable performance and energy models
  //CarbonDisableModels();
  //printf("\ndistance:%d \n",D[N-1]);

			FILE *f1 = fopen("file.txt", "w");

    for(int i = 0; i < N; i++) {
			if(test1[i]==1)
      fprintf(f1,"pr(%d) = %f\n", i,D[i]);
    }
    printf("\n");
		fclose(f1);


  // Stop the simulator
  //CarbonStopSim();
  return 0;
}

int initialize_single_source(double*  D,
                             int*  Q,
                             int   source,
                             int   N,
                             double initial_rank)
{
  for(int i = 0; i < N; i++)
  {
    D[i] = 0.15;//initial_rank;
		pgtmp[i] = 0.15;//initial_rank;
    Q[i] = 0;
  }

  //  D[source] = 0;
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
