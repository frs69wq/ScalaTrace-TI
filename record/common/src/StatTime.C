/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "StatTime.h"

struct timeval StatTime::timestamp;

StatTime::StatTime(int s):
    Stat(s)
{}

void StatTime::start(){
  gettimeofday(&(StatTime::timestamp), NULL);
}

long long StatTime::end(){
  struct timeval now;
  gettimeofday(&now, NULL);
  long long whole = now.tv_sec - (StatTime::timestamp).tv_sec;
  long long frac = now.tv_usec - (StatTime::timestamp).tv_usec;
  return whole * 1000000 + frac;
}
