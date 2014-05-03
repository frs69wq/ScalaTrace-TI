/*
 * Copyright (c) 2002-May 2, 2014: Bronis de Supinski (bronis@llnl.gov)
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef _UMPIRE_MPI_MATCH_H
#define _UMPIRE_MPI_MATCH_H

#include <mpi.h>

/* this type makes finding matches much easier, particularly 
   for MPI_Sendrecv and MPI_Sendrecv_replace... */
typedef enum
{
  UMPI_SEND_OP,
  UMPI_RECV_OP,
  UMPI_COLLECTIVE_OP
} 
umpi_mpi_op_type_t;


/* this type supports MPI_ANY_SOURCE deadlock checking... */
typedef enum
{
  UMPI_NOT_PSEUDO_MATCH = 0,
  UMPI_PRIMARY_PSEUDO_MATCH,
  UMPI_SECONDARY_PSEUDO_MATCH
} 
umpi_pseudo_t;


/* we create one of these for each matching rank, 
   i.e. comm size for the collective ops */
typedef struct _umpi_matching_op_t
{
  int match_global_rank;
  int match_local_rank;
  MPI_Request umpi_mpi_handle;          /* handle of local request */
  int umpi_mpi_tag;
  umpi_mpi_op_type_t match_type;
  int safely_completed;         /* deadlock free completion finished */
                                /* before the matching_op was seen */
  umpi_op_t *matching_op;
  int umpi_match_handle;        /* handle of matching request */
  umpi_pseudo_t umpi_pseudo_match;  /* match values are temporary... */
  Q_t *any_source_m_ops;        /* queue of clones of this m_op */
                                /* used for MPI_ANY_SOURCE handling */
  struct _umpi_matching_op_t *clone_op; /* if this is a copy of an m_op */
                                        /* keep pointer to original m_op... */
}
umpi_matching_op_t;


typedef struct _umpi_match_search_t
{
  umpi_op_t *cur_umpi_op;
  umpi_matching_op_t *cur_umpi_m_op;
}
umpi_match_search_t;


typedef struct _umpi_completion_t
{
  umpi_op_t *completing_umpi_op;
  int fully_completed;
}
umpi_completion_t;


extern Q_t ***umpi_mgr_recv_match_op_qs;
extern Q_t ***umpi_mgr_send_match_op_qs;
extern Q_t ***umpi_mgr_col_match_op_qs;


/* message matching prototypes */
umpi_matching_op_t *umpi_dup_m_op (umpi_matching_op_t *reqm_op);
int free_m_op (umpi_matching_op_t *m_op);


int umpi_match_messages (umpi_op_t *new_op);
int umpi_try_to_match_message (umpi_op_t *new_op, umpi_matching_op_t *m_op, 
			       int is_a_new_op, int looking_for_pseudo_match);
int umpi_match_for_completed_any_src (int rank);
int umpi_match_messages (umpi_op_t *new_op);
int umpi_translate_any_src_to_global_rank (int i_rank, 
					   int umpi_comm_handle, 
					   int correspondent);
int umpi_pseudo_recv_match_found (umpi_matching_op_t *m_op, int cur_rank);
int umpi_fix_any_src_match_op_qs (umpi_op_t *op, umpi_matching_op_t *m_op);


int umpi_get_matching_op_by_handle (Q_t *m_op_q, int handle, 
				    umpi_matching_op_t **ret_m_op);

int umpi_dec_m_op_ref_count (void *a, void *arg_ptr, int *res);


int umpi_set_g_lookup (umpi_op_t *op);


#endif

/* EOF */
