#include "mpi.h"
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <time.h>

#define ARRAY_SIZE 1000000 

int main (int argc,  char *argv[]) {

    srand(time(0));

   int myid, numprocs;
   int namelen;
   int* numbers = new int[ARRAY_SIZE];
   char processor_name[MPI_MAX_PROCESSOR_NAME];

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);
   MPI_Get_processor_name(processor_name, &namelen);
 
   printf("Process %d on %s\n", myid, processor_name);
 
   for (int i=0; i<ARRAY_SIZE; i++)
      numbers[i] = rand();  //Assign randomly generated value

   int s = (int)floor(ARRAY_SIZE/numprocs);
   int s0 = s + ARRAY_SIZE%numprocs;

   int startIndex = s0 + (myid-1)*s;
   int endIndex = startIndex + s;

   double startwtime;


   if (myid == 0) {
      startwtime = MPI_Wtime();
   }

   int i;
   int part_sum = 0;

   int currentMax = 0; // Smallest value returnable by RAND()
   int currentMin = 2147483647; // This is RAND_MAX
   
   
   if (myid == 0) {
      // master worker - comput the master's numbers

      

      for (i=0; i<s0; i++) {

         if (numbers[i] > currentMax)
         {
            currentMax = numbers[i];
         }

         if (numbers[i] < currentMin)
         {
            currentMin = numbers[i];
         }

         
      }
      printf("Process %d - startIndex 0 endIndex %d; current minimum int %ld; current max int %ld\n",
             myid, s0-1, currentMin, currentMax);
   } else {
      //slave's work

      //int currentMax = 0; // Smallest value returnable by RAND()
      //int currentMin = 2147483647; // This is RAND_MAX

      for (i= startIndex; i<endIndex; i++) {
         
         if (numbers[i] > currentMax)
         {
            currentMax = numbers[i];
         }

         if (numbers[i] < currentMin)
         {
            currentMin = numbers[i];
         }


      }

      printf("Process %d - startIndex 0 endIndex %d; current minimum int %ld; current max int %ld\n",
             myid, s0-1, currentMin, currentMax);
   }

   int receivedMax = 0;
   MPI_Reduce(&currentMax, &receivedMax, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

   int receivedMin = 0;
   MPI_Reduce(&currentMin, &receivedMin, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
   

   if (receivedMax > currentMax)
   {
      currentMax = receivedMax;
   }

   if (receivedMin < currentMin)
   {
      currentMin = receivedMin;
   }

   if (myid == 0) {
      double runTime = MPI_Wtime() - startwtime;
      printf("Execution time (sec) = %f Minimum = %ld Maximum = %ld\n",
             runTime, currentMin, currentMax);
   }

   delete[] numbers;

   MPI_Finalize();
}

