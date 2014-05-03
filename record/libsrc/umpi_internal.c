/*
 * Copyright (c) 2000-May 2, 2014: Jeffrey Vetter (vetter3@llnl.gov)
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

/* umpi_internal.c -- internal vars */


#include "umpi_internal.h"

#ifdef UMPI_HAND_INLINING
Q_t *munge_q = NULL;
Q_ele_t *munge_ele = NULL;
umpi_table_t *munge_t = NULL;
umpi_table_column_t *munge_col = NULL;
#endif

#ifndef UMPI_SHARED_MEM_ONLY

MPI_Comm UMPI_COMM_WORLD;

#endif

#ifdef UMPI_MPI_IP_PANIC

MPI_Errhandler UMPI_ERRORS_PANIC;
MPI_Comm UMPI_PANIC_COMM;
pthread_t umpi_panic_thread;
pthread_mutex_t umpi_panic_mutex;
volatile int umpi_in_panic = 0;
volatile int umpi_panic_begun = 0;
volatile int umpi_panic_ready;
pthread_cond_t umpi_panic_condition;

#endif

#ifndef UMPI_MPI_ONLY

Mesg *mesgptr;

#ifdef UMPI_SHARED_MEM_ONLY

int shmid;

#endif

#endif

#if defined (UMPI_IP_ONLY) || defined (UMPI_MPI_IP_PANIC)

int mgr_sockfd, newsockfd;
pthread_t umpi_outfield_thread, umpi_heartbeat_thread;
volatile int umpi_outfield_ready, umpi_heartbeat_ready;
pthread_cond_t umpi_outfield_condition, umpi_heartbeat_condition;
pthread_cond_t umpi_user_condition;
pthread_mutex_t umpi_outfield_mutex, umpi_user_mutex;
volatile int umpi_mpi_done = 0;

#endif

FILE *errorLogFile = NULL;

#ifdef UMPI_DEBUG_LEVEL_2
Q_t *umpi_task_op_q = NULL;
Q_t *umpi_asynch_task_op_q = NULL;
#endif

Q_t *umpi_task_persistent_init_op_q = NULL;
Q_t *umpi_task_req_start_op_q = NULL;
umpi_op_t *umpi_task_last_preop = NULL;


void *umpi_task_dtypes = NULL;	    /* type tracking binary tree root */
void *umpi_task_groups = NULL;	    /* group tracking binary tree root */
void *umpi_task_reduce_ops = NULL;  /* MPI_Op tracking binary tree root */
void *umpi_task_errhandlers = NULL; /* error handler tracking bin tree root */
void *umpi_task_commited_dtypes = NULL; /* committed type tr. bin tree root */

double range = 1000.0;
int cur_index = 0;
int delayStrategy = 0;
int gErrorExit = 1;		/* our atexit checks this to see if it's set */

int umpi_rank = 0;
int umpi_size = 0;
int umpi_task_seq_num = 0;

/*
  pthread_attr_t umpi_thread_attr;
  pthread_t umpi_thread;
*/

int activeTasks;

#ifdef UMPI_DEBUG_LEVEL_2
Q_t **umpi_mgr_history_qs;	  /* one for each mpi task: info pool */
#endif

/* one of each of the following for each mpi task */
Q_t **umpi_mgr_deadlock_qs;	       
umpi_op_t **umpi_mgr_last_preops;      
Q_t **umpi_mgr_persistent_init_op_qs;


Q_t ***umpi_mgr_recv_match_op_qs;
Q_t ***umpi_mgr_send_match_op_qs;
Q_t ***umpi_mgr_col_match_op_qs;


umpi_table_t *umpi_req_start_table = NULL; /* table to track MPI req
					      handle corresponding
					      to umpi_req_handle */
/* mpi_req_track_t **umpi_req_array;          array indexed by
					      umpi_req_handles that
					      tracks active reqs */
int          umpi_req_array_size = 0;      /* size of umpi_req_array */ 


umpi_table_t *umpi_comm_table = NULL;  /* table to track corresponding MPI 
					  comm handle to umpi_comm_handle */
/* umpi_comm_t  **umpi_comm_array = NULL;   array indexed by umpi_comm_handles
					   that tracks related data including
					   lrank to grank translation... */
int          umpi_comm_array_size = 0; /* size of umpi_comm_array... */

umpi_table_t *umpi_type_table;

FILE *mgrLogFile = NULL;
FILE *taskLogFile = NULL;
FILE *taskAsynchLogFile = NULL;

#ifdef UMPI_COMBINED_LOCAL_FILES
pthread_mutex_t umpi_l_file_mutex;
#endif


/* one op error collecting globals... 
umpi_op_collector_t *umpi_task_uncommitted_dtypes = NULL;
umpi_op_collector_t *umpi_task_asynch_redundant_type_frees = NULL;
umpi_op_collector_t *umpi_task_asynch_redundant_group_frees = NULL;
umpi_op_collector_t *umpi_task_asynch_redundant_op_frees = NULL;
umpi_op_collector_t *umpi_task_asynch_redundant_errhandler_frees = NULL;
umpi_op_collector_t *umpi_task_asynch_reused_type_handles = NULL;
umpi_op_collector_t *umpi_task_asynch_reused_group_handles = NULL;
umpi_op_collector_t *umpi_task_asynch_reused_op_handles = NULL;
umpi_op_collector_t *umpi_task_asynch_reused_errhandler_handles = NULL;
umpi_op_collector_t *umpi_mgr_redundant_comm_frees = NULL;
umpi_op_collector_t *umpi_mgr_bad_activations = NULL;
umpi_op_collector_t *umpi_mgr_bad_request_frees = NULL;
umpi_op_collector_t *umpi_mgr_bad_request_handles = NULL;
umpi_op_collector_t *umpi_mgr_bad_type_handles = NULL;
umpi_op_collector_t *umpi_mgr_incompatible_types = NULL;
umpi_op_collector_t *umpi_mgr_unfreed_persistent_reqs = NULL;
umpi_op_collector_t *umpi_mgr_lost_reqs = NULL;
umpi_op_collector_t *umpi_mgr_dropped_reqs = NULL;
umpi_op_collector_t *umpi_mgr_lost_comms = NULL;
*/

/* two op error collecting globals... 
umpi_op_collector_t *umpi_task_send_overwrites = NULL;
umpi_op_collector_t *umpi_mgr_bad_reactivations = NULL;
umpi_op_collector_t *umpi_mgr_incomplete_request_frees = NULL;
umpi_op_collector_t *umpi_mgr_extra_type_commits = NULL;
umpi_op_collector_t *umpi_mgr_type_mismatches = NULL;
*/

umpi_mpi_lookup_t  *umpi_g_lookup;

/* EOF */
