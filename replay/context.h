/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "Event.h"
#include "Loop.h"
#include <map>
#include "RandMnger.h"
#include <climits>
#include "Timer.h"

#define MEMBERSHIP_TRACKING_SIZE 1000

using namespace std;

typedef struct {
  Event *loophead;
  Event *looptail;
  loop_t* loopitem;
  int iteration;
} loopcontext_t;

extern int my_rank;

class Context{

private:
  /* for trace traversal */
  int rank;
  Event *iter;
  int depth;
  vector<loopcontext_t *> lps;
  void buildLoopcontext(Event *e);
  map<int, bool> membership;

  /* for random number generation */
  RandMnger* randmnger;

  /* for timing */
  Timer *timer;

  bool hasMember(Event *e, int rank);
  Event* next_helper();

public:
  Context(Event *head, Event* tail, int _rank);
  Context(Event *head, Event* tail, int _rank, unsigned int _seed, int _limit = INT_MAX, compute_t *_compute = NULL);
  ~Context();

  Event* next();
  bool hasNext(){
    return iter != NULL;
  }

  RandMnger* getRandMnger(){
    return randmnger;
  }

  Timer* getTimer(){
    return timer;
  }
};

#endif /*__CONTEXT_H__*/
