/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */
#include "stack_sig_32.h"
#include <string.h>
#include <stdlib.h>

/*
 *  This code is based on the stack trace implemention in
 *  mpiP (http://mpisourceforge.net/)
 *  See mpiPi_RecrdTraceBack() function in record_stack.c
 */

extern intptr_t * do_stack_unwind ()
{
  jmp_buf jb;
  int i, frame_count = 0, written = 0;
  intptr_t fp = 0, lastfp = 0, pc = 0;
  intptr_t xor_val = 0;

  memset (&jb, 0, sizeof(jmp_buf));
  setjmp(jb);
  fp = ParentFP(jb);

  /* walk out of the wrappers (extra function call for RSDs) */
  for(i = 0; i < SKIP_DEPTH; i++)
  {
    if(fp != (intptr_t)0 && fp != 0x01)
    {
      fp = NextFP(fp);
    }
  }

  for(i = 0; i < TRACE_DEPTH; i++) {
    if(fp != (intptr_t)0 && fp != 0x01) {
      /* record this frame's pc */
      pc = FramePC(fp);
      if(pc != (intptr_t)0) {
        frame_count++;

        /* Get previous instruction */
        pc_array[i] = (intptr_t)((char*)pc - 1);
      }
      else {
        pc_array[i] = (intptr_t)0;
        break;
      }

      /* update frame ptr */
      lastfp = fp;
      fp = NextFP(fp);
    }
    else {
      pc_array[i] = (intptr_t)0;
      break;
    }
  }
  //on x86 there are two extra random addresses
  //pc_array[--i] = (intptr_t)0;
  //pc_array[--i] = (intptr_t)0;

  return pc_array;
}

extern void do_stack_free (intptr_t * t)
{}
/* EOF */
