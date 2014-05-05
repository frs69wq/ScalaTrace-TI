/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include <iostream>      // Added by Henri so that smpicxx can compile this


#include "op_exec.h"

#define RECORDED_MPI_REQUEST_NULL -1

extern int my_rank, my_size;

ptr_handler_t replay_ptr = NULL;
req_handler_t replay_req = NULL;

Timer *replay_timer;

/* collectives */
void replay_Allgather(Event *e, Timer *timer){
  MPI_Comm comm;
  MPI_Datatype datatype, recvtype;
  int datatype_size, recvtype_size, count, recvcount, ret, comm_size;
  void *sendbuf, *recvbuf;

  comm = (MPI_Comm)index_to_comm (replay_ptr, e->getParamValueForRank(COMM, my_rank));
  MPI_Comm_size(comm, &comm_size);

  datatype = index_to_type (replay_ptr, e->getParamValueForRank(DATATYPE, my_rank));
  if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
    cerr << "MPI_Allgather error: MPI_Type_size failed - probably invalid type: "
        << e->getParamValueForRank(DATATYPE, my_rank) << endl;
    exit(0);
  }
  recvtype = index_to_type (replay_ptr, e->getParamValueForRank(RECVTYPE, my_rank));
  if(MPI_Type_size(recvtype, &recvtype_size) != MPI_SUCCESS) {
    cerr << "MPI_Allgather error: MPI_Type_size failed - probably invalid type: "
        << e->getParamValueForRank(RECVTYPE, my_rank) << endl;
    exit(0);
  }

  count = e->getParamValueForRank(COUNT, my_rank);
  sendbuf = malloc(count * datatype_size);

  recvcount = e->getParamValueForRank(RECVCOUNT, my_rank);
  recvbuf = malloc(recvcount * recvtype_size * comm_size);

  if(!sendbuf || !recvbuf) {
    cerr << "MPI_Allgather error: cannot malloc buffers" << endl;
    exit(0);
  }

  /* MPI call */
  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Allgather(sendbuf, count, datatype, recvbuf, recvcount, recvtype, comm);
  if(timer)
    timer->setTime();
  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Allgather did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  if(sendbuf)
    free(sendbuf);
  if(recvbuf)
    free(recvbuf);
}

void replay_Allreduce(Event *e, Timer *timer){
  MPI_Comm comm;
  MPI_Datatype datatype;
  MPI_Op mpi_op;
  int datatype_size, count, ret;
  void *sendbuf, *recvbuf;

  comm = (MPI_Comm)index_to_comm (replay_ptr, e->getParamValueForRank(COMM, my_rank));

  datatype = index_to_type (replay_ptr, e->getParamValueForRank(DATATYPE, my_rank));
  if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
    cerr << "MPI_Allreduce error: MPI_Type_size failed - probably invalid type: "
        << e->getParamValueForRank(DATATYPE, my_rank) << endl;
    exit(0);
  }

  count = e->getParamValueForRank(COUNT, my_rank);

  recvbuf = malloc(count * datatype_size);
  sendbuf = malloc(count * datatype_size);
  if(!sendbuf || !recvbuf) {
    cerr << "MPI_Allreduce error: cannot malloc buffers" << endl;
    exit(0);
  }

  mpi_op = index_to_op (replay_ptr, e->getParamValueForRank(MPI_OP, my_rank) );

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Allreduce(sendbuf, recvbuf, count, datatype, mpi_op, comm);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Allreduce did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  if(sendbuf)
    free(sendbuf);
  if(recvbuf)
    free(recvbuf);
}

