/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "Trace.h"


/*************************************************************************
 * PendingIterations
 * **********************************************************************/

PendingIterations::PendingIterations(){}

PendingIterations::~PendingIterations(){
  iterationmap_iterator_t it;
  for(it = mh_key.begin(); it != mh_key.end(); it++){
    if(it->second)
      delete it->second;
  }
  mh_key.clear();
  mt_key.clear();
  th_key.clear();
  tt_key.clear();
}

void PendingIterations::insert(iteration_t *iteration){
  mh_key.insert(pair<Event*, iteration_t*>(iteration->merge_head, iteration));
  mt_key.insert(pair<Event*, iteration_t*>(iteration->merge_tail, iteration));
  th_key.insert(pair<Event*, iteration_t*>(iteration->target_head, iteration));
  tt_key.insert(pair<Event*, iteration_t*>(iteration->target_tail, iteration));
}

void PendingIterations::create(Event* merge_head, Event* merge_tail, int merge_length,
    Event* target_head, Event* target_tail, int target_length){
  iteration_t *iteration = new iteration_t;
  iteration->merge_head = merge_head;
  iteration->merge_tail = merge_tail;
  iteration->merge_length = merge_length;
  iteration->target_head = target_head;
  iteration->target_tail = target_tail;
  iteration->target_length = target_length;
  insert(iteration);
}

vector<iteration_t *> PendingIterations::getIterationsByMergeHead(Event* merge_head){
  pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
  iterations = mh_key.equal_range(merge_head);
  iterationmap_iterator_t it;
  vector<iteration_t *> rtn;
  for(it = iterations.first; it != iterations.second; ++it)
    rtn.push_back(it->second);
  return rtn;
}
vector<iteration_t *> PendingIterations::getIterationsByMergeTail(Event* merge_tail){
  pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
  iterations = mt_key.equal_range(merge_tail);
  iterationmap_iterator_t it;
  vector<iteration_t *> rtn;
  for(it = iterations.first; it != iterations.second; ++it)
    rtn.push_back(it->second);
  return rtn;
}
vector<iteration_t *> PendingIterations::getIterationsByTargetHead(Event* target_head){
  pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
  iterations = th_key.equal_range(target_head);
  iterationmap_iterator_t it;
  vector<iteration_t *> rtn;
  for(it = iterations.first; it != iterations.second; ++it)
    rtn.push_back(it->second);
  return rtn;
}
vector<iteration_t *> PendingIterations::getIterationsByTargetTail(Event* target_tail){
  pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
  iterations = tt_key.equal_range(target_tail);
  iterationmap_iterator_t it;
  vector<iteration_t *> rtn;
  for(it = iterations.first; it != iterations.second; ++it)
    rtn.push_back(it->second);
  return rtn;
}

void PendingIterations::deleteIteration(iteration_t* iteration){
  pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
  iterationmap_iterator_t it;

  iterations = mh_key.equal_range(iteration->merge_head);
  for(it = iterations.first; it != iterations.second; ++it)
    if(it->second == iteration){
      mh_key.erase(it);
      break;
    }
  iterations = mt_key.equal_range(iteration->merge_tail);
  for(it = iterations.first; it != iterations.second; ++it)
    if(it->second == iteration){
      mt_key.erase(it);
      break;
    }
  iterations = th_key.equal_range(iteration->target_head);
  for(it = iterations.first; it != iterations.second; ++it)
    if(it->second == iteration){
      th_key.erase(it);
      break;
    }
  iterations = tt_key.equal_range(iteration->target_tail);
  for(it = iterations.first; it != iterations.second; ++it)
    if(it->second == iteration){
      tt_key.erase(it);
      break;
    }
}

void PendingIterations::print(){
  iterationmap_iterator_t it;
  for(it = mh_key.begin(); it != mh_key.end(); it++){
    if(it->second){
      cerr << it->second->merge_head << " ";
      cerr << it->second->merge_head->getOp() << " ";
      cerr << it->second->merge_tail << " ";
      cerr << it->second->merge_tail->getOp() << " ";
      cerr << it->second->merge_length << " ";
      cerr << it->second->target_head << " ";
      cerr << it->second->target_head->getOp() << " ";
      cerr << it->second->target_tail << " ";
      cerr << it->second->target_tail->getOp() << " ";
      cerr << it->second->target_length << endl;
    }
  }
  cerr << endl;
  cerr << endl;
  cerr << endl;
}

void PendingIterations::updateIterationMergeHead(iteration_t* iteration, Event* new_merge_head){
  pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
  iterationmap_iterator_t it;
  iterations = mh_key.equal_range(iteration->merge_head);
  for(it = iterations.first; it != iterations.second; ++it){
    if(it->second == iteration){
      mh_key.erase(it);
      mh_key.insert(pair<Event*, iteration_t*>(new_merge_head, iteration));
      iteration->merge_head = new_merge_head;
      break;
    }
  }
}
void PendingIterations::updateIterationMergeTail(iteration_t* iteration, Event* new_merge_tail){
  pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
  iterationmap_iterator_t it;
  iterations = mt_key.equal_range(iteration->merge_tail);
  for(it = iterations.first; it != iterations.second; ++it){
    if(it->second == iteration){
      mt_key.erase(it);
      mt_key.insert(pair<Event*, iteration_t*>(new_merge_tail, iteration));
      iteration->merge_tail = new_merge_tail;
      break;
    }
  }
}
void PendingIterations::updateIterationTargetHead(iteration_t* iteration, Event* new_target_head){
  pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
  iterationmap_iterator_t it;
  iterations = th_key.equal_range(iteration->target_head);
  for(it = iterations.first; it != iterations.second; ++it){
    if(it->second == iteration){
      th_key.erase(it);
      th_key.insert(pair<Event*, iteration_t*>(new_target_head, iteration));
      iteration->target_head = new_target_head;
      break;
    }
  }
}
void PendingIterations::updateIterationTargetTail(iteration_t* iteration, Event* new_target_tail){
  pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
  iterationmap_iterator_t it;
  iterations = tt_key.equal_range(iteration->target_tail);
  for(it = iterations.first; it != iterations.second; ++it){
    if(it->second == iteration){
      tt_key.erase(it);
      tt_key.insert(pair<Event*, iteration_t*>(new_target_tail, iteration));
      iteration->target_tail = new_target_tail;
      break;
    }
  }
}


