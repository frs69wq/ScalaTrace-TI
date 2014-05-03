/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */
#include "histo_exec.h"

map<int, MPI_Group> comm_groups;
set<int> destSet;
set<int> srcSet;
map<int, Context*> peerContexts;
unsigned int seed = 0;
int dest_limit = INT_MAX;
priority_queue<sendinfo_t*, vector<sendinfo_t*>, CompareSendinfo> pending_recvs;
HandleQueue *handleQueue = NULL;

extern int doTestFor;
#ifdef DEBUG
map<int, int> scounts;
map<int, int> rcounts;
#endif

map<int, unsigned int> stags;
map<int, unsigned int> rtags;


void setSeed(unsigned int _seed){
  seed = _seed;
}

void setDestLimit(int _limit){
  dest_limit = _limit;
}

int translateRank(int rank, int in_comm, int out_comm){
  map<int, MPI_Group>::iterator in_group_it;
  map<int, MPI_Group>::iterator out_group_it;
  int out_rank = -1;

  if(out_comm == -1)
    out_comm = comm_to_index(replay_ptr, MPI_COMM_WORLD);

  if(in_comm == out_comm)
    return rank;

  in_group_it = comm_groups.find(in_comm);
  out_group_it = comm_groups.find(out_comm);
  if(in_group_it != comm_groups.end() && out_group_it != comm_groups.end() ){
    MPI_Group_translate_ranks(in_group_it->second, 1, &rank, out_group_it->second, &out_rank);
  }
  if(out_rank == -1 || out_rank == MPI_UNDEFINED){
    cerr << "error: cannot translate ranks" << endl;
    exit(0);
  }
  return out_rank;
}


void setupCommunicators(Trace* trace){
  trace->resetTraversalIterators();
  Context *myContext =  new Context(trace->getHead(), trace->getTail(), my_rank);
  Event *e;
  int op_code;
  MPI_Comm comm_out;
  MPI_Group group;

  while(myContext->hasNext()){
    e = myContext->next();
    op_code = e->getOp();

    switch(op_code){
    case umpi_MPI_Init:
      replay_ptr = init_ptr (PTR_HANDLER_SZ);
      MPI_Comm_group(MPI_COMM_WORLD, &group);
      comm_groups[comm_to_index(replay_ptr, MPI_COMM_WORLD)] = group;
      break;

      /* communicator ops */
    case umpi_MPI_Comm_create:
      comm_out = replay_Comm_create(e, NULL);
      MPI_Comm_group(comm_out, &group);
      comm_groups[comm_to_index(replay_ptr, comm_out)] = group;
      break;

    case umpi_MPI_Comm_dup:
      comm_out = replay_Comm_dup(e, NULL);
      MPI_Comm_group(comm_out, &group);
      comm_groups[comm_to_index(replay_ptr, comm_out)] = group;
      break;

    case umpi_MPI_Comm_free:
      /* we do not replay MPI_Comm_free here because we may
       * use the communicator later during the real replay */
      break;

    case umpi_MPI_Comm_split:
      comm_out = replay_Comm_split(e, NULL);
      MPI_Comm_group(comm_out, &group);
      comm_groups[comm_to_index(replay_ptr, comm_out)] = group;
      break;

      /* group ops */
    case umpi_MPI_Group_free:
      /* we do not replay MPI_Group_free here because we may
       * use the group later during the real replay */
      break;
    case umpi_MPI_Comm_group:
    case umpi_MPI_Group_difference:
    case umpi_MPI_Group_excl:
    case umpi_MPI_Group_incl:
    case umpi_MPI_Group_intersection:
    case umpi_MPI_Group_range_excl:
    case umpi_MPI_Group_range_incl:
    case umpi_MPI_Group_union:
      switch_event(op_code, e, NULL);
      break;

    default:
      break;
    }
  }
  if(myContext)
    delete myContext;
}

void destroyCommunicators(){
  map<int, MPI_Group>::iterator it;
  for(it=comm_groups.begin(); it!=comm_groups.end(); it++)
    MPI_Group_free(&(it->second));
  finalize_ptr(replay_ptr);
  replay_ptr = NULL;
}

