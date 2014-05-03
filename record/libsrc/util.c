/*
 * Copyright (c) 1999-May 2, 2014: Jeffrey Vetter (vetter3@llnl.gov)
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */
/* util.c -- common utils */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include "utils.h"


/* TODO: pull this from system headers.  Look at _POSIX2_LINE_MAX in limits.h. */
#define ARG_MAX 2048

int xargc;
char **xargv;

#include <setjmp.h>

#include "common.h"
#include "umpi_internal.h"
//#include "internal.h"

static int debug = 0;

#if 0

void
_task_atexit (void)
{
  if (gErrorExit) {
    fprintf (stderr, "UMPIRE caught an ABNORMAL MPI TERMINATION. Exiting!\n");

#if defined (UMPI_IP_ONLY) || defined (UMPI_MPI_IP_PANIC)
#if 0

    if (umpi_rank == UMPI_MGR_RANK) {
      {
	/* temporary stuff to get socket cleaned up (??!!) */
	struct sockaddr_un temp_sockaddr;
	unsigned long      temp_socklen;

	assert (umpi_size > 1);

	newsockfd = accept (mgr_sockfd, 
			    (struct sockaddr *) &temp_sockaddr, 
			    &temp_socklen);

	if (newsockfd >= 0)
	  close (newsockfd);
      }
      close (mgr_sockfd);
    }
    else if (umpi_rank == (UMPI_MGR_RANK + 1) % umpi_size) {
      /* temporary stuff to get socket cleaned up (??!!) */
      struct sockaddr_un temp_sockaddr;
      unsigned long      temp_socklen;

      /* for now... */
      bzero ((char *) &temp_sockaddr, sizeof (struct sockaddr_un));
      temp_sockaddr.sun_family = AF_UNIX;
      strcpy (temp_sockaddr.sun_path, UMPI_MGRSTR_PATH);
      temp_socklen = 
	strlen (temp_sockaddr.sun_path) + sizeof (temp_sockaddr.sun_family);

      if ((newsockfd = socket (AF_UNIX, SOCK_STREAM,0)) < 0) {
	printf ("outfielder %d: can't open temporary socket\n", umpi_rank);
      }
      else {
	if (connect (newsockfd, 
		     (struct sockaddr *) &temp_sockaddr, temp_socklen) < 0) {
	  fprintf (stderr, 
		   "outfielder %d: can't connect to manager socket\n", 
		   umpi_rank);
	}
	else {
	  close (newsockfd);
	}
      }
    }

#endif
#endif
  }

#ifndef UMPI_COMBINED_LOCAL_FILES
  if (taskLogFile != NULL) {
    umpi_Local_summary ();
    fclose (taskLogFile);
    taskLogFile = NULL;
  }
#endif

  if (taskAsynchLogFile != NULL) {
    umpi_outfielder_summary();    
    fclose (taskAsynchLogFile);
    taskAsynchLogFile = NULL;
  }

  if (mgrLogFile != NULL) {
    umpi_mgr_summary ();
    fclose (mgrLogFile);
    mgrLogFile = NULL;
  }

  return;
}

#endif

double
exp_dev ()
{
  double x;

  for (; (x = drand48 ()) == 0.0;) /* NULL */ ;

  return -log (x);
}

long
expDelayTime ()
{
  return (long) rint ((range * exp_dev ()));
}

void
_delay (int i)
{
  switch (delayStrategy) {
  case 0:
    break;

  case 1: 
    {
      int usecs = expDelayTime ();
      usleep (usecs);
    }
    break;

  default:
    assert (0);
    break;
  }

  return;
}

unsigned
chksum (void *buf, int byteCount)
{
  int i;
  unsigned sum = 961937405;
  char *bp = (char *) buf;
  for (i = 0; i < byteCount; i++) {
    unsigned val = (unsigned) (bp[i]);
    sum ^= val + 7;
  }
  return sum;
}

void
_Abort (char *i_file, int i_line, char *i_msg)
{
  fprintf (stderr,
	   "UMPIRE INTERNAL ERROR (%s,%d): %s\n", i_file, i_line, i_msg);
  exit (-1);
}


void
err_sys (char *s)
{
  fprintf (stderr, "Aborting...system error (%s)\n", s);
  PMPI_Abort (MPI_COMM_WORLD, -1);
}

