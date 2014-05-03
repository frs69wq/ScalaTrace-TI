/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "context.h"

Context::Context(Event *head, Event *tail, int _rank):
rank(_rank),
iter(head),
depth(0)
{
  loopcontext_t *lp = new loopcontext_t;
  lp->loophead = head;
  lp->looptail = tail;
  lp->loopitem = NULL;
  lp->iteration = 1;
  lps.push_back(lp);

  randmnger = new RandMnger(0);
  timer = new Timer();
}

Context::Context(Event *head, Event *tail, int _rank, unsigned int _seed, int _limit, compute_t *_compute):
    rank(_rank),
    iter(head),
    depth(0)
{
  loopcontext_t *lp = new loopcontext_t;
  lp->loophead = head;
  lp->looptail = tail;
  lp->loopitem = NULL;
  lp->iteration = 1;
  lps.push_back(lp);

  randmnger = new RandMnger(_seed, _limit);
  timer = new Timer(_compute);
}

Context::~Context(){
  for(size_t i=0; i<lps.size(); i++)
    if(lps[i])
      delete lps[i];

  if(randmnger)
    delete randmnger;

  if(timer)
    delete timer;
}

bool Context::hasMember(Event *e, int rank){
  int id = e->getId();
  map<int, bool>::iterator it = membership.find(id);
  if(it != membership.end() ){
    return it->second;
  } else {
    bool rtn = e->hasMember(rank);
    /* limit the size of the map */
    if(membership.size() > MEMBERSHIP_TRACKING_SIZE){
      membership.clear();
    }
    membership[id] = rtn;
    return rtn;
  }
}

void Context::buildLoopcontext(Event *e){
  if(e->getLoopDepth() == 0)
    return;

  vector<loop_t *> *loopstack = e->getLoopStack()->getLoops();
  bool found = false;
  size_t i;
  for(i=0; i<loopstack->size(); i++){
    if(found == false && loopstack->at(i) == lps[depth]->loopitem){
      found = true;
      i++;
      break;
    }
  }

  if(found == false){
    i=0;
  }
  int mem, iters;
  Event *it;
  for(; i<loopstack->size(); i++){
    loopcontext_t *lp = new loopcontext_t;
    lp->loophead = e;

    mem = e->getLoopStack()->getMemForRank(i, rank);
    iters = e->getLoopStack()->getIterForRank(i, rank);
    //if(mem == 0 || iters == 0){
    if(mem == 0){
      delete lp;
      break;
    }
    it = e;
    for(int j=0; j<mem-1; j++)
      for(it = it->next; !hasMember(it, rank); it = it->next);
    lp->looptail = it;
    lp->loopitem = loopstack->at(i);
    lp->iteration = iters;
    lps.push_back(lp);
    depth++;
  }
}

Event* Context::next(){
  Event* rtn;
  while( true ){
    rtn = next_helper();
    if(rtn != NULL || (rtn == NULL && depth == -1))
      break;
  }
  return rtn;
}

Event* Context::next_helper(){
  if(!iter)
    return NULL;

  Event* current = iter;
  loopcontext_t *lp = lps[depth];

  if(lp->iteration <= 0){
    assert(iter == lp->looptail);
    current = NULL;
  }

  if(iter == lp->looptail){
    lp->iteration--;
    if(lp->iteration <= 0){
      do {
        lps.pop_back();
        depth--;
        if(depth == -1){
          iter = NULL;
          break;
        }
        lp = lps[depth];
        if(iter == lp->looptail){
          lp->iteration--;
          if(lp->iteration != 0){
            iter = lp->loophead;
            buildLoopcontext(iter);
            break;
          }
        } else {
          for(iter = iter->next; iter && !hasMember(iter, rank); iter = iter->next);
          if(iter && iter->getLoopDepth() > 0){
            buildLoopcontext(iter);
          }
          break;
        }
      } while (true);
    } else {
      iter = lp->loophead;
      buildLoopcontext(iter);
    }

  } else {
    for(iter = iter->next; iter && !hasMember(iter, rank); iter = iter->next);
    if(iter && iter->getLoopDepth() > 0){
      buildLoopcontext(iter);
    }
  }

  return current;
}
