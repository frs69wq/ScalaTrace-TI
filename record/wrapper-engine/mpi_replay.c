/* -*- Mode: C; -*- */
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