void replay_Alltoall(Event *e, Timer *timer){
  MPI_Comm comm;
  MPI_Datatype datatype, recvtype;
  int datatype_size, recvtype_size, count, recvcount, ret, comm_size;
  void *sendbuf, *recvbuf;

  comm = (MPI_Comm)index_to_comm (replay_ptr, e->getParamValueForRank(COMM, my_rank));
  MPI_Comm_size(comm, &comm_size);

  datatype = index_to_type (replay_ptr, e->getParamValueForRank(DATATYPE, my_rank));
  if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
    cerr << "MPI_Alltoall error: MPI_Type_size failed - probably invalid type: "
        << e->getParamValueForRank(DATATYPE, my_rank) << endl;
    exit(0);
  }
  recvtype = index_to_type (replay_ptr, e->getParamValueForRank(RECVTYPE, my_rank));
  if(MPI_Type_size(recvtype, &recvtype_size) != MPI_SUCCESS) {
    cerr << "MPI_Alltoall error: MPI_Type_size failed - probably invalid type: "
        << e->getParamValueForRank(RECVTYPE, my_rank) << endl;
    exit(0);
  }

  count = e->getParamValueForRank(COUNT, my_rank);
  sendbuf = malloc(count * datatype_size * comm_size);

  recvcount = e->getParamValueForRank(RECVCOUNT, my_rank);
  recvbuf = malloc(recvcount * recvtype_size * comm_size);

  if(!sendbuf || !recvbuf) {
    cerr << "MPI_Alltoall error: cannot malloc buffers" << endl;
    exit(0);
  }

  /* MPI call */
  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Alltoall(sendbuf, count, datatype, recvbuf, recvcount, recvtype, comm);
  if(timer)
    timer->setTime();
  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Alltoall did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  if(sendbuf)
    free(sendbuf);
  if(recvbuf)
    free(recvbuf);
}

void replay_Alltoallv(Event *e, Timer *timer){
}

void replay_Barrier(Event *e, Timer *timer){
  MPI_Comm comm;
  int ret;

  comm = (MPI_Comm)index_to_comm (replay_ptr, e->getParamValueForRank(COMM, my_rank));
  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Barrier(comm);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Barrier did not return MPI_SUCCESS" << endl;
    exit(0);
  }
}

void replay_Bcast(Event *e, Timer *timer){
  MPI_Comm comm;
  MPI_Datatype datatype;
  int datatype_size, count, ret, root;
  void *sendbuf;

  comm = (MPI_Comm)index_to_comm (replay_ptr, e->getParamValueForRank(COMM, my_rank));

  datatype = index_to_type (replay_ptr, e->getParamValueForRank(DATATYPE, my_rank));
  if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
    cerr << "MPI_Bcast error: MPI_Type_size failed - probably invalid type: "
        << e->getParamValueForRank(DATATYPE, my_rank) << endl;
    exit(0);
  }

  count = e->getParamValueForRank(COUNT, my_rank);
  root = e->getParamValueForRank(RT, my_rank);

  sendbuf = malloc(count * datatype_size);
  if(!sendbuf) {
    cerr << "MPI_Bcast error: cannot malloc buffer" << endl;
    exit(0);
  }

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Bcast(sendbuf, count, datatype, root, comm);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Bcast did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  if(sendbuf)
    free(sendbuf);
}

void replay_Reduce(Event *e, Timer *timer){
  MPI_Comm comm;
  MPI_Datatype datatype;
  MPI_Op mpi_op;
  int datatype_size, count, ret, root;
  void *sendbuf, *recvbuf;

  comm = (MPI_Comm)index_to_comm (replay_ptr, e->getParamValueForRank(COMM, my_rank));

  datatype = index_to_type (replay_ptr, e->getParamValueForRank(DATATYPE, my_rank));
  if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
    cerr << "MPI_Reduce error: MPI_Type_size failed - probably invalid type: "
        << e->getParamValueForRank(DATATYPE, my_rank) << endl;
    exit(0);
  }

  count = e->getParamValueForRank(COUNT, my_rank);
  sendbuf = malloc(count * datatype_size);
  if(!sendbuf) {
    cerr << "MPI_Reduce error: cannot malloc send buffer" << endl;
    exit(0);
  }

  root = e->getParamValueForRank(RT, my_rank);
  if(my_rank == root){
    recvbuf = malloc(count * datatype_size);
    if(!recvbuf) {
      cerr << "MPI_Reduce error: cannot malloc recv buffer" << endl;
      exit(0);
    }
  } else {
    recvbuf = NULL;
  }

  mpi_op = index_to_op (replay_ptr, e->getParamValueForRank(MPI_OP, my_rank) );

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Reduce(sendbuf, recvbuf, count, datatype, mpi_op, root, comm);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Reduce did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  if(sendbuf)
    free(sendbuf);
  if(recvbuf)
    free(recvbuf);
}