/*************************************************************************
 * Trace
 * **********************************************************************/

Trace::Trace(){
  head = NULL;
  tail = NULL;
}

Trace::~Trace(){
  Event* iter = head, *next;
  while(iter){
    next = iter->next;
    delete iter;
    iter = next;
  }
}

void Trace::deleteTrace(Trace *trace){
  if(trace)
    delete trace;
}



void Trace::fastCompressLoop(int window){
  static int warned_once = 0;
  int distance;

  int head_flag;      /* Whether target head has been found. */
  int match_flag;     /* Whether a node matching target head has been found. */
  int found_flag;     /* */

  int target_length=1, merge_length=1;

  Event *target_tail = NULL;
  Event *target_head = NULL;
  Event *merge_tail = NULL;
  Event *merge_head = NULL;

  distance = 0;
  head_flag = 0;
  found_flag = 1;

  do {
    target_tail = tail;

    if(found_flag || !head_flag) {
      /* if the found flag is set - start a new search */
      target_head = target_tail;
      distance = 0;
      target_length = merge_length = 1;
    } else {
      /* otherwise continue a previous search */
      target_head = target_head->prev;
      target_length++;
      merge_length = 1;
    }

    head_flag = 0;
    match_flag = 0;
    found_flag = 0;


    /* Search backwards in the queue until we find a match with the target tail.
			After this loop is done, either head_flag will be false (and nothing found)
			or the head will be just after the matching tail.
     */
    while(target_head->prev != NULL && (window == -1 || window > distance)) {
      if( !target_head->checkLoc(MEMBER)
          && target_tail->sigMatch(target_head->prev)
          && target_tail->opMatch(target_head->prev)){
        head_flag = 1;
        break;
      }
      target_head = target_head->prev;
      target_length++;
      distance++;
    }

    /* didn't find head: can skip the rest. */
    if (!head_flag) {
      /* Warn if the window was exceeded, but only warn once. */
      if(!warned_once && window != -1 && window <= distance) {
        fprintf(stderr, "warning: window exceeded - possible compression missed (window set at %d)\n", window);
        fflush(stderr);
        warned_once = 1;
      }
      continue;
    }

    /* If the head flag is true, we've got the target_tail, the target_head, and the merge_tail (just before target_head).
		Now we can try to find a match for the target_head somewhere before the merge tail.  This will be merge_head.*/
    merge_head = target_head->prev;
    merge_tail = target_head->prev;

    while(merge_head != NULL) {
      if( !merge_head->checkLoc(MEMBER)
          && merge_head->sigMatch(target_head)
          && merge_head->opMatch(target_head)
          && (merge_length == target_length)
      ) {
        match_flag = 1;
        break;
      }
      merge_head = merge_head->prev;
      merge_length++;
    }

    /* check lengths of sequences and avoid a traversal if there's clearly no match */
    if(merge_length != target_length ){
      continue;
    }

    /* If we didn't find the merge head, we know there's no match in the sequence. Bail. */
    if(!match_flag) continue;

    // Make sure the sequences [target_head .. target_tail] and [merge_head .. merge_tail] match.
    if (!matchLoop(merge_head, merge_tail, merge_length, target_head, target_tail, target_length)) {
      continue;
    } else {
      found_flag = 1;
      mergeLoopEvents(merge_head, merge_tail, target_head, target_tail);
      // this adjusts prsd info in the merge sequence to reflect the new loop we're adding
      updateLoopInfo(merge_head, merge_tail, target_head, target_tail, merge_length);
      // if the compression was successful, go through and free all the target nodes from the end
      // of the seqence.
      deleteTail(merge_tail);
    }

  } while(head_flag || found_flag);

} /* fastCompressLoop */


bool Trace::matchLoop(Event* merge_head, Event* merge_tail, int merge_length, 
    Event *target_head, Event* target_tail, int target_length){

  Event *merge_iter = merge_tail;
  Event *target_iter = target_tail;

  do {
    /* check signatures */
    if( !(target_iter->sigMatch(merge_iter) && target_iter->opMatch(merge_iter)) ) {
      return false;
    }

    /* check loop information - the merge head is a special case
			we can merge the target into any loop layer */
    int cur_depth;
    if(merge_head == merge_iter) {
      cur_depth = target_iter->getLoopDepth();
    } else {
      cur_depth = merge_iter->getLoopDepth();
    }

    if(!merge_iter->loopMatch(target_iter, cur_depth, target_length) ) {
      return false;
    }

    merge_iter = merge_iter->prev;
    target_iter = target_iter->prev;

  } while (merge_iter && target_iter && merge_iter != merge_head->prev && target_iter != target_head->prev);

  // if we got through the whole sequence, this is true.
  return (merge_iter == merge_head->prev && target_iter == target_head->prev);
} /* matchLoop */


void Trace::mergeLoopEvents(Event* merge_head, Event* merge_tail, Event* target_head, Event* target_tail){
  Event* merge_iter;
  Event* target_iter;
#if FEATURE_SIG_DIFF > 0
  /* the following code changes the signature of the histograms */
  merge_iter = merge_tail;
  target_iter = target_tail;

  Event* search;
  StackSig* target_sig;
  StackSig* merge_sig;

  while(target_iter != merge_tail){
    target_sig = target_iter->getSignature();
    merge_sig = merge_iter->getSignature();
    search = target_tail;
    while(search != merge_tail){
      search->updateStatsSig(target_sig, merge_sig);
      search = search->prev;
    }
    target_iter->setSignature(*merge_sig);
    target_iter = target_iter->prev;
    merge_iter = merge_iter->prev;
  }
#endif
  merge_iter = merge_tail;
  target_iter = target_tail;
  while(target_iter != merge_tail){
    merge_iter->eventParamMerge(target_iter);
    merge_iter->eventStatsMerge(target_iter);
    target_iter = target_iter->prev;
    merge_iter = merge_iter->prev;
  }
} /* mergeLoopEvents */


