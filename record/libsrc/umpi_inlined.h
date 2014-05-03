/*
 * Copyright (c) 2003-May 2, 2014: Bronis de Supinski (bronis@llnl.gov)
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */
/* umpi_inlined.h -- hand inlining macros header */

#ifndef _HAND_INLINED_H
#define _HAND_INLINED_H

#ifdef UMPI_HAND_INLINING

/* inlining of queue functions... */
#define q_empty(q)  ((q)->count == 0)
#define q_count(q)  ((q)->count)
#define q_head(q) (munge_q=(q),(((munge_q) && (munge_q->head)) ? (munge_q->head->data) : NULL))
#define q_tail(q) (munge_q=(q),(((munge_q) && (munge_q->tail)) ? (munge_q->tail->data) : NULL))
#define q_append(q, data) (q_enqueue(q, data))

/* there is a bug in the following; can speed things up if fixed... */
#if 0
#define q_enqueue(q, newdata) \
	(munge_q=(q), \
	(((munge_q == NULL) || (munge_q->count<0)) ? UMPIERR_FATAL : \
	(munge_ele=(Q_ele_t *) malloc (sizeof (Q_ele_t)), \
	munge_ele->data=(newdata), \
	munge_ele->prev=munge_q->tail, \
	munge_ele->next=NULL, \
	munge_q->count++, \
	((munge_q->count>1) ? \
	(munge_q->tail->next=munge_ele,munge_q->tail=munge_ele,0) : \
	(munge_q->head = munge_q->tail = munge_ele,0)))))

#define q_dequeue(q, thedata) \
	(munge_q=(q), \
	(((munge_q)&&(munge_q->count>0)) ? \
	((munge_ele=munge_q->head, \
	*(thedata)=munge_ele->data, \
	munge_q->count == 1) ? \
	(munge_q->head=munge_q->tail=NULL, \
	munge_q->count--,free(munge_ele),0) : \
	(munge_q->head=munge_ele->next, \
	munge_ele->prev=NULL,munge_q->count--,free(munge_ele),0)) : \
	UMPIERR_FATAL))
#endif

/* inlining of reference count functions... */
#define umpi_set_ref_count(op, value)  ((op)->umpi_op_ref_count = (value)) 
#define umpi_get_ref_count(op)  ((op)->umpi_op_ref_count)
#define umpi_inc_ref_count(op)  ((op)->umpi_op_ref_count++)


#ifndef UMPI_DEBUG_LEVEL_2

#define umpi_dec_ref_count(op, location)  do { \
  if ((op->umpi_op_ref_count -= 1) == 0) free_op(op); \
} while (0)

#endif


/* inlining of lookup table... */
#define get_struct_by_opcode(opcode) (&(umpi_mpi_lookup[(opcode)-UMPI_BASE_OPCODE]))


/* inlining of table functions... */
#define table_get_row_column(t, therow, thecolumn) (munge_t=(t),((munge_t) ? ((thecolumn >= munge_t->column_count) ? (munge_t->default_val) : (munge_col = &(munge_t->columns[thecolumn]), ((therow >= munge_col->row_count) ? (munge_t->default_val) : (munge_col->rows[therow])))) : NULL))

#endif

#endif