void
err_dump (char *s)
{
  err_sys (s);
}

void
debugmsg (int rank, void *pc, char *msg)
{
  if (debug)
    fprintf (stderr, "mgr read %d(0x%" PRIxPTR "): %s\n", rank, (intptr_t)pc, msg);

  return;
}


void
umpi_l_msg (int rank, void *pc, char *msg)
{
#ifdef UMPI_DEBUG_LEVEL_0
#ifdef UMPI_COMBINED_LOCAL_FILES
  pthread_mutex_lock( &umpi_l_file_mutex );
#endif
  fprintf (taskLogFile, "local:%d/%" PRIxPTR ": %s\n", rank, (intptr_t) pc, msg);
#ifdef UMPI_COMBINED_LOCAL_FILES
  pthread_mutex_unlock( &umpi_l_file_mutex );
#endif
#endif
  return;
}


void
umpi_l_asynch_msg (int rank, void *pc, char *msg)
{
#ifdef UMPI_DEBUG_LEVEL_0
#ifdef UMPI_COMBINED_LOCAL_FILES
  pthread_mutex_lock( &umpi_l_file_mutex );
#endif
  fprintf (taskAsynchLogFile, "outfielder:%d/%" PRIxPTR ": %s\n", rank, (intptr_t) pc, msg);
#ifdef UMPI_COMBINED_LOCAL_FILES
  pthread_mutex_unlock( &umpi_l_file_mutex );
#endif
#endif
  return;
}


void
umpi_mgr_err_msg (int rank, void *pc, char *msg)
{
  char buf[1024];
  sprintf (buf, "%d/%" PRIxPTR ": %s\n", rank, (intptr_t) pc, msg);
  fprintf (mgrLogFile, "%s", buf);
  fflush (mgrLogFile);
  fprintf (stderr, "%s", buf);
  fflush (stderr);
  return;
}


void
umpi_mgr_internal_err_msg (int rank, void *pc, char *msg)
{
  char buf[1024];
  sprintf (buf, "%d/%" PRIxPTR ": %s\n", rank, (intptr_t) pc, msg);
  fprintf (mgrLogFile, "%s", buf);
  fflush (mgrLogFile);
  fprintf (stderr, "%s", buf);
  fflush (stderr);
  return;
}


void
umpi_mgr_msg (int rank, void *pc, char *msg)
{
  fprintf (mgrLogFile, "%d/%" PRIxPTR ": %s\n", rank, (intptr_t) pc, msg);
}


void
umpi_l_err_msg (int rank, void *pc, char *msg)
{
  char buf[1024];
  sprintf (buf, "ULCL %d/%" PRIxPTR ": %s\n", rank, (intptr_t) pc, msg);
  fprintf (stderr, "%s", buf);
  fflush (stderr);
#ifdef UMPI_DEBUG_LEVEL_0
#ifdef UMPI_COMBINED_LOCAL_FILES
  pthread_mutex_lock( &umpi_l_file_mutex );
#endif
  fprintf (taskLogFile, "%s", buf);
  fflush (taskLogFile);
#ifdef UMPI_COMBINED_LOCAL_FILES
  pthread_mutex_unlock( &umpi_l_file_mutex );
#endif
#endif
  return;
}


void
umpi_l_internal_err_msg (int rank, void *pc, char *msg)
{
  char buf[1024];
  sprintf (buf, "ULCL %d/%" PRIxPTR ": %s\n", rank, (intptr_t) pc, msg);
  fprintf (stderr, "%s", buf);
  fflush (stderr);
#ifdef UMPI_DEBUG_LEVEL_0
#ifdef UMPI_COMBINED_LOCAL_FILES
  pthread_mutex_lock( &umpi_l_file_mutex );
#endif
  fprintf (taskLogFile, "%s", buf);
  fflush (taskLogFile);
#ifdef UMPI_COMBINED_LOCAL_FILES
  pthread_mutex_unlock( &umpi_l_file_mutex );
#endif
#endif
  return;
}