void Trace::updateLoopInfo(Event* merge_head, Event *merge_tail, Event *target_head, Event *target_tail, int merge_length){
  /* first, merge the loop structures in the loop body of the current depth */
  Event *miter = merge_tail, *titer = target_tail;
  while(titer != target_head){
    miter->getLoopStack()->mergeLoopStack(titer->getLoopStack() );
    miter->setLoc(MEMBER);
    miter = miter->prev;
    titer = titer->prev;
  }

  /* then, merge the loop heads */
  int target_depth = target_head->getLoopDepth();
  int merge_depth = merge_head->getLoopDepth();
  if(target_depth == merge_depth){
    merge_head->getLoopStack()->mergeLoopStack(target_head->getLoopStack() );
    merge_head->insertLoop(merge_length, 2);
  } else {
    assert(merge_depth > target_depth);
    merge_head->getLoopStack()->mergeLoopStack(target_head->getLoopStack() );
    merge_head->getLoopStack()->incLoopIter(merge_depth - target_depth - 1);
  }
  merge_head->setLoc(LEADER);
} /* updateLoopInfo */


void Trace::deleteTail(Event* merge_tail){
  Event *iter = tail, *prev;
  while(iter != merge_tail){
    prev = iter->prev;
    delete iter;
    iter = prev;
  }
  iter->next = NULL;
  tail = iter;
}




void Trace::slowCompressLoop(int window){
  static int warned_once = 0;
  int distance;
  Event *iter;
  iteration_t *rtn;
  iteration_t *iteration;
  vector<iteration_t *> iterations;
  vector<iteration_t *>::iterator iteration_it;
  int i;

  int head_flag;      /* Whether target head has been found. */
  int match_flag;     /* Whether a node matching target head has been found. */
  int found_flag;     /* */

  int target_length=1, merge_length=1;
  int target_real_length, merge_real_length;
  double ratio;

  Event *target_tail = NULL;
  Event *target_head = NULL;
  Event *merge_tail = NULL;
  Event *merge_head = NULL;

#if defined FEATURE_SIG_DIFF && FEATURE_SIG_DIFF > 0
  Event* search;
  StackSig* target_sig;
  StackSig* merge_sig;
#endif

  distance = 0;
  head_flag = 0;
  found_flag = 1;

  do {
    target_tail = tail;

    if(found_flag || !head_flag) {
      /* if the found flag is set - start a new search */
      target_head = target_tail;
      distance = 0;
      target_length = merge_length = 1;
    } else {
      /* otherwise continue a previous search */
      target_head = target_head->prev;
      target_length++;
      merge_length = 1;
    }

    head_flag = 0;
    match_flag = 0;
    found_flag = 0;


    /* Search backwards in the queue until we find a match with the target tail.
     * After this loop is done, either head_flag will be false (and nothing found)
     * or the head will be just after the matching tail. */
    while(target_head->prev != NULL && (window == -1 || window > distance)) {
      if( !target_head->checkLoc(MEMBER)
          && !target_head->checkLoc(PENDING_MEMBER)
          && !target_head->checkLoc(PENDING_LEADER)
          && !target_head->checkLoc(PENDING_TAIL)
          && target_tail->sigMatch(target_head->prev)
          && target_tail->opMatch(target_head->prev)){
        head_flag = 1;
        break;
      }
      target_head = target_head->prev;
      target_length++;
      distance++;
    }

    /* didn't find head: can skip the rest. */
    if (!head_flag) {
      /* Warn if the window was exceeded, but only warn once. */
      if(!warned_once && window != -1 && window <= distance) {
        cerr<<"warning: window exceeded - possible compression missed (window: "<<window<<"   distance: "<<distance<<")"<<endl;
        warned_once = 1;
      }
      break;
    }

    /* If the head flag is true, we've got the target_tail, the target_head, and
     * the merge_tail (just before target_head). Now we can try to find a match
     * for the target_head somewhere before the merge tail.  This will be merge_head.*/
    merge_head = target_head->prev;
    merge_tail = target_head->prev;

    while(merge_head != NULL) {
      if( !merge_head->checkLoc(MEMBER)
          && merge_head->sigMatch(target_head)
          && merge_head->opMatch(target_head)){

        target_real_length = realLength(target_head, target_tail);
        merge_real_length = realLength(merge_head, merge_tail);
        assert(target_real_length > 0 && merge_real_length > 0);
        if(target_real_length > merge_real_length){
          ratio = (double)merge_real_length / (double)target_real_length;
        } else {
          ratio = (double)target_real_length / (double)merge_real_length;
        }
        if(1 - ratio < MAX_LENGTH_DIFF){
#if defined FEATURE_SIG_DIFF && FEATURE_SIG_DIFF > 0
          /* heuristic that helps to identify the best match */
          if( (target_tail->sigEquals(merge_tail) && target_head->sigEquals(merge_head))
              || (merge_length == target_length && merge_length == 1 )  )
#endif
            if(merge_head->checkLoc(PENDING_MEMBER) || merge_head->checkLoc(PENDING_LEADER)
                || merge_head->checkLoc(PENDING_TAIL)){
              for(iter = merge_head, i = 0; iter; iter = iter->prev, i++){
                iterations = pendingIterations.getIterationsByTargetHead(iter);
                if(iterations.size() == 1){
                  if(iterations[0]->target_tail->getId() >= merge_head->getId()){
                    break;
                  }
                }else{
                  /* only one pending iteration can have iter as the
                   * target_head, because it's impossible to choose a
                   * PENDING_LEADER event as the target_head */
                  assert(iterations.size()==0);
                }
                iterations.clear();
              }
              if(iterations.size() == 1 && (merge_tail == iterations[0]->target_tail)){
                match_flag = 1;
                break;
              }
            } else {
              match_flag = 1;
              break;
            }
        }
      }
      merge_head = merge_head->prev;
      merge_length++;
    }

    /* If we didn't find the merge head, we continue to the next iteration to try to
     * find a longer iteration. */
    if(!match_flag)
      continue;
    found_flag = 1;

    /* mark the loop iteration */
    /* 1. check if the current iteration is a trailing iteration, if
     * it is a trailing iteration, only the case where merge_tail equals
     * to the tail of the pervious iteration is allowed */
    if(merge_head->checkLoc(PENDING_MEMBER) || merge_head->checkLoc(PENDING_LEADER)
        || merge_head->checkLoc(PENDING_TAIL)){
      for(iter = merge_head, i = 0; iter; iter = iter->prev, i++){
        iterations = pendingIterations.getIterationsByTargetHead(iter);
        if(iterations.size() == 1){
          //if(iterations[0]->target_length - i > 0){
          if(iterations[0]->target_tail->getId() >= merge_head->getId()){
            break;
          }
        }else{
          /* only one pending iteration can have iter as the
           * target_head, because it's impossible to choose a
           * PENDING_LEADER event as the target_head */
          assert(iterations.size()==0);
        }
        iterations.clear();
      }

      if(iterations.size() == 1 && (merge_tail == iterations[0]->target_tail)){
        iterations = pendingIterations.getIterationsByTargetTail(merge_tail);
        for(iteration_it = iterations.begin();
            iteration_it != iterations.end(); iteration_it++)
          for(iter = (*iteration_it)->target_tail;
              iter != (*iteration_it)->target_head; iter = iter->prev)
            if(iter == merge_head){
              (*iteration_it)->target_tail->unsetLoc(PENDING_TAIL);
              (*iteration_it)->target_tail->setLoc(PENDING_MEMBER);
              pendingIterations.updateIterationTargetTail((*iteration_it), target_tail);
              (*iteration_it)->target_length += target_length;
              break;
            }

      } else {
        found_flag = 0;
        continue;
      }
    }


#if defined FEATURE_SIG_DIFF && FEATURE_SIG_DIFF > 0
    if(!target_tail->sigEquals(merge_tail)){
      target_sig = target_tail->getSignature();
      merge_sig = merge_tail->getSignature();
      search = target_tail;
      while(search != merge_tail){
        search->updateStatsSig(target_sig, merge_sig);
        search = search->prev;
      }
      target_tail->setSignature(*merge_sig);
    }
    if(target_tail != target_head && !target_head->sigEquals(merge_head)){
      target_sig = target_head->getSignature();
      merge_sig = merge_head->getSignature();
      search = target_tail;
      while(search != merge_tail){
        search->updateStatsSig(target_sig, merge_sig);
        search = search->prev;
      }
      target_head->setSignature(*merge_sig);
    }
#endif


    /* 2. if merge_head ... merge_tail is a pending iteration, merge it */
    iterations = pendingIterations.getIterationsByTargetHead(merge_head);
    rtn = NULL;
    for(iteration_it = iterations.begin(); iteration_it != iterations.end(); iteration_it++){
      if((*iteration_it)->target_head == merge_head && (*iteration_it)->target_tail == merge_tail){
        iteration = *iteration_it;
        rtn = compressLoopLCS(iteration);
        updateIds(rtn->merge_head);
        break;
      }
    }

    /* 3. mark the current iteration as pending */
    if(rtn){
      pendingIterations.create(rtn->merge_head, rtn->merge_tail, rtn->merge_length,
          target_head, target_tail, target_length);
      delete rtn;
    } else {
      pendingIterations.create(merge_head, merge_tail, merge_length, target_head, target_tail, target_length);
      for(iter = merge_tail; iter != merge_head && iter != NULL; iter=iter->prev)
        iter->setLoc(MEMBER);
      assert(iter);
      iter->setLoc(LEADER);
    }
    /* mark the events in the target iteration as PENDING */
    target_tail->setLoc(PENDING_TAIL);
    if(target_tail != target_head){
      for(iter = target_tail->prev; iter != target_head && iter != NULL; iter = iter->prev)
        iter->setLoc(PENDING_MEMBER);
    }
    target_head->setLoc(PENDING_LEADER);

  } while(head_flag && !found_flag);

} /* slowCompressLoop */