/* the following functions make the assumption that the input trace
 * is node-level, so these functions use Event::getCurrentValue for
 * a better performance */

/* point-to-point */
void replay_Irecv(Event* e, Timer *timer){
  MPI_Comm comm;
  MPI_Datatype datatype = 0;
  MPI_Request request;
  int datatype_size, count, src, ret, tag;
  void *recvbuf = 0;

  comm = (MPI_Comm)index_to_comm (replay_ptr, e->getCurrentValue(COMM));
  datatype = index_to_type (replay_ptr, e->getCurrentValue(DATATYPE));
  if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
    cerr << "MPI_Irecv error: MPI_Type_size failed - probably invalid type: "
        << e->getCurrentValue(DATATYPE) << endl;
    exit(0);
  }
  count = e->getCurrentValue(COUNT);
  recvbuf = malloc(count * datatype_size);
  if(!recvbuf) {
    cerr << "malloc MPI_Irecv buffer" << endl;
    exit(0);
  }
  add_mem_entry(recvbuf);
  src = e->getCurrentValue(SOURCE);
  src = DECODE_SOURCE (src);
  tag = e->getCurrentValue(TAG);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Irecv(recvbuf, count, datatype, src, tag, comm, &request);
  if(timer)
    timer->setTime();

  add_request_entry(replay_req, request);
  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Irecv did not return MPI_SUCCESS" << endl;
    exit(0);
  }

}

void replay_Recv(Event* e, Timer *timer){
  MPI_Comm comm;
  MPI_Datatype datatype = 0;
  MPI_Status status;
  int datatype_size, count, src, ret, tag;
  void *recvbuf = 0;

  comm = (MPI_Comm)index_to_comm (replay_ptr, e->getCurrentValue(COMM));
  datatype = index_to_type (replay_ptr, e->getCurrentValue(DATATYPE));
  if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
    cerr << "MPI_Recv error: MPI_Type_size failed - probably invalid type: "
        << e->getCurrentValue(DATATYPE) << endl;
    exit(0);
  }
  count = e->getCurrentValue(COUNT);
  recvbuf = malloc(count * datatype_size);
  if(!recvbuf) {
    cerr << "malloc MPI_Recv buffer" << endl;
    exit(0);
  }
  src = e->getCurrentValue(SOURCE);
  src = DECODE_SOURCE (src);
  tag = e->getCurrentValue(TAG);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Recv(recvbuf, count, datatype, src, tag, comm, &status);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Recv did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  if(recvbuf)
    free(recvbuf);

}

void replay_Isend(Event *e, Timer *timer){
  MPI_Comm comm;
  MPI_Datatype datatype = 0;
  MPI_Request request;
  int datatype_size, count, dest, ret, tag;
  void *sendbuf = 0;

  comm = (MPI_Comm)index_to_comm (replay_ptr, e->getCurrentValue(COMM));
  datatype = index_to_type (replay_ptr, e->getCurrentValue(DATATYPE));
  if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
    cerr << "MPI_Isend error: MPI_Type_size failed - probably invalid type: "
        << e->getCurrentValue(DATATYPE) << endl;
    exit(0);
  }
  count = e->getCurrentValue(COUNT);
  sendbuf = malloc(count * datatype_size);
  if(!sendbuf) {
    cerr << "malloc MPI_Isend buffer" << endl;
    exit(0);
  }
  add_mem_entry(sendbuf);
  dest = e->getCurrentValue(DEST);
  dest = DECODE_DEST (dest);
  tag = e->getCurrentValue(TAG);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Isend(sendbuf, count, datatype, dest, tag, comm, &request);
  if(timer)
    timer->setTime();

  add_request_entry(replay_req, request);
  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Isend did not return MPI_SUCCESS" << endl;
    exit(0);
  }

}