void setupDestSet(Trace* trace){
  trace->resetTraversalIterators();
  Context *myContext =  new Context(trace->getHead(), trace->getTail(), my_rank, seed + my_rank, dest_limit);
  Event *e;
  int op_code;
  int dest, comm_id;

  while(myContext->hasNext()){
    e = myContext->next();
    op_code = e->getOp();

    switch(op_code){
    case umpi_MPI_Send:
    case umpi_MPI_Isend:
      comm_id = e->getParamValueForRank(COMM, my_rank);
      dest = e->getParamValueForRank(DEST, my_rank, myContext->getRandMnger());
      dest = DECODE_DEST(dest);
      dest = translateRank(dest, comm_id);
      if(dest != my_rank){
        destSet.insert(dest);
#ifdef DEBUG
        scounts[dest] = 0;
#endif
        stags[dest] = 0;
      }
      break;
    default:
      break;
    }
  }

  if(my_rank == doTestFor){
    cout << "Rank " << my_rank << ": dest set size = " << destSet.size() << endl;
    for(set<int>::iterator it = destSet.begin(); it!=destSet.end(); it++)
      cout << *it << "  ";
    cout << endl;
  }

  if(myContext)
    delete myContext;
}

void notifyPeers(){
  int sbuf[my_size];
  int rbuf[my_size];
  memset(sbuf, 0, my_size * sizeof(int) );
  memset(rbuf, 0, my_size * sizeof(int) );
  int srcSetSize;
  int maxSetSize;
  int minSetSize;

  set<int>::iterator it;
  for(it = destSet.begin(); it != destSet.end(); it++)
    sbuf[*it] = 1;

  MPI_Alltoall(sbuf, 1, MPI_INT, rbuf, 1, MPI_INT, MPI_COMM_WORLD);

  for(int i=0; i<my_size; i++)
    if(rbuf[i] == 1)
      srcSet.insert(i);

  if(my_rank == doTestFor){
    cout << "Rank " << my_rank << ": source set size = " << srcSet.size() << endl;
    for(set<int>::iterator it = srcSet.begin(); it!=srcSet.end(); it++)
      cout << *it << "  ";
    cout << endl;
  }
  srcSetSize = srcSet.size();
  PMPI_Reduce(&srcSetSize, &maxSetSize, 1, MPI_INT, MPI_MAX, doTestFor, MPI_COMM_WORLD);
  PMPI_Reduce(&srcSetSize, &minSetSize, 1, MPI_INT, MPI_MIN, doTestFor, MPI_COMM_WORLD);
  if(my_rank == doTestFor)
    cout << "Source set sizes: max = " << maxSetSize << ", min = " << minSetSize << endl;

}


void setupPeerContexts(Trace* trace){
  set<int>::iterator it;
  for(it = srcSet.begin(); it != srcSet.end(); it++){
    peerContexts[*it] = new Context(trace->getHead(), trace->getTail(), *it, seed + *it, dest_limit);
#ifdef DEBUG
    rcounts[*it] = 0;
#endif
    rtags[*it] = 0;
  }
}


void destroyPeerContexts(){
  map<int, Context*>::iterator it;
  for(it = peerContexts.begin(); it != peerContexts.end(); it++)
    if(it->second)
      delete it->second;
}

void prepareRecvs(){
  map<int, Context*>::iterator it;
  sendinfo_t* info = NULL;
  for(it = peerContexts.begin(); it != peerContexts.end(); it++){
    info = getNextSendForRank(it->first);
    if(info)
      pending_recvs.push(info);
  }
}

void postRecvs(Timer *global_timer, bool doPost){
  long long int total = global_timer->getTime();
  sendinfo_t* info = NULL;
  MPI_Comm comm;
  MPI_Datatype datatype;
  MPI_Request request;
  int datatype_size, src, ret, abs_src;
  Event *e;
  void *recvbuf;

  while(!pending_recvs.empty()){
    info = pending_recvs.top();
    if(info && info->time <= total){
      pending_recvs.pop();
      e = info->e;
      abs_src = info->src;
#ifdef DEBUG
      rcounts[abs_src]++;
#endif
      // issue recv
      comm = (MPI_Comm)index_to_comm (replay_ptr, info->comm);
      datatype = index_to_type (replay_ptr, info->datatype);
      if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
        cerr << "MPI_Irecv error: MPI_Type_size failed - probably invalid type: "
            << info->datatype << endl;
        exit(0);
      }
      //recvbuf = malloc(info->count * datatype_size);
      recvbuf = handleQueue->getRecvBuffer(info->count * datatype_size);
      if(!recvbuf) {
        cerr << "malloc MPI_Irecv buffer" << endl;
        exit(0);
      }
      src = translateRank(abs_src, comm_to_index(replay_ptr, MPI_COMM_WORLD), info->comm);

      ret = MPI_Irecv(recvbuf, info->count, datatype, src, info->tag, comm, &request);

      handleQueue->add(request, recvbuf);
      if(ret != MPI_SUCCESS) {
        cerr << "error: MPI_Irecv did not return MPI_SUCCESS" << endl;
        exit(0);
      }

      delete info;
      if(doPost && abs_src != my_rank){
        info = getNextSendForRank(abs_src);
        if(info)
          pending_recvs.push(info);
      }
      info = NULL;
    } else {
      break;
    }
  }
}