void Trace::updateIds(Event* from){
  int base = from->prev ? from->prev->getId() + 1 : 0;
  Event* iter = from;
  while(iter){
    iter->setId(base);
    base++;
    iter = iter->next;
  }
}

/* this function doesn't really return the real length due to 
 * performance consideration. It returns the longer one of 
 * the merge iteration and the target iteration */
int Trace::realLengthHelper(iteration_t* iteration, int bound){
  Event *iter;
  int max;
  iteration_t *current;
  vector<iteration_t *> iterations;
  vector<iteration_t *>::iterator iteration_it;
  int target_real_length = 0, merge_real_length = 0;

  for(iter = iteration->target_head; iter != iteration->target_tail->next && iter != NULL; ){
    if(iter->checkLoc(LEADER)){
      iterations = pendingIterations.getIterationsByMergeHead(iter);
      max = INT_MIN;
      current = NULL;
      for(iteration_it = iterations.begin(); iteration_it != iterations.end(); ++iteration_it){
        if((*iteration_it)->merge_length > max){
          current = *iteration_it;
          max = (*iteration_it)->merge_length;
        }
      }
      if(current){
        iter = current->target_tail->next;
        target_real_length += realLengthHelper(current, current->merge_length);
        continue;
      }
    }
    iter = iter->next;
    target_real_length++;
  }

  /* check if there is pending iteration inside the merge iteration */
  for(iter = iteration->merge_head; iter != iteration->merge_tail->next && iter != NULL; ){
    if(iter->checkLoc(LEADER)){
      iterations = pendingIterations.getIterationsByMergeHead(iter);
      max = INT_MIN;
      current = NULL;
      for(iteration_it = iterations.begin(); iteration_it != iterations.end(); ++iteration_it){
        if((*iteration_it)->merge_length < bound && (*iteration_it)->merge_length > max){
          current = *iteration_it;
          max = (*iteration_it)->merge_length;
        }
      }
      if(current){
        iter = current->target_tail->next;
        merge_real_length += realLengthHelper(current, current->merge_length);
        continue;
      }
    }
    iter = iter->next;
    merge_real_length++;
  }

  return merge_real_length > target_real_length ? merge_real_length : target_real_length;
}

int Trace::realLength(Event* head, Event* tail){
  Event* iter;
  int max;
  int len = 0;
  vector<iteration_t *> iterations;
  vector<iteration_t *>::iterator iteration_it;
  iteration_t *current;

  for(iter = head; iter != NULL && iter != tail->next; ){
    if(iter->checkLoc(LEADER)){
      iterations = pendingIterations.getIterationsByMergeHead(iter);
      max = INT_MIN;
      for(iteration_it = iterations.begin(); iteration_it != iterations.end(); ++iteration_it){
        if((*iteration_it)->merge_length > max){
          current = *iteration_it;
          max = (*iteration_it)->merge_length;
        }
      }
      if(current){
        iter = current->target_tail->next;
        len += realLengthHelper(current, current->merge_length);
        continue;
      }
    }
    iter = iter->next;
    len++;
  }
  return len;
}