void
umpi_l_asynch_err_msg (int rank, void *pc, char *msg)
{
  char buf[1024];
  sprintf (buf, "ULCL %d/%" PRIxPTR ": %s\n", rank, (intptr_t) pc, msg);
  fprintf (stderr, "%s", buf);
  fflush (stderr);
#ifdef UMPI_DEBUG_LEVEL_0
#ifdef UMPI_COMBINED_LOCAL_FILES
  pthread_mutex_lock( &umpi_l_file_mutex );
#endif
  fprintf (taskAsynchLogFile, "%s", buf);
  fflush (taskAsynchLogFile);
#ifdef UMPI_COMBINED_LOCAL_FILES
  pthread_mutex_unlock( &umpi_l_file_mutex );
#endif
#endif
  return;
}


void
umpi_l_asynch_internal_err_msg (int rank, void *pc, char *msg)
{
  char buf[1024];
  sprintf (buf, "ULCL %d/%" PRIxPTR ": %s\n", rank, (intptr_t) pc, msg);
  fprintf (stderr, "%s", buf);
  fflush (stderr);
#ifdef UMPI_DEBUG_LEVEL_0
#ifdef UMPI_COMBINED_LOCAL_FILES
  pthread_mutex_lock( &umpi_l_file_mutex );
#endif
  fprintf (taskAsynchLogFile, "%s", buf);
  fflush (taskAsynchLogFile);
#ifdef UMPI_COMBINED_LOCAL_FILES
  pthread_mutex_unlock( &umpi_l_file_mutex );
#endif
#endif
  return;
}


int
free_op (umpi_op_t * op)
{
  umpi_mpi_lookup_t *lookup;
  
  assert (op);

  lookup = get_struct_by_opcode (op->op);

  if (lookup->free_op != NULL)
    lookup->free_op (op);

  if (op->matching_ops != NULL) {
    umpi_matching_op_t *m_op;

    while (!q_empty(op->matching_ops)) {
      q_dequeue (op->matching_ops, (void **) &m_op);
      //      free_m_op (m_op);
    }
    
    q_close (op->matching_ops);
  }

  if (op->matching_reqs != NULL)
    free (op->matching_reqs);

  if (op->completions != NULL)
    free (op->completions);

  //  if (op->themap != NULL)
  //    umpi_mpi_free_typemap (op->themap);

  if (op->valchksum != NULL)
    free (op->valchksum);

  free (op);

  return 0;
}

#if 0

umpi_op_t *
_copy_uop (umpi_op_t * op)
{
  umpi_mpi_lookup_t *lookup;
  umpi_op_t *p = NULL;

  assert (op);

  p = malloc (sizeof (umpi_op_t));
  assert (p);
  bcopy (op, p, sizeof (umpi_op_t));
  
  p->umpi_op_ref_count = 0;
  p->matching_reqs = NULL;
  p->matching_ops = NULL;
  p->completions = NULL;
  p->might_deadlock = 0;

  lookup = get_struct_by_opcode (op->op);
  if (lookup->copy_op != NULL)
    lookup->copy_op (op, p);

  if (op->themap != NULL)
     umpi_mpi_copy_typemap (op->themap, &(p->themap));

  if (op->valchksum != NULL) {
    p->valchksum = (int *) malloc (op->data.mpi.count * sizeof (int));
    assert (p->valchksum);
    bcopy (op->valchksum, p->valchksum, 
	   op->data.mpi.count * sizeof (int));
  }

  return p;
}

#endif

void
umpi_get_handles_and_count (umpi_op_t *p, int *count, MPI_Request **handles)
{
  umpi_mpi_lookup_t *lookup;

  assert (p);
  
  lookup = get_struct_by_opcode (p->op);

  if (lookup->multiple_requests) {
    *count = p->data.mpi.count;
    *handles = p->data.mpi.array_of_requests;
  }
  else {
    *count = 1;
    *handles = &(p->data.mpi.request);
  }

  return;
}
  

int
umpi_get_handle_index (umpi_op_t *start_p, MPI_Request handle)
{
  umpi_mpi_lookup_t *start_lu;
  int               index  = 0;

  assert (start_p);

  start_lu = get_struct_by_opcode (start_p->op);

  if (start_lu->multiple_requests) {
    /* find the index of the persistent init... */
    while ((index < start_p->data.mpi.count) &&
	     (start_p->data.mpi.array_of_requests[index] != handle))
      index++;

    assert (start_p->data.mpi.array_of_requests[index] == handle);
  }

  return index;
}


