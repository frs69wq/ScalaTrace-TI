/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include <iostream>      // Added by Henri so that smpicxx can compile this

#include "handle_queue.h"

#ifdef FEATURE_PREALLOC_MEM
HandleQueue::HandleQueue()
{
  if(my_rank == doTestFor)
    cout << "Pre-allocating " << CHUNK_SIZE << " bytes sendbuffer and recvbuffer" << endl;
  sendbuffer = (void *)malloc(CHUNK_SIZE);
  if(!sendbuffer){
    cout << "error: cannot pre-allocate sendbuffer: " << CHUNK_SIZE << endl;
    exit(0);
  }
  recvbuffer = (void *)malloc(CHUNK_SIZE);
  if(!recvbuffer){
    cout << "error: cannot pre-allocate recvbuffer: " << CHUNK_SIZE << endl;
    exit(0);
  }

  sbufsize = CHUNK_SIZE;
  rbufsize = CHUNK_SIZE;
}
#else
HandleQueue::HandleQueue():
    sbufsize(0),
    sendbuffer(NULL),
    rbufsize(0),
    recvbuffer(NULL)
{}
#endif

HandleQueue::~HandleQueue(){
  while(requests.size() > 0){
    /*if(my_rank == 0)
			cout << requests.size() << " " << buffers.size() << endl;*/
    testsingleCompletion();
  }
#ifdef FEATURE_PREALLOC_MEM
  if(sendbuffer)
    free(sendbuffer);
  if(recvbuffer)
    free(recvbuffer);
#endif
}

void *HandleQueue::getSendBuffer(int size){
#ifdef FEATURE_PREALLOC_MEM
  if(size > sbufsize){
    cerr << "error: requesting " << size << " bytes, larger than existing " << sbufsize << " bytes" << endl;
    exit(0);
  }
  return sendbuffer;
#else
  sendbuffer = (void *)malloc(size);
  if(!sendbuffer){
    cerr << "error: cannot allocate memory: " << size << " bytes" << endl;
    exit(0);
  }
  return sendbuffer;
#endif
}

void *HandleQueue::getRecvBuffer(int size){
#ifdef FEATURE_PREALLOC_MEM
  if(size > rbufsize){
    cerr << "error: requesting " << size << " bytes, larger than existing " << rbufsize << " bytes" << endl;
    exit(0);
  }
  return recvbuffer;
#else
  recvbuffer = (void *)malloc(size);
  if(!recvbuffer){
    cerr << "error: cannot allocate memory: " << size << " bytes" << endl;
    exit(0);
  }
  return recvbuffer;
#endif
}

void HandleQueue::testallCompletion(){
  int size, count, flag, begin = 0, upper;
  MPI_Request *req_arr;
  vector<MPI_Request>::iterator req_it;
  vector<void *>::iterator buf_it;

  size = requests.size();
  if(size >= 0){
    req_it = requests.begin();
    buf_it = buffers.begin();
    req_arr = &(requests[0]);

    for(begin = (size / BATCH_SIZE) * BATCH_SIZE; begin >= 0; begin -= BATCH_SIZE){
      count = size - begin > BATCH_SIZE ? BATCH_SIZE : size - begin;
      MPI_Testall(count, req_arr + begin, &flag, MPI_STATUSES_IGNORE);
      if(flag == true){
        upper = begin + count;
        for(int i = begin; i < upper; i++){
          if(buffers[i] != NULL)
            free(buffers[i]);
        }
        requests.erase(req_it + begin, req_it + upper);
        buffers.erase(buf_it + begin, buf_it + upper);
      }
    }
  }
#ifdef DEBUG
  if(my_rank == 0)
    cout << requests.size() << endl;
#endif
}

void HandleQueue::testsomeCompletion(){
  int incount = requests.size(), outcount, index;
  MPI_Request *array_of_requests;
  int *array_of_indices;
  MPI_Status *array_of_statuses;
  vector<MPI_Request>::iterator req_it;
  vector<void *>::iterator buf_it;

  if(incount > 0){
    array_of_requests = &(requests[0]);
    array_of_indices = (int *)malloc(incount * sizeof(int));
    array_of_statuses = (MPI_Status *)malloc(incount * sizeof(MPI_Status));

    MPI_Testsome(incount, array_of_requests, &outcount, array_of_indices, array_of_statuses);

    if(outcount > 0){
      req_it = requests.begin();
      buf_it = buffers.begin();
      for(int i=outcount - 1; i>=0; i--){
        index = array_of_indices[i];
        if(buffers[index] != NULL){
          free(buffers[index]);
        }
        requests.erase(req_it + index);
        buffers.erase(buf_it + index);
      }
    }
    free(array_of_indices);
    free(array_of_statuses);
  }

#ifdef DEBUG
  if(my_rank == 0)
    cout << requests.size() << endl;
#endif
}


void HandleQueue::testsingleCompletion(){
  //return;
  //assert(requests.size() == buffers.size());
  int flag;
  MPI_Status status;
  vector<MPI_Request>::iterator req_it;
  vector<void *>::iterator buf_it;

#ifdef DEBUG
  if(my_rank == doTestFor)
    cout << requests.size() << "  ";
#endif

  for(req_it=requests.begin(), buf_it=buffers.begin(); req_it!=requests.end() && buf_it!=buffers.end(); ){
    MPI_Test(&(*req_it), &flag, &status);
    if(flag == true && *req_it == MPI_REQUEST_NULL){
      req_it = requests.erase(req_it);
      if(*buf_it != NULL){
        free(*buf_it);
      }
      buf_it = buffers.erase(buf_it);
    } else {
      req_it++;
      buf_it++;
    }
  }

#ifdef DEBUG
  if(my_rank == doTestFor)
    cout << requests.size() << endl;
#endif
}


void HandleQueue::testCompletion(){
  testsingleCompletion();
}

void HandleQueue::add(MPI_Request request, void *buf){
  requests.push_back(request);
#ifdef FEATURE_PREALLOC_MEM
  buf = NULL;
#endif
  buffers.push_back(buf);
  assert(requests.size() == buffers.size());
}
