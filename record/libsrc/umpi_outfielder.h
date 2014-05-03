/*
 * Copyright (c) 2002-May 2, 2014: Bronis de Supinski (bronis@llnl.gov)
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef _UMPIRE_OUTFIELDER_H
#define _UMPIRE_OUTFIELDER_H


/* NOT USED YET - WILL MAKE SPECIFIC TO OUTFIELDER WORKINGS... */
typedef struct _outfielder_op
{
  int op;			/* MPI operation */
  int order;			/* pre or post mpi op */
  int rank;
  void *pc;			/* return address to function calling MPI_* */
  union _local_data
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

  struct _op **matching_reqs;    /* pointers to matching requests */
  struct _umpi_completion_t *completions;      /* array of completions */
  struct _umpi_mpi_typemap *themap; /* used for tracking map for chksum
				       for committed types in task only 
				       (so far)...
				       store here for now; will be more
				       rigorous later... */

  int updated_by_postop;
  int was_pseudo_cleaned;
  int schksum;                   /* chksum on send buffer */
  int rchksum;                   /* chksum on receive buffer */
  int *valchksum;                /* chksums on multiple request buffers... */
  int umpi_comm_handle;          /* tracks assigned handle in comm_creators */
  int umpi_comm_trans;           /* umpi_comm_handle corresponding
				    to data.mpi.comm */

}
umpi_outfielder_op_t;


/* outfielder globals dependent on communication mechanism... */
#if defined (UMPI_IP_ONLY) || defined (UMPI_MPI_IP_PANIC)

extern int mgr_sockfd, newsockfd;
extern pthread_t umpi_outfield_thread, umpi_heartbeat_thread;
extern volatile int umpi_outfield_ready, umpi_heartbeat_ready;
extern pthread_cond_t umpi_outfield_condition, umpi_heartbeat_condition;
extern pthread_cond_t umpi_user_condition;
extern pthread_mutex_t umpi_outfield_mutex, umpi_user_mutex;
extern volatile int umpi_mpi_done;

#endif

#ifdef UMPI_MPI_IP_PANIC

extern MPI_Errhandler UMPI_ERRORS_PANIC;
extern pthread_t umpi_panic_thread;
extern pthread_mutex_t umpi_panic_mutex;
extern pthread_cond_t umpi_panic_condition;
extern volatile int umpi_in_panic;
extern volatile int umpi_panic_ready;
extern volatile int umpi_panic_begun;

/* technically, these probably belongs in a "umpi_error_handlers.h" file... */
void umpi_errhandler (MPI_Comm *umpi_comm, int *errorcode, ...);
int umpi_mpi_panic (void *pc);

#endif


/* general outfielder globals... */
extern int gErrorExit;
extern int umpi_rank;
extern int umpi_size;
extern int umpi_task_seq_num;

#ifdef UMPI_DEBUG_LEVEL_2
extern Q_t *umpi_task_op_q;
extern Q_t *umpi_asynch_task_op_q;
#endif

extern Q_t *umpi_task_persistent_init_op_q;
extern Q_t *umpi_task_req_start_op_q;
extern umpi_op_t *umpi_task_last_preop;
extern int cur_index;
/*
  extern pthread_t umpi_thread;
  extern pthread_attr_t umpi_thread_attr;
*/
extern int delayStrategy;    /* used in _delay, which is not used... */
extern double range;         /* used in _delay, which is not used... */
extern FILE *taskLogFile;
extern FILE *taskAsynchLogFile;

#ifdef UMPI_COMBINED_LOCAL_FILES
extern pthread_mutex_t umpi_l_file_mutex;
#endif

/* general outfielder prototypes... */
void umpi_Global_Init (int size, int rank, char *prog_name);
void umpi_Global_Finalize (int size, int rank);
void umpi_Local_Init (int size, int rank, char *prog_name);
void umpi_Local_Finalize (int size, int rank);
void umpi_Asynch_Local_Finalize (int size, int rank);
int umpi_Asynch_op (umpi_op_t * op);
int umpi_process_asynch_op (umpi_op_t *op);
int umpi_send_to_mgr (umpi_op_t *op);


/* these might be better in a umpi_utils.h file... */
void umpi_l_msg (int rank, void *pc, char *msg);
void umpi_l_err_msg (int rank, void *pc, char *msg);
void umpi_l_internal_err_msg (int rank, void *pc, char *msg);

/* these might be better in a umpi_utils.h file... */
void umpi_l_asynch_msg (int rank, void *pc, char *msg);
void umpi_l_asynch_err_msg (int rank, void *pc, char *msg);
void umpi_l_asynch_internal_err_msg (int rank, void *pc, char *msg);

#ifdef UMPI_DEBUG_LEVEL_0
int DumpTaskQs (void);
int DumpAsynchTaskQs (void);
#endif


#endif

/* EOF */