void replay_Send(Event *e, Timer *timer){
  MPI_Comm comm;
  MPI_Datatype datatype = 0;
  int datatype_size, count, dest, ret, tag;
  void *sendbuf = 0;

  comm = (MPI_Comm)index_to_comm (replay_ptr, e->getCurrentValue(COMM));
  datatype = index_to_type (replay_ptr, e->getCurrentValue(DATATYPE));
  if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
    cerr << "MPI_Isend error: MPI_Type_size failed - probably invalid type: "
        << e->getCurrentValue(DATATYPE) << endl;
    exit(0);
  }
  count = e->getCurrentValue(COUNT);
  sendbuf = malloc(count * datatype_size);
  if(!sendbuf) {
    cerr << "malloc MPI_Send buffer" << endl;
    exit(0);
  }
  dest = e->getCurrentValue(DEST);
  dest = DECODE_DEST (dest);
  tag = e->getCurrentValue(TAG);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Send(sendbuf, count, datatype, dest, tag, comm);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Send did not return MPI_SUCCESS" << endl;
    exit(0);
  }
  if(sendbuf)
    free(sendbuf);
}

/* Communicator ops */

MPI_Comm replay_Comm_create(Event *e, Timer *timer){
  MPI_Comm comm, comm_out;
  MPI_Group group;
  int comm_id, group_id, ret;

  comm_id = e->getParamValueForRank(COMM, my_rank);
  comm = (MPI_Comm)index_to_comm(replay_ptr, comm_id);

  group_id = e->getParamValueForRank(GROUP, my_rank);
  group = (MPI_Group)index_to_group (replay_ptr, group_id);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Comm_create(comm, group, &comm_out);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Comm_create did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  add_comm_entry(replay_ptr, comm_out);

  return comm_out;
}

MPI_Comm replay_Comm_dup(Event *e, Timer* timer){
  MPI_Comm comm, comm_out;
  int comm_id, ret;

  comm_id = e->getParamValueForRank(COMM, my_rank);
  comm = (MPI_Comm)index_to_comm(replay_ptr, comm_id);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Comm_dup(comm, &comm_out);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS){
    cerr << "error: MPI_Comm_dup did not return MPI_SUCCESS" << endl;
    exit(0);
  }
  add_comm_entry(replay_ptr, comm_out);

  return comm_out;
}

void replay_Comm_free(Event *e, Timer *timer){
  MPI_Comm comm;
  int comm_id, ret;

  comm_id = e->getParamValueForRank(COMM, my_rank);
  comm = (MPI_Comm)index_to_comm(replay_ptr, comm_id);

  remove_comm_entry(replay_ptr, comm);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Comm_free(&comm);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS){
    cerr << "error: MPI_Comm_free did not return MPI_SUCCESS" << endl;
    exit(0);
  }
}


MPI_Comm replay_Comm_split(Event *e, Timer *timer){
  MPI_Comm comm, comm_out;
  int comm_id, ret, color, key;

  comm_id = e->getParamValueForRank(COMM, my_rank);
  comm = (MPI_Comm)index_to_comm(replay_ptr, comm_id);

  color = e->getParamValueForRank(COLOR, my_rank);
  key = e->getParamValueForRank(KEY, my_rank);
  key = DECODE_KEY(key);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Comm_split(comm, color, key, &comm_out);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS){
    cerr << "error: MPI_Comm_split did not return MPI_SUCCESS" << endl;
    exit(0);
  }
  add_comm_entry(replay_ptr, comm_out);

  return comm_out;
}



/* group ops */

MPI_Group replay_Comm_group(Event *e, Timer *timer){
  MPI_Comm comm;
  MPI_Group group;
  int comm_id, ret;

  comm_id = e->getParamValueForRank(COMM, my_rank);
  comm = (MPI_Comm)index_to_comm(replay_ptr, comm_id);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Comm_group(comm, &group);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS){
    cerr << "error: MPI_Comm_group did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  add_group_entry(replay_ptr, group);

  return group;
}