iteration_t* Trace::compressLoopLCS(iteration_t* iteration){
  Event *iter;
  int max, count, iteration_length;
  iteration_t *current, *rtn;
  vector<matching_pair_t *> *pairs = NULL;
  vector<iteration_t *> iterations;
  vector<iteration_t *>::iterator iteration_it;

  /* delete the current iteration from pendingIterations so that it won't be
   * evaluated repeatedly */
  pendingIterations.deleteIteration(iteration);

  /* check if there is pending iteration inside the target iteration */
  for(iter = iteration->target_head; iter != iteration->target_tail->next && iter != NULL; iter = iter->next){
    if(iter->checkLoc(LEADER)){
      /* loops are either completely disjoint or perfectly embedded: if there is
       * a LEADER event (merge_head) in an iteration, the corresponding merge_tail,
       * target_head, target_tail must also be in the same iteration. Therefore, when
       * we merge target_head ... target_tail into the previous iteration and remove
       * them, we only have to handle the case where target_tail is also the last event
       * of a larger iteration that directly or indirectly contains the two iterations
       * merge_head ... merge_tail target_head ... target_tail.
       */
      iterations = pendingIterations.getIterationsByMergeHead(iter);
      max = INT_MIN;
      current = NULL;
      for(iteration_it = iterations.begin(); iteration_it != iterations.end(); ++iteration_it){
        if((*iteration_it)->merge_length > max){
          current = *iteration_it;
          max = (*iteration_it)->merge_length;
        }
      }
      if(current){
        if(iteration->target_tail == current->target_tail){
          rtn = compressLoopLCS(current);
          iteration->target_tail = rtn->merge_tail;
        } else {
          rtn = compressLoopLCS(current);
        }
        if(rtn)
          delete rtn;
      }
    }
  }

  /* check if there is pending iteration inside the merge iteration */
  for(iter = iteration->merge_head; iter != iteration->merge_tail->next && iter != NULL; iter = iter->next){
    if(iter->checkLoc(LEADER)){
      iterations = pendingIterations.getIterationsByMergeHead(iter);
      max = INT_MIN;
      current = NULL;
      for(iteration_it = iterations.begin(); iteration_it != iterations.end(); ++iteration_it){
        if((*iteration_it)->merge_length > max){
          current = *iteration_it;
          max = (*iteration_it)->merge_length;
        }
      }
      if(current){
        if(iteration->merge_tail == current->target_tail){
          rtn = compressLoopLCS(current);
          iteration->merge_tail = rtn->merge_tail;
        } else {
          rtn = compressLoopLCS(current);
        }
        if(rtn)
          delete rtn;
      }
    }
  }

  for(iter = iteration->target_head, count = 1; iter != iteration->target_tail; iter = iter->next, count++);
  iteration->target_length = count;
  for(iter = iteration->merge_head, count = 1; iter != iteration->merge_tail; iter = iter->next, count++);
  iteration->merge_length = count;

  pairs = matchLoopLCS(iteration->merge_head, iteration->merge_tail, iteration->merge_length,
      iteration->target_head, iteration->target_tail, iteration->target_length );
  if(!pairs){
    assert(0);
  } else {
    iteration_length = updateLoopLCS(iteration->merge_head, iteration->merge_tail,
        iteration->merge_length, iteration->target_head, iteration->target_tail, pairs);
    mergeLoopEvents(iteration->merge_head, iteration->merge_tail,
        iteration->target_head, iteration->target_tail);
    updateLoopInfo(iteration->merge_head, iteration->merge_tail, iteration->target_head,
        iteration->target_tail, iteration_length);
    deleteIteration(iteration->target_head, iteration->target_tail);
  }

  for(unsigned i=0; i<pairs->size(); i++)
    delete pairs->at(i);
  delete pairs;

  iteration->target_head = NULL;
  iteration->target_tail = NULL;
  iteration->target_length = 0;
  iteration->merge_length = iteration_length;
  return iteration;
}

void Trace::finalizePendingIterations(){
  Event *iter;
  vector<iteration_t *> iterations;
  vector<iteration_t *>::iterator iteration_it;
  iteration_t *current, *rtn;
  int max;

  for(iter = head; iter != NULL; iter = iter->next){
    if(iter->checkLoc(LEADER)){
      iterations = pendingIterations.getIterationsByMergeHead(iter);
      max = INT_MIN;
      current = NULL;
      rtn = NULL;
      for(iteration_it = iterations.begin(); iteration_it != iterations.end(); ++iteration_it){
        if((*iteration_it)->merge_length > max){
          current = *iteration_it;
          max = (*iteration_it)->merge_length;
        }
      }
      if(current)
        rtn = compressLoopLCS(current);
      if(rtn)
        delete rtn;
    }
  }
}

void Trace::cleanupLoops(){
  Event *iter = head;
  while(iter){
    iter->getLoopStack()->cleanup();
    iter = iter->next;
  }
}


vector<matching_pair_t *>* Trace::matchLoopLCS(Event* merge_head, Event* merge_tail, int merge_length,
    Event *target_head, Event* target_tail, int target_length){
  assert(merge_length > 0 && target_length > 0);

  /* calculate the longest common subsequence */
  vector<matching_pair_t *> *pairs;
  /* merge_head and target_head should always be in the LCS */
  if(merge_length == 1 && target_length == 1){
    pairs = new vector<matching_pair_t *>();
    matching_pair_t *pair = new matching_pair_t;
    pair->index1 = 0;
    pair->index2 = 0;
    pairs->push_back(pair);
    return pairs;
  } else if( (merge_length == 1 && target_length == 2) || (merge_length == 2 && target_length == 1)){
    /* should never reach here! */
    assert(0);
  } else {
    merge_head = merge_head->next;
    merge_length--;
    target_head = target_head->next;
    target_length--;
  }

  int width = target_length + 1;
  int *b, *c;

  b = (int *) calloc( (merge_length+1) * (target_length+1), sizeof(int) );
  if(!b){
    cerr << "calloc(" << (merge_length+1) * (target_length+1) * sizeof(int) << ") b failed" << endl;
    exit(1);
  }
  c = (int *) calloc( (merge_length+1) * (target_length+1), sizeof(int) );
  if(!c){
    cerr << "calloc(" << (merge_length+1) * (target_length+1) * sizeof(int) << ") c failed" << endl;
    exit(1);
  }
  Event *miter, *titer;
  int i, j;
  for(i=0, miter = merge_head; miter != merge_tail->next && miter != NULL; i++, miter = miter->next ){
    for(j=0, titer = target_head; titer != target_tail->next && titer != NULL; j++, titer = titer->next ){
      if(miter->sigMatch(titer) && miter->opMatch(titer) ){
        c[(i+1)*width + (j+1)] = c[i*width + j] + 1;
        b[(i+1)*width + (j+1)] = DIRECTION_UL;
      } else if(c[i*width + (j+1)] >= c[(i+1)*width + j]) {
        c[(i+1)*width + (j+1)] = c[i*width + (j+1)] ;
        b[(i+1)*width + (j+1)] = DIRECTION_UP;
      } else {
        c[(i+1)*width + (j+1)] = c[(i+1)*width + j] ;
        b[(i+1)*width + (j+1)] = DIRECTION_LEFT;
      }
    }
  }

  pairs = getLCS(b, merge_length, target_length);
  for(unsigned i=0; i<pairs->size(); i++){
    pairs->at(i)->index1++;
    pairs->at(i)->index2++;
  }
  matching_pair_t *pair = new matching_pair_t;
  pair->index1 = 0;
  pair->index2 = 0;
  pairs->insert(pairs->begin(), pair);
  free(b);
  free(c);
  return pairs;
} /* matchLoopLCS */