void* recvHandler(void *timer){
  Timer *global_timer = (Timer *)timer;
  postRecvs(global_timer, false);
  prepareRecvs();
  return NULL;
}

void* testCompletion(void *handleQueue){
  HandleQueue *h = (HandleQueue*) handleQueue;
  if(h)
    h->testCompletion();
  return NULL;
}

#ifdef DEBUG
void printMsgCounts(){
  int total = 0;
  for(map<int, int>::iterator it=scounts.begin(); it!=scounts.end(); it++)
    total += it->second;
  cout << "Msgs sent: " << total << endl;
  total = 0;
  for(map<int, int>::iterator it=rcounts.begin(); it!=rcounts.end(); it++)
    total += it->second;
  cout << "Msgs received: " << total << endl << endl;
}
#endif

void histo_replay(Trace* trace){
  trace->resetTraversalIterators();
  Timer *global_timer = new Timer();
  compute_t compute;
  compute.func = &recvHandler;
  compute.param = (void *)global_timer;
  Context *myContext = new Context(trace->getHead(), trace->getTail(), my_rank, seed + my_rank, dest_limit, &compute);
  Event *e;
  int op_code;
  map<int, Context*>::iterator it;
  Timer *replay_timer = myContext->getTimer();

  MPI_Comm comm;
  MPI_Datatype datatype = 0;
  MPI_Request request;

  int comm_id, datatype_id, datatype_size, count, dest, tag, ret = MPI_ERR_OTHER;
  void *sendbuf = 0;
  int i = 0;

  compute_t reqTester;

  while(myContext->hasNext()){
    e = myContext->next();
    op_code = e->getOp();
    replay_timer->setCurrentEvent(e);

#ifdef DEBUG
    if( i % 100 == 0 && my_rank == doTestFor )
      printMsgCounts();
#endif

    switch(op_code){
    case umpi_MPI_Init:
      handleQueue = new HandleQueue();
      reqTester.func = &testCompletion;
      reqTester.param = (void *)handleQueue;
      sendinfo_t *info;
      for(it = peerContexts.begin(); it != peerContexts.end(); it++){
        info = getNextSendForRank(it->first);
        pending_recvs.push(info);
      }
      replay_timer->setTime();
      global_timer->setTime();
      break;
    case umpi_MPI_Finalize:
      // issue recvs for all the sends
      global_timer->resetTime();
      //recvHandler((void *)global_timer);
#ifdef DEBUG
      if(my_rank == doTestFor){
        cout << "Rank " << my_rank << " Finalizing..." << endl;
        printMsgCounts();
      }
#endif
      postRecvs(global_timer, true);
#ifdef DEBUG
      if(my_rank == doTestFor)
        printMsgCounts();
      /* Uncomment the following loops for more debugging
				for(map<int, int>::iterator it=scounts.begin(); it!=scounts.end(); it++)
					cout << my_rank << " sent " << it->second << " msgs to " << it->first << endl;
				for(map<int, int>::iterator it=rcounts.begin(); it!=rcounts.end(); it++)
					cout << my_rank << " received " << it->second << " msgs from " << it->first << endl;
       */
#endif
      delete handleQueue;
      break;

      /* we don't replay communicator ops,
       * because communicators are already built during preprocessing,
       * but we do replay the computations before them */
    case umpi_MPI_Comm_create:
    case umpi_MPI_Comm_dup:
    case umpi_MPI_Comm_free:
      /*comm_id = e->getParamValueForRank(COMM, my_rank);
				comm_groups.erase(comm_id);
				replay_Comm_free(e, replay_timer);
				break;*/
    case umpi_MPI_Comm_split:
      replay_timer->simulateComputeTime();
      replay_timer->setTime();
      break;

      /* replay send ops, blocking send ops are replaced with non-blocking ones*/
    case umpi_MPI_Send:
    case umpi_MPI_Isend:
      comm_id = e->getParamValueForRank(COMM, my_rank);
      comm = (MPI_Comm)index_to_comm (replay_ptr, comm_id);
      datatype_id = e->getParamValueForRank(DATATYPE, my_rank);
      datatype = index_to_type (replay_ptr, datatype_id);
      if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
        cerr << "MPI_Isend error: MPI_Type_size failed - probably invalid type: "
            << datatype_id << endl;
        exit(0);
      }
      count = e->getParamValueForRank(COUNT, my_rank);
      //sendbuf = malloc(count * datatype_size);
      sendbuf = handleQueue->getSendBuffer(count * datatype_size);
      if(!sendbuf) {
        cerr << "malloc MPI_Isend buffer" << endl;
        exit(0);
      }
      dest = e->getParamValueForRank(DEST, my_rank, myContext->getRandMnger());
      dest = DECODE_DEST (dest);
      //tag = e->getParamValueForRank(TAG, my_rank);
      tag = stags[translateRank(dest, comm_id)];
      stags[translateRank(dest, comm_id)]++;

      if(op_code == umpi_MPI_Send)
        replay_timer->simulateComputeTime(true, &reqTester);
      else
        replay_timer->simulateComputeTime();
      ret = MPI_Isend(sendbuf, count, datatype, dest, tag, comm, &request);
      replay_timer->setTime();

      handleQueue->add(request, sendbuf);
      if(ret != MPI_SUCCESS) {
        cerr << "error: MPI_Isend did not return MPI_SUCCESS" << endl;
        exit(0);
      }
#ifdef DEBUG
      scounts[translateRank(dest, comm_id)]++;
#endif

      if(translateRank(dest, comm_id) == my_rank){
        sendinfo_t *info = new sendinfo_t;
        info->time = 0; //post the recv immediately
        info->e = e;
        info->count = count;
        info->datatype = datatype_id;
        info->src = my_rank;
        info->tag = tag;
        info->comm = comm_id;
        pending_recvs.push(info);
      }
      break;

      /* we don't replay receive ops, but we do replay the computations before them */
    case umpi_MPI_Recv:
    case umpi_MPI_Irecv:
      postRecvs(global_timer, true);
      if(op_code == umpi_MPI_Recv)
        replay_timer->simulateComputeTime(true, &reqTester);
      else
        replay_timer->simulateComputeTime();
      replay_timer->setTime();
      break;

      /* we don't replay wait ops, but we do replay the computations before them */
    case umpi_MPI_Wait:
    case umpi_MPI_Waitall:
      if(i++ % 50 == 0){
        if(handleQueue)
          handleQueue->testCompletion();
      }
      replay_timer->simulateComputeTime(true, &reqTester);
      replay_timer->setTime();
      break;

      /* others: collectives, groups ops, etc */
    default:
      switch_event(op_code, e, replay_timer);
      break;
    }
  }

  if(myContext)
    delete myContext;
  if(global_timer)
    delete global_timer;
}

