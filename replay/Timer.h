/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */
#ifndef __TIMER_H__
#define __TIMER_H__

#include "Event.h"
#include <sys/time.h>
#include <climits>

typedef void* (*func_t)(void* );
typedef struct {
  func_t func;
  void *param;
} compute_t;

using namespace std;

class Timer{

private:
  int leftover;
  long long int timestamp;
  Event *cur_event;
  Event *prev_event;
  long long int accumulated;
  compute_t* compute;

  long long int getCurrentTime();
  int busyWait(int sleep_duration);

public:
  Timer();
  Timer(compute_t* _compute);
  ~Timer();

  void setCurrentEvent(Event *op);
  void setTime();
  void resetTime();
  void maxTime();
  long long int getTime();
  int getCompTime();
  int getNumInst();
  int getCommTime();
  void simulateComputeTime(bool doComm = false, compute_t *_exec = NULL);

  void accumulateCompTime(){
    accumulated += getCompTime();
  }
  void accumulateCommTime(){
    accumulated += getCommTime();
  }
  long long int getAccumulatedTime(){
    return accumulated;
  }

};

#endif /* __TIMER_H__ */
