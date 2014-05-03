/*
 * Copyright (c) 2007-May 2, 2014: Prasun Ratn <prasun@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */
#include <stdint.h>
#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "walker.h"

#if defined(__x86_64__)
__inline__ static void **NextStackFrame(void **old_sp) {
  void **new_sp = (void **) *old_sp;
  if (new_sp <= old_sp) return NULL;
  if ((uintptr_t)new_sp & (sizeof(void *) - 1)) return NULL;
  if ((uintptr_t)new_sp - (uintptr_t)old_sp > 100000) return NULL;
  return new_sp;
}

intptr_t * do_stack_unwind ()
{
  void **sp;
  ucontext_t * ucontext;
  int n = 0, i = 0;
  void** new_sp = NULL;
  int skip = SKIP_DEPTH;

  ucontext = (ucontext_t *) malloc (sizeof (ucontext_t));
  
  getcontext (ucontext);

  sp = (void **) __builtin_frame_address(0);

  while (sp && n < TRACE_DEPTH-1) {
    if (*(sp+1) == (void *)0) {
      break;
    }
    new_sp = NextStackFrame(sp);
    if (!new_sp) break;
    if (skip <= 0)
    {
      pc_array [i++] = (intptr_t)*(sp+1); 
      n++;
    }
    skip--;
    sp = new_sp;
  }

  free (ucontext);
  pc_array[i] = 0;

  return pc_array;
}

void do_stack_free (intptr_t * t)
{}

#else
#error X86_64 specific file
#endif

#ifdef STACK_SIG_DEBUG
void a(int b)
{
  if (b)
  {
    void ** sp;

    sp = (void **) __builtin_frame_address(0);
    a (b - 1);
  }
  else
  {
    intptr_t * tmp = do_stack_unwind ();
    int i = 0;
    while (tmp[i]) {
      printf ("%x ", tmp[i]);
      i++;
    }
    printf ("\n");
  }
}
void one ()
{
  a(2);
}
void two ()
{
  a(2);
}
void three ()
{
  a(2);
}
int main ()
{
  one(); //two(); three();
  return 0;
}

#endif
/* vim: set tw=80 ts=2 sw=2 expandtab: */
