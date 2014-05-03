/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef __HISTO_EXEC_H__
#define __HISTO_EXEC_H__

#include <set>
#include <map>
#include <list>
#include <queue>
#include "Trace.h"
#include "Event.h"
#include "context.h"
#include "umpi_mpi_ops.h"
#include <stdlib.h>
#include "PtrHandler.h"
#include "op_exec.h"
#include "ReqHandler.h"
#include "mem_handler.h"
#include "handle_queue.h"
#include <climits>
#include "Timer.h"

#define DECODE_DEST_FOR_RANK(d,r) ((d + r) % my_size)

using namespace std;
class CompareSendinfo;

typedef struct {
  long long int time;
  Event *e;
  int count;
  int datatype;
  int src;
  int tag;
  int comm;
} sendinfo_t;

extern int my_rank, my_size;
extern ptr_handler_t replay_ptr;
extern req_handler_t replay_req;

void setSeed(unsigned int _seed);
void setDestLimit(int _limit);
int translateRank(int rank, int in_comm, int out_comm = -1);

void setupCommunicators(Trace* trace);
void destroyCommunicators();
void setupDestSet(Trace* trace);
void notifyPeers();
void setupPeerContexts(Trace* trace);
void destroyPeerContexts();

void histo_replay(Trace* trace);
sendinfo_t* getNextSendForRank(int rank);

/**
 * compares times such that the earliest send is at 
 * the top of the priority_queue
 */
class CompareSendinfo{
public:
  bool operator()(sendinfo_t* s1, sendinfo_t* s2){
    if(s1->time < s2->time)
      return false;
    else
      return true;
  }
};

#endif
