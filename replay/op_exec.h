/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef __OP_EXEC_H__
#define __OP_EXEC_H__

#include "PtrHandler.h"
#include "Timer.h"
#include "replay.h"
#include "Event.h"
#include "umpi_mpi_ops.h"
#include "ReqHandler.h"
#include "mem_handler.h"
#include <sys/time.h>


using namespace std;

class Event;

extern ptr_handler_t replay_ptr;

void switch_event(int op_code, Event *e, Timer *timer);

int replay_event (Event *e, int depth, int iteration);

/* replay functions for individual MPI functions */
/* collectives */
void replay_Allgather(Event *e, Timer *timer);
void replay_Allgatherv(Event *e, Timer *timer);
void replay_Allreduce(Event *e, Timer *timer);
void replay_Alltoall(Event *e, Timer *timer);
void replay_Alltoallv(Event *e, Timer *timer);
void replay_Barrier(Event *e, Timer *timer);
void replay_Bcast(Event *e, Timer *timer);
void replay_Gather(Event *e, Timer *timer);
void replay_Gatherv(Event *e, Timer *timer);
void replay_Reduce(Event *e, Timer *timer);
void replay_Reduce_scatter(Event *e, Timer *timer);
void replay_Scatter(Event *e, Timer *timer);
void replay_Scatterv(Event *e, Timer *timer);

/* communicator ops */
MPI_Comm replay_Comm_create(Event *e, Timer *timer);
MPI_Comm replay_Comm_dup(Event *e, Timer *timer);
void replay_Comm_free(Event *e, Timer *timer);
MPI_Comm replay_Comm_split(Event *e, Timer *timer);

/* group ops*/
MPI_Group replay_Comm_group(Event *e, Timer *timer);
MPI_Group replay_Group_difference(Event *e, Timer *timer);
MPI_Group replay_Group_excl(Event *e, Timer *timer);
void replay_Group_free(Event *e, Timer *timer);
MPI_Group replay_Group_incl(Event *e, Timer *timer);
MPI_Group replay_Group_intersection(Event *e, Timer *timer);
MPI_Group replay_Group_range_excl(Event *e, Timer *timer);
MPI_Group replay_Group_range_incl(Event *e, Timer *timer);
MPI_Group replay_Group_union(Event *e, Timer *timer);

#endif /*__OP_EXEC_H__*/

