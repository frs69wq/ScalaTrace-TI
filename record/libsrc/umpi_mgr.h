/* -*- Mode: C; -*- */
/* Creator: Bronis R. de Supinski (bronis@llnl.gov) Tue June 11 2002 */
/* $Header: /home/cvs/cvs/xwu3/ScalaTraceV2/record/libsrc/umpi_mgr.h,v 1.1 2012/04/02 05:12:09 xwu3 Exp $ */
/* umpi_mgr.h -- mgr data structures and globals */

#ifndef _UMPIRE_MGR_H
#define _UMPIRE_MGR_H


/* eventually rename this to umpi_mgr_op_t... */
typedef struct _mgr_op
{
  int op;			/* MPI operation */
  int order;			/* pre or post mpi op */
  int rank;
  void *pc;			/* return address to function calling MPI_* */
  union _data
  {
    umpi_mpi_param_t mpi;
  }
  data;
  int res;
  int umpi_op_ref_count;
  int seq_num;			/* sequence number for op on it's task */

  Q_t *matching_ops;            /* queue of umpi_matching_op_t elements */
                                /* stores pointers to matching ops, plus 
				   info needed for uncompleted matches */
                                /* a queue simplifies the dependency analysis
				   a pointer simplifies initial condition */

  struct _mgr_op **matching_reqs;    /* pointers to matching requests */
  struct _umpi_completion_t *completions;      /* array of completions */
  struct _umpi_mpi_typemap *themap; /* used for tracking map for chksum
				       for committed types in task only 
				       (so far)...
				       store here for now; will be more
				       rigorous later... */
  struct _mgr_op *type_op;          /* pointer to relevant type commit... */
                                    /* outfielder uses w/non-blocking ops */
                                    /* mgr uses for sendtype if 2 types... */
  struct _mgr_op *recvtype_op;     /* mgr pointer to 2nd type commit... */

  int umpi_type_handle;             /* internal handle to type array entry... */
  int umpi_recv_type_handle;

  int umpi_type_ref_count;          /* reservations on op for type handle */

  int might_deadlock;               /* TRUE if on a deadlock queue */
  int updated_by_postop;
  int was_pseudo_cleaned;
  int schksum;                   /* chksum on send buffer */
  int rchksum;                   /* chksum on receive buffer */
  int *valchksum;                /* chksums on multiple request buffers... */
  int umpi_comm_handle;          /* tracks assigned handle in comm_creators */
  int umpi_comm_trans;           /* umpi_comm_handle corresponding
				    to data.mpi.comm */
  
  int profile_level;     /* Stored parameter from MPI_Pcontrol. */
}
umpi_op_t;


/* this is currently only really used by UMPI_MPI_IP_PANIC but it's */
/* simpler to have it available in all the versions... */
void umpi_mgr_panic (void);


/* general mgr globals... */
extern int activeTasks;
extern FILE *mgrLogFile;


#ifdef UMPI_DEBUG_LEVEL_2
extern Q_t **umpi_mgr_history_qs; /* one for each mpi task: new info pool */
#endif


extern umpi_op_t **umpi_mgr_last_preops;


/* this might be better in a umpi_utils.h file... */
void umpi_mgr_msg (int rank, void *pc, char *msg);
void umpi_mgr_err_msg (int rank, void *pc, char *msg);
void umpi_mgr_internal_err_msg (int rank, void *pc, char *msg);

/* general mgr prototypes... */
void *umpi_mgr (void *arg);
int umpi_process_mgr_info (umpi_op_t *new_op);
int umpi_verify_mgr_integrity (umpi_op_t *new_op);
int umpi_mgr_large_op_recv (char *buf, int packed_size, int sender);
void umpi_g_save_for_postop_processing (umpi_op_t *op);
void umpi_g_get_matching_preop (umpi_op_t *op, umpi_op_t **old_op);


#ifdef UMPI_DEBUG_LEVEL_0
int DumpMgrQs (void);
#endif


#endif

/* EOF */
