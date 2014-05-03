/* -*- Mode: C; -*- */
/* Creator: Jeffrey Vetter (vetter3@llnl.gov) Wed Mar 15 2000 */
/* $Header: /home/cvs/cvs/xwu3/ScalaTraceV2/record/libsrc/umpi_pc.h,v 1.1 2012/04/02 05:12:10 xwu3 Exp $ */

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
