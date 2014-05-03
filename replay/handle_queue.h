/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */
#ifndef __HANDLE_QUEUE_H__
#define __HANDLE_QUEUE_H__

#include <vector>
#include <assert.h>
#include <stdlib.h>
#include <mpi.h>

#define BATCH_SIZE 50
#define CHUNK_SIZE (1 << 28)

using namespace std;

extern int my_rank;
extern int my_size;
extern int doTestFor;

class HandleQueue{
private:
  vector<MPI_Request> requests;
  vector<void *> buffers;
  long long int sbufsize;
  void *sendbuffer;
  long long int rbufsize;
  void *recvbuffer;

public:
  HandleQueue();
  ~HandleQueue();

  void testallCompletion();
  void testsomeCompletion();
  void testsingleCompletion();
  void testCompletion();
  void add(MPI_Request request, void *buf);

  void *getSendBuffer(int size);
  void *getRecvBuffer(int size);
};


#endif
