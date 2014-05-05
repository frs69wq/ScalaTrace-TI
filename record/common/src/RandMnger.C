/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include <iostream>      // Added by Henri so that smpicxx can compile this

#include "RandMnger.h"

RandMnger::RandMnger(){}

RandMnger::RandMnger(unsigned int _seed, int _limit):
    seed(_seed),
    limit(_limit)
{}

RandMnger::~RandMnger(){}

double RandMnger::getRandomValue(Histogram *h){
  if(!h){
    cerr << "error: null histogram" << endl;
    exit(0);
  }

  int r = rand();
  set<double>::iterator it;

  if(seenValues.size() >= (unsigned int)limit){
    return valuelist[ r%limit ];
  }

  double rtn = h->randomValue(r);
  it = seenValues.find(rtn);
  if(it == seenValues.end()){
    seenValues.insert(rtn);
    valuelist.push_back(rtn);
  }

  return rtn;
}