void Trace::insertEvent(Event *e, Event *position){
  if( !e || !position ){
    cerr << "error: cannot insert " << e << " after " << position << endl;
    exit(1);
  }
  e->prev = position;
  if(position->next){
    e->next = position->next;
    e->next->prev = e;
  }
  position->next = e;
}


int Trace::updateLoopLCS(Event* merge_head, Event* merge_tail, int merge_length, Event* target_head, Event* target_tail, vector<matching_pair_t *>* pairs){

  int i = 0, j = 0, mend_pos = 0, tend_pos = 0, tbegin_pos = 0, expand, mdepth, tdepth;
  int n = pairs->size();
  Event *mbegin, *mend, *tbegin, *tend, *iter, *dummy, *temp;
  mbegin = mend = merge_head;
  tbegin = tend = target_head;
  bool changeMade = false;
  /* prerequisite: merge_head matches target_head, merge_tail matches target_tail */
  assert(pairs->size() > 0 && pairs->at(0)->index1 == 0 && pairs->at(0)->index2 == 0 );

  loop_t *outer;
  loophead_t* head;
  vector<loophead_t *> mloopheads;
  vector<loophead_t *> tloopheads;

  if(merge_head->getLoopDepth() > 1){ /* all events have one level of loop for themselves (mem:1 iter:1) */
    head = new loophead_t;
    head->event = merge_head;
    head->pos = mend_pos;
    mloopheads.push_back(head);
  }
  if(target_head->getLoopDepth() > 1){
    head = new loophead_t;
    head->event = target_head;
    head->pos = tend_pos;
    tloopheads.push_back(head);
  }

  for(int pair_iter = 0; pair_iter < n; pair_iter++){
    while(i < pairs->at(pair_iter)->index1){
      mend = mend->next;
      i++;
      mend_pos++;
      if(mend->getLoopDepth() > 1){
        head = new loophead_t;
        head->event = mend;
        head->pos = mend_pos;
        mloopheads.push_back(head);
      }
    }
    while(j < pairs->at(pair_iter)->index2){
      tend = tend->next;
      j++;
      tend_pos++;
      if(tend->getLoopDepth() > 1){
        head = new loophead_t;
        head->event = tend;
        head->pos = tend_pos;
        tloopheads.push_back(head);
      }
    }

    temp = mend->prev;
    if(tbegin != tend){
      /* mend->prev should always exist */
      assert(mend->prev);
      expand = mend->getLoopStack()->getLoopAt(0)->mem->getNumValues(true);
      for(iter = tbegin; iter != tend; iter = iter->next){
        dummy = iter->getDummyEvent();
        /* expand the loop information by referring to the immediate following event:
         * assume the following event is E and the number of times E was called is n,
         * n equals to the (uncompressed) length of the mem/iter vector of the outer
         * most loop of E. If E starts loop structures, n equals to the times E was
         * called if there is no loop structures on E. Therefore, in either case, n
         * will be the times the dummy event will be evaluated if inserted to the merge
         * iteration */
        dummy->expandLoop(expand);
        insertEvent(dummy, mend->prev);
        mend_pos++;
        /* increase mem by one for loops that contain the inserted event */
        for(vector<loophead_t *>::iterator it = mloopheads.begin(); it != mloopheads.end(); ){
          changeMade = (*it)->event->getLoopStack()->incLoopMem(mend_pos - 1 - (*it)->pos);
          if(!changeMade)
            mloopheads.erase(it);
          else
            it++;
        }
      }
    }
    if(mbegin != mend){
      /* tend->prev should always exist */
      assert(tbegin->prev);
      expand = tbegin->getLoopStack()->getLoopAt(0)->mem->getNumValues(true);
      for(iter = mbegin; iter != temp->next; iter = iter->next){
        dummy = iter->getDummyEvent();
        dummy->expandLoop(expand);
        insertEvent(dummy, tbegin->prev);
        tend_pos++;
        tbegin_pos++;
        /* increase mem by one for loops that contain the inserted event */
        for(vector<loophead_t *>::iterator it = tloopheads.begin(); it != tloopheads.end(); ){
          changeMade = (*it)->event->getLoopStack()->incLoopMem(tbegin_pos - (*it)->pos);
          if(!changeMade)
            tloopheads.erase(it);
          else
            it++;
        }
      }
    }

    if(mend != merge_head && tend != target_head){
      mdepth = mend->getLoopDepth();
      tdepth = tend->getLoopDepth();
      if(mdepth > tdepth)
        tend->alignLoop(mdepth);
      else
        mend->alignLoop(tdepth);
    } else if(mend == merge_head && tend == target_head){
      /* heads are handled differently */
      /* check the outer most loop of merge_head, if it's a loop of the entire
       * iteration (mem == merge_length), align the rest of the loop nest with
       * the loops on target_head, otherwise align all loops on merge_head with
       * the loops on target_head */
      if(mend->getLoopStack()->getLoopAt(0)->mem->getNumValues(true) == 1
          && mend->getLoopStack()->getLoopAt(0)->mem->getScalarValueForRank() == merge_length ){
        outer = mend->getLoopStack()->popOuterLoop();
      } else {
        outer = NULL;
      }
      mdepth = mend->getLoopDepth();
      tdepth = tend->getLoopDepth();
      if(mdepth > tdepth)
        tend->alignLoop(mdepth);
      else
        mend->alignLoop(tdepth);
      if(outer)
        mend->getLoopStack()->insertLoop(outer);
    } else {
      assert(0); /* should never reach here!! */
    }

    mbegin = mend->next;
    tbegin = tend->next;
    tbegin_pos = tend_pos;
  }

  mloopheads.clear();
  tloopheads.clear();

  return mend_pos + 1;
}