MPI_Group replay_Group_difference(Event *e, Timer *timer){
  MPI_Group group1, group2, group_out;
  int ret, group_id1, group_id2;

  group_id1 = e->getParamValueForRank(GROUP1, my_rank);
  group1 = (MPI_Group)index_to_group (replay_ptr, group_id1);
  group_id2 = e->getParamValueForRank(GROUP2, my_rank);
  group2 = (MPI_Group)index_to_group (replay_ptr, group_id2);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Group_difference(group1, group2, &group_out);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS){
    cerr << "error: MPI_Group_difference did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  add_group_entry(replay_ptr, group_out);

  return group_out;
}

MPI_Group replay_Group_excl(Event *e, Timer *timer){
  MPI_Group group, group_out;
  int ret, group_id, count, *ranks;

  group_id = e->getParamValueForRank(GROUP, my_rank);
  group = (MPI_Group)index_to_group (replay_ptr, group_id);

  count = e->getParamValueForRank(COUNT, my_rank);

  ranks = e->getParamValuesForRank(ARRAY_OF_RANKS, count, my_rank);
  if(ranks == NULL){
    cerr << "error: MPI_Group_excl cannot find ranks" << endl;
    exit(0);
  }

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Group_excl(group, count, ranks, &group_out);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Group_excl did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  add_group_entry(replay_ptr, group_out);

  if(ranks)
    free(ranks);

  return group_out;

}

void replay_Group_free(Event *e, Timer* timer){
  MPI_Group group;
  int group_id, ret;

  group_id = e->getParamValueForRank(GROUP, my_rank);
  group = (MPI_Group)index_to_group(replay_ptr, group_id);

  remove_group_entry(replay_ptr, group);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Group_free(&group);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Group_free did not return MPI_SUCCESS" << endl;
    exit(0);
  }

}


MPI_Group replay_Group_incl(Event *e, Timer *timer){
  MPI_Group group, group_out;
  int ret, group_id, count, *ranks;

  group_id = e->getParamValueForRank(GROUP, my_rank);
  group = (MPI_Group)index_to_group (replay_ptr, group_id);

  count = e->getParamValueForRank(COUNT, my_rank);

  ranks = e->getParamValuesForRank(ARRAY_OF_RANKS, count, my_rank);
  if(ranks == NULL){
    cerr << "error: MPI_Group_incl cannot find ranks" << endl;
    exit(0);
  }

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Group_incl(group, count, ranks, &group_out);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Group_incl did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  add_group_entry(replay_ptr, group_out);

  if(ranks)
    free(ranks);

  return group_out;

}

MPI_Group replay_Group_intersection(Event *e, Timer *timer){
  MPI_Group group1, group2, group_out;
  int ret, group_id1, group_id2;

  group_id1 = e->getParamValueForRank(GROUP1, my_rank);
  group1 = (MPI_Group)index_to_group (replay_ptr, group_id1);
  group_id2 = e->getParamValueForRank(GROUP2, my_rank);
  group2 = (MPI_Group)index_to_group (replay_ptr, group_id2);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Group_intersection(group1, group2, &group_out);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS){
    cerr << "error: MPI_Group_intersection did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  add_group_entry(replay_ptr, group_out);

  return group_out;
}

MPI_Group replay_Group_range_excl(Event *e, Timer *timer){
  typedef int triplet_t[3];
  MPI_Group group, group_out;
  int ret, group_id, count, *ranks;
  triplet_t *triplets;

  group_id = e->getParamValueForRank(GROUP, my_rank);
  group = (MPI_Group)index_to_group (replay_ptr, group_id);

  count = e->getParamValueForRank(COUNT, my_rank);

  ranks = e->getParamValuesForRank(RANGES, count * 3, my_rank);
  if(ranks == NULL){
    cerr << "error: MPI_Group_range_excl cannot find ranks" << endl;
    exit(0);
  }

  triplets = (triplet_t*)malloc(count * sizeof(triplet_t));
  if(triplets == NULL){
    cerr << "error: MPI_Group_range_excl cannot allocate buffer for triplets" << endl;
    exit(0);
  }
  for(int i=0; i<count; i++){
    for(int j=0; j<3; j++)
      triplets[i][j] = ranks[i*3 + j];
  }

  if(ranks)
    free(ranks);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Group_range_excl(group, count, triplets, &group_out);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Group_range_excl did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  add_group_entry(replay_ptr, group_out);

  if(triplets)
    free(triplets);

  return group_out;
}

