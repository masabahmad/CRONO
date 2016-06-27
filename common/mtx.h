#include <cstdio>
#include <cstdlib>
#include <getopt.h>

#define MAX            100000000
#define INT_MAX        100000000
#define BILLION 1E9

//Global Variables
int **W;          //Weights
int **W_index;    //Edges
//int *D;           //Working Array
//int *Q;           //Working Array
int *edges;       //Edges for each vertex
int *exist;       //Wherther the vertex exists in the graph
int *unique;      //Unique vertex 
int largest=0;    //Largest Vertex in graph
int degree=0;     //Initialize Maximum Degree
const char *conv_file;

void mtx(const char* filename)
{
      FILE *file_mtx_param = NULL;
      FILE *file_mtx_deg = NULL;
      FILE *file_mtx = NULL;
      //const char *filename = argv[3];
      file_mtx_param = fopen(filename,"r");
      int number_of_lines=0;
      int node_count =0; 
      int intermediate=0;
      int edge_count =0; 
      int first,second;
      char ch; 
      while (EOF != (ch=getc(file_mtx_param)))
      {   
     
        if(ch=='\n')  
          number_of_lines++;
        if(number_of_lines==2)
        {   
          int f0 = fscanf(file_mtx_param, "%d %d %d", &node_count,&intermediate,&edge_count);
          if(f0 != 3 && f0 != EOF)
          {   
            printf ("Error: Read %d values, expected 2. Parsing failed.\n",f0);
            exit (EXIT_FAILURE);
          }   
          break;
        }   
      }   
      number_of_lines=0;
      fclose(file_mtx_param);
      largest = node_count;
      printf("\nMatrix .mtx graph with Parameters: Vertices:%d Edges:%d",node_count,edge_count);

   if(posix_memalign((void**) &edges, 64, node_count * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &exist, 64, node_count * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   for(int i=0;i<node_count;i++)
   {
     edges[i]=0; exist[i] = 0;
   }

      file_mtx_deg = fopen(filename,"r");
      while (EOF != (ch=getc(file_mtx_deg)))
      {
        if(ch=='\n')
          number_of_lines++;
        if(number_of_lines>2)
        {
          int f0 = fscanf(file_mtx_deg, "%d %d", &first,&second);
          if(f0 != 2 && f0 != EOF)
            exit (EXIT_FAILURE);
          edges[second-1]++;
          exist[second-1]=1; exist[first-1]=1;
        }
      }
      fclose(file_mtx_deg);

      degree=edges[0];
      for(int i=1;i<node_count;i++)
      {
        if(edges[i]>degree)
          degree = edges[i];
      }
      printf("\nLargest Degree:%d\n",degree);
      number_of_lines = 0;
      ch = 'a';

   /*if(posix_memalign((void**) &D, 64, node_count * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }
   if(posix_memalign((void**) &Q, 64, node_count * sizeof(int)))
   {
      fprintf(stderr, "Allocation of memory failed\n");
      exit(EXIT_FAILURE);
   }*/
   printf("\nAllocated Working Arrays");

   //W = (int**) malloc(node_count*sizeof(int*));
   W_index = (int**) malloc(node_count*sizeof(int*));
   for(int i = 0; i < node_count; i++)
   {
      //W[i] = (int *)malloc(sizeof(int)*N);
      /*if(posix_memalign((void**) &W[i], 64, degree*sizeof(int)))
      {
         fprintf(stderr, "Allocation of memory failed\n");
         exit(EXIT_FAILURE);
      }*/
      if(posix_memalign((void**) &W_index[i], 64, degree*sizeof(int)))
      {
         fprintf(stderr, "Allocation of memory failed\n");
         exit(EXIT_FAILURE);
      }
   }
   printf("\nAllocated Weight/Index Arrays");

   for(int i=0;i<node_count;i++)
   {
      for(int j=0;j<edges[i];j++)
      {
         //W[i][j] = 1000000000;
         W_index[i][j] = INT_MAX;
      }
   }
   for(int i=0;i<node_count;i++)
   {
     edges[i]=0;
   }
   printf("\nInitialized Arrays");


      file_mtx = fopen(filename,"r");
      int inter=0;
      while (EOF != (ch=getc(file_mtx)))
      {
        if(ch=='\n')
          number_of_lines++;
        //if(number_of_lines > edge_count-100)
        //  break;
        if(number_of_lines>2)
        {
          int f0 = fscanf(file_mtx, "%d %d", &first,&second);
          if(f0 != 2 && f0 != EOF)
            exit (EXIT_FAILURE);
          //if(number_of_lines >= 132500000)
          //  printf("\n %d %d %d",first-1,second-1,number_of_lines);
          if((first-1 || second-1) > node_count)
            continue;
          inter = edges[second-1];
          W_index[second-1][inter] = first-1;
          edges[second-1]++;
        }
      }
      fclose(file_mtx);
      printf("\nDone Allocating and Initializing");
     
		  //Create .gr file format from .mtx	
			FILE *file_gr = NULL;
      conv_file = "rgg.gr"; 
			file_gr = fopen(conv_file,"w");
      fprintf(file_gr,"#.\n");
      fprintf(file_gr,"#.\n");
      fprintf(file_gr,"#.\n");
			fprintf(file_gr,"p sp %d %d\n",node_count,edge_count);
			for(int i=0;i<node_count;i++)
			{
        for(int j=0;j<edges[i];j++)
				{
					fprintf(file_gr,"%d %d\n",i,W_index[i][j]); //a  +1 +1
				}
			}
			fclose(file_gr);
      printf("\n Done Writing to .gr file");
}
