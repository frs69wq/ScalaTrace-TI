/* -*- Mode: C; -*- */
/* Creator: Jeffrey Vetter (vetter3@llnl.gov) Wed Feb 23 2000 */
/* $Header: /home/cvs/cvs/xwu3/ScalaTraceV2/record/libsrc/queue.h,v 1.1 2012/04/02 05:12:09 xwu3 Exp $ */
/* queue.h -- queue header */

#ifndef _QUEUE_H
#define _QUEUE_H

#define Q_CONTINUE 0
#define Q_BREAK 1

typedef struct Q_ele
{
  void *data;
  struct Q_ele *next, *prev;
}
Q_ele_t;

typedef struct Q
{
  int cookie;
  int count;
  Q_ele_t *head, *tail;
}
Q_t;

typedef int (*qComparator) (void *, void *);	/* ==0 is match, < and > less, greater */
typedef int (*qApply) (void *);	/* take the data node as a pointer and apply qApply to it */
typedef int (*qApplyWithArg) (void *, void *, int *);	/* take the data node as a pointer and apply qApplyWithArg to it; pass additional args in second pointer, use int * so user determines how to combine results (and intial result value) and use return value to break iteration */
typedef int (*_qApply) (struct Q_ele *);	/* take the Q_ele as a pointer and apply qApply to it */

int qCompare_ints (void *a, void *b);
int qCompare_q_ele_data_addrs (void *a, void *b);

#include "queue_proto.h"

#endif

/* EOF */