MPI_Group replay_Group_range_incl(Event *e, Timer *timer){
  typedef int triplet_t[3];
  MPI_Group group, group_out;
  int ret, group_id, count, *ranks;
  triplet_t *triplets;

  group_id = e->getParamValueForRank(GROUP, my_rank);
  group = (MPI_Group)index_to_group (replay_ptr, group_id);

  count = e->getParamValueForRank(COUNT, my_rank);

  ranks = e->getParamValuesForRank(RANGES, count * 3, my_rank);
  if(ranks == NULL){
    cerr << "error: MPI_Group_range_incl cannot find ranks" << endl;
    exit(0);
  }

  triplets = (triplet_t*)malloc(count * sizeof(triplet_t));
  if(triplets == NULL){
    cerr << "error: MPI_Group_range_incl cannot allocate buffer for triplets" << endl;
    exit(0);
  }
  for(int i=0; i<count; i++){
    for(int j=0; j<3; j++)
      triplets[i][j] = ranks[i*3 + j];
  }

  if(ranks)
    free(ranks);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Group_range_incl(group, count, triplets, &group_out);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Group_range_incl did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  add_group_entry(replay_ptr, group_out);

  if(triplets)
    free(triplets);

  return group_out;
}

MPI_Group replay_Group_union(Event *e, Timer *timer){
  MPI_Group group1, group2, group_out;
  int ret, group_id1, group_id2;

  group_id1 = e->getParamValueForRank(GROUP1, my_rank);
  group1 = (MPI_Group)index_to_group (replay_ptr, group_id1);
  group_id2 = e->getParamValueForRank(GROUP2, my_rank);
  group2 = (MPI_Group)index_to_group (replay_ptr, group_id2);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Group_union(group1, group2, &group_out);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS){
    cerr << "error: MPI_Group_union did not return MPI_SUCCESS" << endl;
    exit(0);
  }

  add_group_entry(replay_ptr, group_out);

  return group_out;
}


/* Wait */
void replay_Wait(Event *e, Timer *timer){
  int offset, ret;
  MPI_Request request;
  MPI_Status status;

  offset = e->getCurrentValue(REQUEST);
  lookup_request( replay_req, offset, RECORDED_MPI_REQUEST_NULL, &request);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Wait(&request, &status);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Wait did not return MPI_SUCCESS" << endl;
    exit(0);
  }
  if (request == MPI_REQUEST_NULL)
    free_mem_entry(offset);
}

void replay_Waitall(Event *e, Timer *timer){
  int *offsets, count;
  MPI_Request *requests;
  MPI_Status *statuses;
  int ret;

  count = e->getCurrentValue(COUNT);
  offsets = e->getCurrentValues(ARRAY_OF_REQUESTS, count);
  if(offsets == NULL){
    cerr << "error: MPI_Waitall could not find the offsets" << endl;
    exit(0);
  }
  requests = (MPI_Request *) malloc(sizeof(MPI_Request) * count);
  statuses = (MPI_Status *) malloc(sizeof(MPI_Status) * count);

  lookup_requestlist( replay_req, offsets, count, RECORDED_MPI_REQUEST_NULL, requests);

  if(timer)
    timer->simulateComputeTime();
  ret = MPI_Waitall(count, requests, statuses);
  if(timer)
    timer->setTime();

  if(ret != MPI_SUCCESS) {
    cerr << "error: MPI_Waitall did not return MPI_SUCCESS" << endl;
    int x, length;
    char err[MPI_MAX_ERROR_STRING];
    for(x=0; x<count; x++){
      if(statuses[x].MPI_ERROR!=MPI_SUCCESS)
        MPI_Error_string(statuses[x].MPI_ERROR, err, &length);
      cerr <<  statuses[x].MPI_SOURCE << " " << err << endl;
    }
    exit(0);
  }

  for (int i = 0; i < count; i++) {
    if (requests[i] == MPI_REQUEST_NULL)
      free_mem_entry (offsets[i]);
  }
  free(requests);
  free(statuses);

  if(offsets != NULL)
    free(offsets);
}


