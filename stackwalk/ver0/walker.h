/*
 * Copyright (c) 2008-May 2, 2014: Prasun Ratn <prasun@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */
#ifndef __WALKER_H__
#define __WALKER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
extern intptr_t * do_stack_unwind ();

#define SKIP_DEPTH     4 /* not used */
#define TRACE_DEPTH 1000 /* maximum length of stack sig */

intptr_t pc_array[TRACE_DEPTH];

#ifdef __cplusplus
}
#endif

#endif
/* vim: set tw=80 sw=2 expandtab: */