void Trace::deleteIteration(Event* target_head, Event* target_tail){
  if(target_head->prev){
    target_head->prev->next = target_tail->next;
  }
  if(target_tail->next){
    target_tail->next->prev = target_head->prev;
  }
  if(target_tail == tail){
    tail = target_head->prev;
  }
  target_head->prev = NULL;
  target_tail->next = NULL;

  Event *iter = target_tail, *prev;
  while(iter){
    prev = iter->prev;
    delete iter;
    iter = prev;
  }
}




void Trace::resetTraversalIterators(){
  Event *iter = head;
  while(iter){
    iter->resetTraversalIterators();
    iter = iter->next;
  }
}

void Trace::appendEvent(Event* event, bool compress){
  if(!event)
    return;

  if(head == NULL){
    event->setId(0);
    head = tail = event;
  } else {
    event->setId(tail->getId()+1);
    tail->next = event;
    event->prev = tail;
    tail = event;
  }
  if(compress){
#ifndef FEATURE_LOOP_LCS
    fastCompressLoop(WINDOW_SIZE);
#else
    slowCompressLoop(WINDOW_SIZE);
#endif
  }

}

void Trace::outputTrace(const char* filename){
  ofstream tracefile(filename);
  if(!tracefile ){
    cerr << "error: unable to open trace file: " << filename;
    return;
  }
  Event *iter = head;
  int seq_num = 0;
  while(iter){
    tracefile << iter->toString(seq_num++) << "\n";
    iter = iter->next;
  }
  tracefile.close();
}


Trace* Trace::inputTrace(const char* filename, int mode, int rank){
  string line, eventbuf;
  ifstream tracefile(filename);
  if(!tracefile){
    cerr << "error: unable to open trace file: " << filename << endl;
    exit(1);
  }
  Trace *trace = new Trace();
  int seqnum = 0;
  while(getline(tracefile, line)){
    if(line.size() > 0){
      eventbuf += line + "\n";
    } else {
      Event *e = Event::inputEvent(eventbuf, mode, rank);
      e->setId(seqnum++);
      trace->appendEvent(e, false);
      eventbuf.clear();
    }
  }
  tracefile.close();
  return trace;
}


void Trace::printTrace(){
  Event *iter = head;
  while(iter){
    cout << iter->toString() << "\n";
    iter = iter->next;
  }

}

int Trace::getPackedSize(){
  int size = 0;
  size += sizeof(int); /* num of events */
  Event *iter = head;
  while(iter){
    size += iter->getPackedSize();
    iter = iter->next;
  }
  return size;
}

void Trace::pack(void *buf, int bufsize, int *position, MPI_Comm comm){
  int len = traceLen();
  PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);
  Event *iter = head;
  while(iter){
    iter->pack(buf, bufsize, position, comm);
    iter = iter->next;
  }
}

void Trace::unpack(void *buf, int bufsize, int *position, MPI_Comm comm){
  int len;
  PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);
  for(int i=0; i<len; i++){
    Event *e = new Event();
    e->unpack(buf, bufsize, position, comm);
    appendEvent(e, false);
  }
}

void Trace::sendTrace(int dest){
  int bufsize = getPackedSize();
  void *buf = malloc(bufsize);
  MPI_Comm comm = MPI_COMM_WORLD;
  int position = 0;
  if(!buf){
    cerr << "Trace::sendTrace(): cannot allocate buffer" << endl;
    exit(1);
  }
  PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
  pack(buf, bufsize, &position, comm);
  PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
  free(buf);
}

void Trace::recvTrace(int src){
  int bufsize;
  void *buf;
  MPI_Comm comm = MPI_COMM_WORLD;
  int position = 0;

  PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
  buf = malloc(bufsize);
  if(!buf){
    cerr << "Trace::recvTrace(): cannot allocate buffer" << endl;
    exit(1);
  }
  PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
  unpack(buf, bufsize, &position, comm);
  free(buf);
}


void Trace::get_lcs_helper(int *b, int i, int j, int width, vector<matching_pair_t *> *pairs){
  if(i == 0 || j == 0)
    return;
  if(b[i*width+j] == DIRECTION_UL){
    get_lcs_helper(b, i-1, j-1, width, pairs);
    matching_pair_t *pair = new matching_pair_t;
    pair->index1 = i-1;
    pair->index2 = j-1;
    pairs->push_back(pair);
    return;
  } else if (b[i*width+j] == DIRECTION_UP){
    get_lcs_helper(b, i-1, j, width, pairs);
    return ;
  } else {
    get_lcs_helper(b, i, j-1, width, pairs);
    return ;
  }
}

vector<matching_pair_t *> * Trace::getLCS(int *b, int mlen, int slen){
  vector<matching_pair_t *> *pairs = new vector<matching_pair_t *>();
  get_lcs_helper(b, mlen, slen, slen+1, pairs);
  return pairs;
}


void Trace::LCSDoMerge(vector<matching_pair_t *> *pairs, Trace *slave){
  int i = 0, j = 0;
  int n = pairs->size();
  Event *mbegin, *mend, *sbegin, *send, *temp;
  mbegin = mend = head;
  sbegin = send = slave->getHead();
  assert(pairs->size()>0 && pairs->at(0)->index1 == 0 && pairs->at(0)->index2 == 0 );
  for(int pair_iter = 0; pair_iter < n; pair_iter++){
    while(i < pairs->at(pair_iter)->index1){
      mend = mend->next;
      i++;
    }
    while(j < pairs->at(pair_iter)->index2){
      send = send->next;
      j++;
    }
    if(sbegin != send){
      temp = send->prev;
      if(sbegin->prev){
        sbegin->prev->next = send;
        send->prev = sbegin->prev;
      } else {
        slave->setHead(send);
        send->prev = NULL;
      }

      if(mend->prev){
        sbegin->prev = mend->prev;
        mend->prev->next = sbegin;
      } else {
        head = sbegin;
        sbegin->prev = NULL;
      }
      mend->prev = temp;
      temp->next = mend;
    }
    mend->eventRanklistMerge(send);
    mend->eventParamMerge(send);
    mend->eventStatsMerge(send);
    mend->getLoopStack()->interMergeLoopStack(send->getLoopStack());
    mbegin = mend->next;
    sbegin = send->next;
  }
}



