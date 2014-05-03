/*
 * Copyright (c) 2000-May 2, 2014: Jeffrey Vetter (vetter3@llnl.gov)
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

/* umpi_pc.h -- pc info */

#ifndef _UMPI_PC_H
#define _UMPI_PC_H

#include <setjmp.h>
#define DEFINE_PC void*pc=(void*)0xdeadead;jmp_buf jb
#define GATHER_PC setjmp(jb);pc=(void*)GetParentPC(jb)

#ifdef AIX
#define GET_FP
#else
#define GET_FP pc = __builtin_frame_address(0)
#endif

#endif

/* EOF */
