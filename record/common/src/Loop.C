/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "Loop.h"

Loop::Loop(){}

Loop::~Loop(){
  for(unsigned i = 0; i < loops.size(); i++){
    if(loops[i] != NULL){
      delete loops[i]->iter;
      delete loops[i]->mem;
      delete loops[i];
    }
  }
}

void Loop::insertLoop(int mem, int iter){
  extern int my_rank;
  loop_t *newLoop = new loop_t;
  newLoop->mem = new Param(LOOPMEM);
  newLoop->mem->initWithScalar(mem, my_rank);
  newLoop->iter = new Param(LOOPITER);
  newLoop->iter->initWithScalar(iter, my_rank);
  loops.insert(loops.begin(), newLoop);
}

void Loop::insertLoop(loop_t *loop){
  loops.insert(loops.begin(), loop);
}

loop_t* Loop::popOuterLoop(){
  loop_t* outer = loops.front();
  loops.front() = NULL;
  loops.erase(loops.begin());
  return outer;
}

vector<loop_t *> * Loop::getLoops(){
  return &loops;
}

loop_t * Loop::getLoopAt(int depth){
  if(depth >= (int)loops.size() || depth < 0)
    return NULL;
  return loops[depth];
}

int Loop::getDepth(){
  return loops.size();
}

int Loop::getMemForRank(int depth, int rank){
  if(depth < 0 || depth >= (int)loops.size())
    return 0;

  loop_t* lp = loops[depth];
  return lp->mem->getValueForRank(rank);
}

int Loop::getIterForRank(int depth, int rank){
  if(depth < 0 || depth >= (int)loops.size())
    return 0;

  loop_t* lp = loops[depth];
  return lp->iter->getValueForRank(rank);
}

void Loop::mergeLoopStack(Loop* other){
  int d1 = loops.size();
  int d2 = other->getDepth() ;
  assert(d1 >= d2);
  vector<loop_t *> *otherLoopStack = other->getLoops();
  for(int i = 0; i < d2 ; i++){
    loops[d1 - d2 + i]->mem->merge(otherLoopStack->at(i)->mem);
    loops[d1 - d2 + i]->iter->merge(otherLoopStack->at(i)->iter);
  }
}

void Loop::interMergeLoopStack(Loop* other){
  int l1 = loops.size();
  int l2 = other->getDepth();
  int len = l1 < l2 ? l1 : l2;
  int i;
  vector<loop_t *> *otherLoopStack = other->getLoops();
  for(i=0; i<len; i++){
    loops[i]->mem->merge(otherLoopStack->at(i)->mem);
    loops[i]->iter->merge(otherLoopStack->at(i)->iter);
  }
  for(; i < l2; i++){
    loops.push_back(otherLoopStack->at(i) );
    otherLoopStack->at(i) = NULL;
  }
}

void Loop::incLoopIter(int depth, int inc){
  assert(0<= depth && depth < (int)loops.size());
  /**
   * it's safe to assume loop[depth]->iter is a scalar value: if it were not,
   * there must be another level of loop at depth-1 because the loop at depth
   * already has multiple iterations (so that is not a scalar value). However,
   * since the depth of the target_iter (refer to Trace::updateLoopInfo() )
   * is at level depth+1, and Event::loopMatch ensures that the depths of
   * master_iter and slave_iter differ only by at most 1, it's impossible for
   * the master_iter to have a loop at level depth-1;
   */
  loops[depth]->iter->incValues(inc);
}

bool Loop::incLoopMem(int boundary){
  bool changeMade = false, found = false;
  vector<int>* mems;
  for(vector<loop_t *>::iterator it = loops.begin(); it != loops.end(); it++){
    found = false;
    mems = (*it)->mem->getValues();
    assert(mems);
    for(vector<int>::iterator mem_it = mems->begin(); mem_it != mems->end(); mem_it++){
      if((*mem_it) > boundary ){
        changeMade = true;
        found = true;
        (*mem_it)++;
      }
    }
    if(found == false)
      break;
  }
  return changeMade;
}

void Loop::cleanup(){
  int m, i;
  for(vector<loop_t *>::iterator it = loops.begin(); it != loops.end();){
    m = i = -1;
    if((*it)->mem->getNumValues(false) == 1){
      m = (*it)->mem->getScalarValueForRank();
    }
    if((*it)->iter->getNumValues(false) == 1){
      i = (*it)->iter->getScalarValueForRank();
    }
    if(m == 1 && i == 1){
      loops.erase(it);
    } else {
      it++;
    }
  }
}

void Loop::resetTraversalIterators(){
  vector<loop_t *>::iterator it;
  for(it = loops.begin(); it != loops.end(); it++){
    (*it)->mem->resetTraversalIterators();
    (*it)->iter->resetTraversalIterators();
  }
}

string Loop::toString(){
  ostringstream rtn;
  for(unsigned i=0; i<loops.size(); i++)
    rtn << "LOOP" << i << ": iter: " << loops[i]->iter->loopParamToString() << " mem: " << loops[i]->mem->loopParamToString() << "\n";
  return rtn.str();
}

void Loop::input(string& buf, int mode, int rank){
  string iter_str = buf.substr(0, buf.find("mem"));
  iter_str = iter_str.substr(iter_str.find(":")+1);
  Param *iter = new Param(LOOPITER);
  size_t begin = 0;
  size_t end = iter_str.find("]");
  string temp;
  while(end != string::npos){
    temp = iter_str.substr(begin, end - begin + 1);
    iter->input(temp, mode, rank);
    begin = end + 1;
    end = iter_str.find("]", end + 1);
  }

  string mem_str = buf.substr(buf.find("mem") + 4);
  Param *mem = new Param(LOOPMEM);
  begin = 0;
  end = mem_str.find("]");
  temp.clear();
  while(end != string::npos){
    temp = mem_str.substr(begin, end - begin + 1);
    mem->input(temp, mode, rank);
    begin = end + 1;
    end = mem_str.find("]", end + 1);
  }

  if(iter->getParamValues()->size() == 0 || mem->getParamValues()->size() == 0){
    delete iter;
    delete mem;
  } else {
    loop_t *l = new loop_t;
    l->mem = mem;
    l->iter = iter;
    loops.push_back(l);
  }
}

int Loop::getPackedSize(){
  int size = 0;
  size += sizeof(int); /* loops.size() */
  for(unsigned i=0; i<loops.size(); i++){
    size += loops[i]->mem->getPackedSize();
    size += loops[i]->iter->getPackedSize();
  }
  return size;
}


void Loop::pack(void *buf, int bufsize, int *position, MPI_Comm comm){
  int size = loops.size();
  PMPI_Pack(&size, 1, MPI_INT, buf, bufsize, position, comm);
  for(int i=0; i<size; i++){
    loops[i]->mem->pack(buf, bufsize, position, comm);
    loops[i]->iter->pack(buf, bufsize, position, comm);
  }
}

void Loop::unpack(void *buf, int bufsize, int *position, MPI_Comm comm){
  int size;
  PMPI_Unpack(buf, bufsize, position, &size, 1, MPI_INT, comm);
  for(int i=0; i<size; i++){
    loop_t *newLoop = new loop_t;
    newLoop->mem = new Param();
    newLoop->mem->unpack(buf, bufsize, position, comm);
    newLoop->iter = new Param();
    newLoop->iter->unpack(buf, bufsize, position, comm);
    loops.push_back(newLoop);
  }
}
