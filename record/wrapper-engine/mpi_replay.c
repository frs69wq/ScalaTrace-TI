/*
 * Copyright (c) 2002-May 2, 2014: Bronis de Supinski (bronis@llnl.gov)
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */
/* mpi_replay.c -- prints code to print specific Umpire ops */

#include <stdio.h>
#include <string.h>
#include <search.h>

#include "y.tab.h"
#include "wrapper.h"


void
RecordReplayOp(char *name)
{
  fprintf (replayHFile, "\nextern void umpi_replay_op_%s(void *node);",name);
  fprintf (replayFile, "\nvoid umpi_replay_op_%s(void *node){}",name);
  return;
}

  
/* eof */
