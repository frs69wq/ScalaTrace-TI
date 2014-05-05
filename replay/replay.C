/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "replay.h"
#include "Trace.h"
#include "Event.h"
#include "mpi.h"
#include <sys/time.h>
#include "op_exec.h"
#include "context.h"
#include "histo_exec.h"
#include <time.h>

#define INPUT_GLOBAL	0
#define INPUT_NODE	1

using namespace std;

int my_rank, my_size;
int doTestFor = 0;

int main(int argc, char **argv){
  char filename[4096];
  Trace* trace;
  int whole, frac;
  struct timeval t_start, t_stop;
  struct timezone tz;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &my_size);

  if(argc < 2){
    cerr << "usage:" << argv[0] << " <trace>" << endl;
    MPI_Finalize ();
    return 1;
  }

#ifdef DEBUG
  int dw = 1;
  int waiting = -1;
  if(argc == 3)
    waiting = atoi(argv[2]);
  if(my_rank == waiting){
    cout << "DEBUG: rank " << my_rank << " waiting..." << endl;
    while(dw);
  }
  MPI_Barrier(MPI_COMM_WORLD);
#endif

  sprintf(filename, "./%s", argv[1]);
#ifdef FEATURE_HISTO_REPLAY
  trace = Trace::inputTrace(filename, INPUT_GLOBAL, my_rank);
  unsigned int seed = time(NULL);
  //seed = 0;
  if(my_rank == doTestFor){
    cout << "Replay size: " << my_size << endl;
    cout << "# of Random Destinations: " << FEATURE_RAND_DEST << endl;
    cout << "Rank " << my_rank << ": seed = " << seed << endl;
  }
  //unsigned int seed = 0;
  PMPI_Bcast(&seed, 1, MPI_INT, 0, MPI_COMM_WORLD);
  setSeed(seed);
  setDestLimit(FEATURE_RAND_DEST);
  setupCommunicators(trace);
  setupDestSet(trace);
  notifyPeers();
  setupPeerContexts(trace);

#else
  trace = Trace::inputTrace(filename, INPUT_NODE, my_rank);
#endif

  PMPI_Barrier(MPI_COMM_WORLD);
  gettimeofday(&t_start, &tz);

#ifdef FEATURE_HISTO_REPLAY
  if(my_rank == 0)
    cout << "Histo replay" << endl;
  histo_replay(trace);
#else
  if(my_rank == 0)
    cout << "Normal replay" << endl;
  trace->traverse(replay_event);
#endif

  PMPI_Barrier(MPI_COMM_WORLD);
  gettimeofday(&t_stop, &tz);


  whole = t_stop.tv_sec - t_start.tv_sec;
  frac = t_stop.tv_usec - t_start.tv_usec;
  if(frac < 0) {
    frac += 1000000;
    whole --;
  }
  if(my_rank == 0){
    cout << "Benchmark:" << filename << "\nSize: " << my_size << endl;
    cout << "Execution time: " << whole << "." << frac << endl << endl;
  }

#ifdef FEATURE_HISTO_REPLAY
  destroyCommunicators();
  destroyPeerContexts();
#endif
  MPI_Finalize();
  return 0;
}