sendinfo_t* getNextSendForRank(int rank){
  map<int, Context*>::iterator it = peerContexts.find(rank);
  Context* srcContext;
  if(it == peerContexts.end()){
    cerr << "error: cannot find the traversal context for rank " << rank << endl;
    return NULL;
  }
  srcContext = it->second;
  Timer *srcTimer = srcContext->getTimer();
  Event *e;
  int op_code;
  int count, datatype, dest, tag, comm;

  while(srcContext->hasNext()){
    e = srcContext->next();
    srcTimer->setCurrentEvent(e);
    srcTimer->accumulateCompTime();
    op_code = e->getOp();

    switch(op_code){
    case umpi_MPI_Send:
    case umpi_MPI_Isend:
      comm = e->getParamValueForRank(COMM, rank);
      dest = e->getParamValueForRank(DEST, rank, srcContext->getRandMnger());
      dest = DECODE_DEST_FOR_RANK(dest, rank);
      dest = translateRank(dest, comm);
      count = e->getParamValueForRank(COUNT, rank);
      datatype = e->getParamValueForRank(DATATYPE, rank);
      //tag = e->getParamValueForRank(TAG, rank);
      if(dest == my_rank){
        sendinfo_t *info = new sendinfo_t;
        info->time = srcTimer->getAccumulatedTime();
#ifdef DEBUG
        //if(my_rank == doTestFor && rank == *(srcSet.begin()))
        //	cout << my_rank << " receives from " << rank << " at time: " << info->time << endl;
#endif
        info->e = e;
        info->count = count;
        info->datatype = datatype;
        info->src = rank;
        tag = rtags[rank];
        rtags[rank]++;
        info->tag = tag;
        info->comm = comm;
        srcTimer->accumulateCommTime();
        return info;
      }
      break;
    default:
      break;
    }
    srcTimer->accumulateCommTime();
  }

  return NULL;
}