void switch_event(int op_code, Event *e, Timer *timer){
  switch(op_code){
  /* collectives */
  case umpi_MPI_Allgather:
    replay_Allgather(e, timer);
    break;
  case umpi_MPI_Allreduce:
    replay_Allreduce(e, timer);
    break;
  case umpi_MPI_Alltoall:
    replay_Alltoall(e, timer);
    break;
  case umpi_MPI_Barrier:
    replay_Barrier(e, timer);
    break;
  case umpi_MPI_Bcast:
    replay_Bcast(e, timer);
    break;
  case umpi_MPI_Reduce:
    replay_Reduce(e, timer);
    break;

    /* communicator ops */
  case umpi_MPI_Comm_create:
    replay_Comm_create(e, timer);
    break;
  case umpi_MPI_Comm_dup:
    replay_Comm_dup(e, timer);
    break;
  case umpi_MPI_Comm_free:
    replay_Comm_free(e, timer);
    break;
  case umpi_MPI_Comm_split:
    replay_Comm_split(e, timer);
    break;

    /* group ops */
  case umpi_MPI_Comm_group:
    replay_Comm_group(e, timer);
    break;
  case umpi_MPI_Group_difference:
    replay_Group_difference(e, timer);
    break;
  case umpi_MPI_Group_excl:
    replay_Group_excl(e, timer);
    break;
  case umpi_MPI_Group_free:
    replay_Group_free(e, timer);
    break;
  case umpi_MPI_Group_incl:
    replay_Group_incl(e, timer);
    break;
  case umpi_MPI_Group_intersection:
    replay_Group_intersection(e, timer);
    break;
  case umpi_MPI_Group_range_excl:
    replay_Group_range_excl(e, timer);
    break;
  case umpi_MPI_Group_range_incl:
    replay_Group_range_incl(e, timer);
    break;
  case umpi_MPI_Group_union:
    replay_Group_union(e, timer);
    break;

  default:
    cerr << "error: unsupported event " << op_code << " on rank " << my_rank << endl;
    cerr << e->toString() << endl;
    exit(0);
    break;
  }
}


int replay_event(Event *e, int depth, int iteration){
  int op_code;

  if(replay_timer)
    replay_timer->setCurrentEvent(e);
  op_code = e->getOp();
  switch(op_code){

  case umpi_MPI_Finalize:
    finalize_req(replay_req);
    finalize_mem();
    finalize_ptr (replay_ptr);
    delete replay_timer;
    break;

  case umpi_MPI_Init:
    init_req(&replay_req, REQ_BUF_SIZE);
    init_mem(REQ_BUF_SIZE);
    replay_ptr = init_ptr (PTR_HANDLER_SZ);
    replay_timer = new Timer();
    replay_timer->setCurrentEvent(e);
    replay_timer->setTime();
    break;

  case umpi_MPI_Irecv:
    replay_Irecv(e, replay_timer);
    break;

  case umpi_MPI_Recv:
    replay_Recv(e, replay_timer);
    break;

  case umpi_MPI_Isend:
    replay_Isend(e, replay_timer);
    break;

  case umpi_MPI_Send:
    replay_Send(e, replay_timer);
    break;

  case umpi_MPI_Wait:
    replay_Wait(e, replay_timer);
    break;

  case umpi_MPI_Waitall:
    replay_Waitall(e, replay_timer);
    break;

  default:
    switch_event(op_code, e, replay_timer);
    break;
  }

  return 0;
}
