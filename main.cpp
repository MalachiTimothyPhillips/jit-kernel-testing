// run on 8 or fewer ranks

#include <stdlib.h>
#include <iostream>

#include <mpi.h>

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  
  int commSize;
  MPI_Comm_size(MPI_COMM_WORLD, &commSize);
  
  int commRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &commRank);

  //setenv("TMPDIR","/home/malachi2/develop/polaris-issue/tmp", 1);
  
  char cmd[BUFSIZ];

  // alter arch accordingly
  sprintf(cmd, "nvcc -O3 --fmad=true -lineinfo -arch=sm_80 -fatbin -Xptxas -v -x cu hello-%d.cu -o hello-%d.bin", commRank, commRank);
  int retVal = system(cmd);

  MPI_Allreduce(MPI_IN_PLACE, &retVal, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

  if(retVal){
    if(commRank == 0){
      printf("Error in JIT compiling kernels.\n");
    }
    fflush(stdout);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  
  MPI_Finalize();
}