umpi_matching_op_t *
umpi_clean_up_startall_m_op (umpi_op_t *req_p, umpi_op_t *create_op)
{
  int                tempi, found = 0;
  int                match_count;
  umpi_mpi_lookup_t  *create_lu;
  umpi_matching_op_t *m_op;

  assert ((req_p) && (req_p->matching_ops) && (create_op));

  match_count = q_count (req_p->matching_ops);
  create_lu = get_struct_by_opcode (create_op->op);

  for (tempi = 0; tempi < match_count; tempi++) {
    /* find the matching op... */
    q_dequeue (req_p->matching_ops, (void **) &m_op);

    if (create_op->data.mpi.request != m_op->umpi_mpi_handle) {
      /* not the matching op that we're looking for... */
      q_enqueue (req_p->matching_ops, m_op);
    }
    else {
      /* restore m_op to the queue... */
      q_enqueue (req_p->matching_ops, m_op);
      
      found = TRUE;

      break;
    }
  }

  assert (found);

  return m_op;
}


#ifndef UMPI_HAND_INLINING


umpi_mpi_lookup_t *
get_struct_by_opcode (int opcode)
{
  /* check for bad opcode; suppose to correspond... */
  assert (((opcode - UMPI_BASE_OPCODE) >= 0) &&
	  ((opcode - UMPI_BASE_OPCODE) < UMPI_OPCODE_COUNT) &&
	  (umpi_mpi_lookup[opcode - UMPI_BASE_OPCODE].id == opcode));
  return &(umpi_mpi_lookup[opcode - UMPI_BASE_OPCODE]);
}


#endif

#if 0

void
q_close_deadlock_q (Q_t *the_q, int location)
{
  umpi_op_t *op;
  int       matched, total, i;

  while (!q_empty(the_q)) {
    q_dequeue (the_q, (void **) &op);

    /* clean up matches... */
    total = q_count (op->matching_ops);
    
    /* THIS ISN'T QUITE GOOD ENOUGH - NEED TO CLEAN UP THE */
    /* THE PERSISTENT INITS AND THE COMPLETIONS BUT THIS IS CLOSE */
    matched = 
      q_iterate (op->matching_ops, umpi_dec_m_op_ref_count, &location, 0);

    /* clean up for the unmatched m_ops... */
    for (i = matched; i < total; i++)
      umpi_dec_ref_count (op, location);

    umpi_dec_ref_count (op, location);
  }

  q_close (the_q);

  return;
}

#endif

void
q_close_op_q (Q_t *the_q, int location)
{
  umpi_op_t *op;

  while (!q_empty(the_q)) {
    q_dequeue (the_q, (void **) &op);
    umpi_dec_ref_count (op, location);
  }

  q_close (the_q);

  return;
}


void
q_close_match_op_q (Q_t *the_q, int location)
{
  umpi_match_search_t *s_op;

  while (!q_empty(the_q)) {
    q_dequeue (the_q, (void **) &s_op);
    umpi_dec_ref_count (s_op->cur_umpi_op, location);
  }

  q_close (the_q);

  return;
}


void
q_close_op_history_q (Q_t *the_q, int location)
{
  umpi_op_t *op;

  while (!q_empty(the_q)) {
    q_dequeue (the_q, (void **) &op);
    /* should make the following true but I'm not certain that it is yet */
    /* assert (umpi_get_ref_count(op) == 1); */
    free_op (op);
  }

  q_close (the_q);

  return;
}


void parse_command_line()
{
    int argl, i;
    FILE *args;
    char *pos;
    char cmdline[ARG_MAX];
    
    /* Check whether we can read from the cmdline file. */
    if (!(args = fopen("/proc/self/cmdline", "r"))) {
      return;
    }
    argl = fread(cmdline, sizeof(char), ARG_MAX, args);
    fclose (args);

    /* Count total number of args before allocating. */
    xargc = 0;    
    for (i=0; i < argl; i++) {
      if (cmdline[i] == '\0') xargc++;
    }
    
    /* tokenize cmdline and put args into xargv */
    xargv = (char**)malloc((xargc + 1) * sizeof(char*));
    pos = cmdline;
    for (i=0; i < xargc; i++) {
      int len = strlen(pos) + 1;
      xargv[i] = (char*)malloc(len * sizeof(char));
      strcpy(xargv[i], pos);
      pos += len + 1;
    }
    xargv[i] = NULL;
}


/* EOF */