void Trace::LCSMergeTrace(Trace *slave){
  int mlen = traceLen();
  int slen = slave->traceLen();
  int width = slen + 1;
  int *b, *c;

  b = (int *) calloc( (mlen+1) * (slen+1), sizeof(int) );
  if(!b){
    fprintf(stderr, "calloc(%ld) b failed\n", (mlen+1) * (slen+1) * sizeof(int));
    exit(1);
  }
  c = (int *) calloc( (mlen+1) * (slen+1), sizeof(int) );
  if(!c){
    fprintf(stderr, "calloc(%ld) c failed\n", (mlen+1) * (slen+1) * sizeof(int));
    exit(1);
  }
  Event *miter, *siter;
  int i, j;
  for(i=0, miter = head; miter != NULL; i++, miter = miter->next ){
    for(j=0, siter = slave->getHead(); siter != NULL; j++, siter = siter->next ){
      if(miter->sigEquals(siter)){
        c[(i+1)*width + (j+1)] = c[i*width + j] + 1;
        b[(i+1)*width + (j+1)] = DIRECTION_UL;
      } else if(c[i*width + (j+1)] >= c[(i+1)*width + j]) {
        c[(i+1)*width + (j+1)] = c[i*width + (j+1)] ;
        b[(i+1)*width + (j+1)] = DIRECTION_UP;
      } else {
        c[(i+1)*width + (j+1)] = c[(i+1)*width + j] ;
        b[(i+1)*width + (j+1)] = DIRECTION_LEFT;
      }
    }
  }

  vector<matching_pair_t *> *pairs = getLCS(b, mlen, slen);
  LCSDoMerge(pairs, slave);
  for(unsigned i=0; i<pairs->size(); i++)
    delete pairs->at(i);
  delete pairs;
  free(b);
  free(c);
}

bool Trace::traverse(event_handler handler){
  return traverse_recursive(head, 0, handler, NULL);
}

bool Trace::traverse_recursive(Event* loophead, int depth, event_handler handler, int *len){
  if( loophead == NULL){
    cerr << "traverse_recursive() called with NULL loophead, depth = " << depth << endl;
    return false;
  }
  if(handler == NULL){
    cerr << "traverse_recursive() called without event handler" << endl;
    return false;
  }

  Event *looptail, *event_iter;
  Loop* loopstack = loophead->getLoopStack();
  loop_t *loop;
  vector<ValueSet *> *mems = NULL, *iters = NULL;
  int iter = 0, mem = 0, length;
  if(loopstack->getDepth() == 0){
    looptail = tail;
    iter = 1;
  } else if( 0 <= depth && depth < loopstack->getDepth() ){
    loop = loopstack->getLoopAt(depth);
    mems = loop->mem->getParamValues();
    iters = loop->iter->getParamValues();
    /* trace reader ensures that only the loop rlparam related to the current node is read */
    assert(mems->size() == 1);
    assert(iters->size() == 1);
    if(typeid(*(mems->at(0))) == typeid(Histogram) || typeid(*(iters->at(0))) == typeid(Histogram)){
      cerr << "traverse_recursive() doesn't support histogram trace: \n" << loophead->toString();
      return false;
    }
    mem = dynamic_cast<ParamValue *>(mems->at(0) )->getCurrentValue(true);
    iter = dynamic_cast<ParamValue *>(iters->at(0) )->getCurrentValue(true);
    looptail = loophead;
    for(int i=1; i<mem; i++)
      looptail = looptail->next;
    if(len)
      *len = mem;
  } else {
    cerr << "invalid depth: loophead: \n" << loophead->toString() << " depth = " << depth << endl;
    return false;
  }

  bool done ;
  for(int i=0; i<iter; i++){
    event_iter = loophead;
    done = false;
    while(!done){
      if(event_iter == NULL){
        cerr << "NULL event" << endl;
        exit(1);
      }

      if(event_iter == loophead && event_iter->getLoopStack()->getDepth() > depth+1){
        if( !traverse_recursive(event_iter, depth+1, handler, &length) ){
          return false;
        }
        for(int j=1; j<length; j++)
          event_iter = event_iter->next;
        if(event_iter == looptail)
          done = true;
        event_iter = event_iter->next;
      }

      if(done)
        break;

      if(event_iter != NULL && event_iter != loophead
          && event_iter->getLoopStack()->getDepth() > 0){
        if( !traverse_recursive(event_iter, 0, handler, &length) ){
          return false;
        }
        for(int j=1; j<length; j++)
          event_iter = event_iter->next;
        if(event_iter == looptail)
          done = true;
        event_iter = event_iter->next;
      } else if (event_iter != NULL){
        if(event_iter == looptail)
          done = true;
        handler(event_iter, depth, i);
        event_iter = event_iter->next;
      }
    }
  }
  return true;

}

/* ---- C wrapper functions follow ----- */

Trace trace, left_trace, right_trace;
extern int my_rank;

void appendEvent(Trace *trace, Event* event){
#ifdef COMPRESS_INTRA_NODE
  trace->appendEvent(event);
#else
  trace->appendEvent(event, false);
#endif
}

void finalizePendingIterations(Trace *trace){
  trace->finalizePendingIterations();
}

void cleanupLoops(Trace *trace){
  trace->cleanupLoops();
}

Trace* inputTrace(char *filename, int mode ){
  return Trace::inputTrace(filename, mode);
}

void outputTrace(Trace *trace, char *filename){
  trace->outputTrace(filename);
}

void sendTrace(Trace* trace, int dest){
  trace->sendTrace(dest);
}

void recvTrace(Trace* trace, int src){
  trace->recvTrace(src);
}

void mergeTrace(Trace *master, Trace *slave){
  master->LCSMergeTrace(slave);
}

void deleteTrace(Trace *trace){
  Trace::deleteTrace(trace);
}

Event *tailEvent(Trace *trace){
  return trace->getTail();
}

Event *headEvent(Trace *trace){
  return trace->getHead();
}
