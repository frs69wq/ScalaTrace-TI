/* -*- Mode: C; -*- */
/* Creator: Bronis R. de Supinski (bronis@llnl.gov) Mon July 1 2002 */
/* currently, just relocated Umpire prototype code by Jeffrey Vetter... */
/* mpi_verification.c -- code to record op-dependent verification code... */


#include <stdio.h>
#include <strings.h>
#include <search.h>
#include <string.h>

#include "y.tab.h"
#include "wrapper.h"


int
RecordAsynchLocalPreStart (char *name)
{
  fprintf (outverifyFile, 
	   "\n\nint umpi_vcode_%s_l_pre (umpi_op_t * op)\n{\n", name);
  fprintf (outverifyFile, "int rc = 0;\n");
  fprintf (outverifyFile, "assert(op);\n");
      
  return 0;
}


int
RecordAsynchLocalPre (char *name)
{
  int i, needed = 0;

  if (vcode_l_pre[0]) {
    needed = 1;
    RecordAsynchLocalPreStart (name);

    for (i = 0; vcode_l_pre[i]; i++) {
      fprintf (outverifyFile, "%s\n", vcode_l_pre[i]);
    }
  }

  if ((AL_Test("resource_destructor")) &&
      (strcmp (name, "MPI_Request_free") != 0) &&
      (strcmp (name, "MPI_Comm_free") != 0)) {
    /* handle the preop part of resource tracking... */
    /* will actually move to asynch_local_pre... */
    if (!needed) {
      needed = 1;
      RecordAsynchLocalPreStart (name);
    }
    
    if (strcmp (name, "MPI_Type_free") == 0)
      fprintf (outverifyFile, "rc |= umpi_track_resources_type_free (op);\n");
    else if (strcmp (name, "MPI_Group_free") == 0)
      fprintf (outverifyFile, 
	       "rc |= umpi_track_resources_group_free (op);\n");
    else if (strcmp (name, "MPI_Op_free") == 0)
      fprintf (outverifyFile, "rc |= umpi_track_resources_op_free (op);\n");
    else if (strcmp (name, "MPI_Errhandler_free") == 0)
      fprintf (outverifyFile, 
	       "rc |= umpi_track_resources_errhandler_free (op);\n");
    else {
      fprintf (stderr, "\n\t\tWARNING: Unexcpected resource destructor");
      fprintf (outverifyFile, "assert (0);\n");
    }
  }

  if (needed) {
    fprintf (outverifyFile, "return rc;\n");
    fprintf (outverifyFile, "} /* umpi_vcode_%s_l_pre */\n\n", name);
    fprintf (outverifyHFile, 
	     "\nextern int umpi_vcode_%s_l_pre (umpi_op_t * op);", name);
  }

  return needed;
}


int
RecordAsynchLocalPostStart (char *name)
{
  fprintf (outverifyFile, 
	   "\n\nint umpi_vcode_%s_l_post (umpi_op_t * op)\n{\n", name);
  fprintf (outverifyFile, "int rc = 0;\n");
  fprintf (outverifyFile, "assert(op);\n");
      
  return 0;
}


int
RecordAsynchLocalPost (char *name)
{
  int i, needed = 0;

  if (vcode_l_post[0]) {
    needed = 1;
    RecordAsynchLocalPostStart (name);

    for (i = 0; vcode_l_post[i]; i++) {
      fprintf (outverifyFile, "%s\n", vcode_l_post[i]);
    }
  }

  if (AL_Test("resource_constructor") && !AL_Test("comm_constructor")) {
    /* handle the postop part of resource tracking... */
    /* not yet refined; need to slice up umpi_mpi_resources.c... */
    /* will actually move to asynch_local_pre... */
    if (!needed) {
      needed = 1;
      RecordAsynchLocalPostStart (name);
    }
    
    if (AL_Test("type_constructor"))
      fprintf (outverifyFile, 
	       "rc |= umpi_track_resources_type_constructor (op);\n");
    else if (AL_Test("group_constructor"))
      fprintf (outverifyFile, 
	       "rc |= umpi_track_resources_group_constructor (op);\n");
    else if (strcmp (name, "MPI_Op_create") == 0)
      fprintf (outverifyFile, 
	       "rc |= umpi_track_resources_op_create (op);\n");
    else if (strcmp (name, "MPI_Errhandler_create") == 0)
      fprintf (outverifyFile, 
	       "rc |= umpi_track_resources_errhandler_create (op);\n");
    else {
      fprintf (stderr, "\n\t\tWARNING: Unexcpected resource constructor");
      fprintf (outverifyFile, "assert (0);\n");
    }
  }

  if (needed) {
    fprintf (outverifyFile, "return rc;\n");
    fprintf (outverifyFile, "} /* umpi_vcode_%s_l_post */\n\n", name);
    fprintf (outverifyHFile, 
	     "\nextern int umpi_vcode_%s_l_post (umpi_op_t * op);", name);
  }

  return needed;
}


int
RecordGlobalPreStart (char *name)
{
  fprintf (mgrverifyFile, 
	   "\n\nint umpi_vcode_%s_g_pre (umpi_op_t * op)\n{\n", name);
  fprintf (mgrverifyFile, "int rc = 0;\n");
  fprintf (mgrverifyFile, "assert(op);\n");

  return 0;
}


int
RecordGlobalPre (char *name)
{
  int i, needed = 0;

  if (vcode_g_pre[0]) {
    needed = 1;
    RecordGlobalPreStart (name);

    for (i = 0; vcode_g_pre[i]; i++) {
      fprintf (mgrverifyFile, "%s\n", vcode_g_pre[i]);
    }
  }

  /* handle the preop part of tracking the */
  /* create_op -> start_op -> completion associations... */
  /* also take care of finding and reserving type commit ops... */
  if (AL_Test("persistent_start")) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }
    
    fprintf (mgrverifyFile, 
	     "rc |= umpi_match_requests_persistent_start (op);\n");
  }
  else if (AL_Test("persistent_init")) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }
    
    fprintf (mgrverifyFile, 
	     "rc |= umpi_match_requests_persistent_init (op);\n");
  }
  else if (AL_Test("nonblocking")) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }
    
    fprintf (mgrverifyFile, 
	     "rc |= umpi_match_requests_nonblocking_transient (op);\n");
  }
  else if (AL_Test("recv")) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }

    fprintf (mgrverifyFile, 
	     "/* need to find type_commit_op and reserve it... */\n");
    fprintf (mgrverifyFile, 
	     "rc |= umpi_mgr_get_comm_typemap (op);\n");

    if (AL_Test("sendrecv")) {
      fprintf (mgrverifyFile, 
	       "/* need to find recvtype_commit_op and reserve it... */\n");
      fprintf (mgrverifyFile, 
	       "rc |= umpi_mgr_get_comm_recvtypemap (op);\n");
    }
        
    fprintf (mgrverifyFile, 
	     "if (op->data.mpi.source == MPI_ANY_SOURCE) {\n");
    fprintf (mgrverifyFile, 
	     "/* save it for post op that fills in the actual source */\n");
    fprintf (mgrverifyFile, "umpi_g_save_for_postop_processing (op);\n");
    fprintf (mgrverifyFile, "}\n\n");
  }
  else if (AL_Test("send")) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }

    fprintf (mgrverifyFile, 
	     "/* need to find type_commit_op and reserve it... */\n");
    fprintf (mgrverifyFile, 
	     "rc |= umpi_mgr_get_comm_typemap (op);\n");
  }
  else if (AL_Test("request_free")) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }
    
    fprintf (mgrverifyFile, "rc |= umpi_match_request_free (op);\n");
  }
  else if (AL_Test("completion")) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }
      
    fprintf (mgrverifyFile, "rc |= umpi_match_requests_completion (op);\n");
  }
  else if ((strcmp (name, "MPI_Scatter") == 0) ||
	   (strcmp (name, "MPI_Scatterv") == 0)) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }
      
    /* Scatter and scatterv are odd since datatype is only valid at root */
    /* while recvtype is valid everywhere; set up appropriate reservations */
    fprintf (mgrverifyFile, 
	     "/* need to find recvtype_commit_op and reserve it... */\n");
    fprintf (mgrverifyFile, 
	     "rc |= umpi_mgr_get_comm_recvtypemap (op);\n");
    fprintf (mgrverifyFile, 
	     "/* need to reserve type_commit_op at root... */\n");
    fprintf (mgrverifyFile, 
	     "rc |= umpi_mgr_get_root_type_commit_op (op);\n");
  }
  else if (AL_Test("collective") &&
	   !(AL_Test ("comm_constructor") ||
	     (strcmp (name, "MPI_Barrier") == 0) ||
	     (strcmp (name, "MPI_Comm_free") == 0) ||
	     (strcmp (name, "MPI_Finalize") == 0) ||
	     (strcmp (name, "MPI_Cart_map") == 0) ||
	     (strcmp (name, "MPI_Graph_map") == 0))) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }
      
    /* comm_constructors, barriers, comm_frees, finalize, and */
    /* cart_maps and graph_maps have no types to match... */
    fprintf (mgrverifyFile, 
	     "/* need to find type_commit_op and reserve it... */\n");
    fprintf (mgrverifyFile, 
	     "rc |= umpi_mgr_get_comm_typemap (op);\n");

    if ((strcmp (name, "MPI_Allgather") == 0) ||
	(strcmp (name, "MPI_Alltoall") == 0) ||
	(strcmp (name, "MPI_Allgatherv") == 0) ||
	(strcmp (name, "MPI_Alltoallv") == 0)) {
      fprintf (mgrverifyFile, 
	       "/* need to find recvtype_commit_op and reserve it... */\n");
      fprintf (mgrverifyFile, 
	       "rc |= umpi_mgr_get_comm_recvtypemap (op);\n");
    }
    else if ((strcmp (name, "MPI_Gather") == 0) ||
	     (strcmp (name, "MPI_Gatherv") == 0)) {
      fprintf (mgrverifyFile, 
	       "/* need to reserve recvtype_commit_op at root... */\n");
      fprintf (mgrverifyFile, 
	       "rc |= umpi_mgr_get_root_recvtype_commit_op (op);\n");
    }
  }

  /* handle the preop part of message matching... */
  /* we don't match persistent inits directly but need comm translated... */
  if (AL_Test("persistent_init")) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }
    
    fprintf (mgrverifyFile, 
	     "op->umpi_comm_trans = umpi_translate_comm (op);\n");
  }
  else if (AL_Test("collective") ||
	   AL_Test("persistent_start") ||
	   AL_Test("send") || AL_Test("recv")) {
    /* not yet refined; need to slice up umpi_mpi_match.c... */
    /* need to slice up the umpi_dependent_global_ranks function... */
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }
    
    fprintf (mgrverifyFile, "rc |= umpi_match_messages (op);\n");
  }

  if (AL_Test("comm_constructor")) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }
    
    if (strcmp (name, "MPI_Comm_split") == 0)
      fprintf (mgrverifyFile, "rc |= umpi_handle_comm_split_pre (op);\n");
    else if (strcmp (name, "MPI_Cart_sub") == 0)
      fprintf (mgrverifyFile, "rc |= umpi_handle_comm_cart_sub_pre (op);\n");
    else if (strcmp (name, "MPI_Intercomm_create") == 0)
      fprintf (mgrverifyFile, 
	      "rc |= umpi_handle_intercomm_create_pre (op);\n");
    else
      fprintf (mgrverifyFile, "rc |= umpi_handle_comms_pre (op);\n");
      
  }
  else if (strcmp (name, "MPI_Comm_free") == 0) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }
    
    fprintf (mgrverifyFile, "rc |= umpi_handle_comm_free (op);\n");
  }

  if (strcmp (name, "MPI_Type_commit") == 0) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }
    
    fprintf (mgrverifyFile, "rc |= umpi_mgr_save_typemap (op);\n");
  }

  if (strcmp (name, "MPI_Type_free") == 0) {
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }
    
    fprintf (mgrverifyFile, 
	     "rc |= umpi_mgr_free_typemap (op, 0 /* FALSE */ );\n");
  }

  if (AL_Test("candeadlock")) {
    /* check for deadlock... */
    /* not yet refined; need to slice up umpi_mpi_req.c... */
    if (!needed) {
      needed = 1;
      RecordGlobalPreStart (name);
    }

    fprintf (mgrverifyFile, "rc |= umpi_verify_deadlock (op);\n");
    fprintf (mgrverifyFile, "if (rc & UMPIERR_DEADLOCK)\n");
    fprintf (mgrverifyFile, "print_deadlock();\n\n");
  }
      
  if (needed) {
    fprintf (mgrverifyFile, "return rc;\n");
    fprintf (mgrverifyFile, "} /* umpi_vcode_%s_g_pre */\n\n", name);
    fprintf (mgrverifyHFile, 
	     "\nextern int umpi_vcode_%s_g_pre (umpi_op_t * op);", name);
  }

  return needed;
}


int
RecordGlobalPostStart (char *name)
{
  fprintf (mgrverifyFile, 
	   "\n\nint umpi_vcode_%s_g_post (umpi_op_t * op)\n{\n", name);
  fprintf (mgrverifyFile, "int rc = 0;\n");
  fprintf (mgrverifyFile, "assert(op);\n");

  return 0;
}


int
RecordGlobalPost (char *name)
{
  int i, needed = 0;

  if (vcode_g_post[0]) {
    needed = 1;
    RecordGlobalPostStart (name);

    for (i = 0; vcode_g_post[i]; i++) {
      fprintf (mgrverifyFile, "%s\n", vcode_g_post[i]);
    }
  }

  if (AL_Test("comm_constructor")) {
    if (!needed) {
      needed = 1;
      RecordGlobalPostStart (name);
    }
    
    if (strcmp (name, "MPI_Comm_dup") == 0)
      fprintf (mgrverifyFile, "rc |= umpi_handle_comm_dup_post (op);\n");
    else if (strcmp (name, "MPI_Cart_sub") == 0)
      fprintf (mgrverifyFile, "rc |= umpi_handle_comm_cart_sub_post (op);\n");
    else if (strcmp (name, "MPI_Intercomm_create") == 0)
      fprintf (mgrverifyFile, 
	      "rc |= umpi_handle_intercomm_create_post (op);\n");
    else
      fprintf (mgrverifyFile, "rc |= umpi_handle_comms_post (op);\n");
  }

  if (AL_Test("persistent_init")) {
    /* handle the postop part of request tracking... */
    if (!needed) {
      needed = 1;
      RecordGlobalPostStart (name);
    }
    
    fprintf (mgrverifyFile, 
	     "rc |= umpi_get_request_handles_persistent_init (op);\n");
  }
  else if (AL_Test("nonblocking") && !AL_Test("persistent_start")) {
    /* handle the postop part of request tracking... */
    if (!needed) {
      needed = 1;
      RecordGlobalPostStart (name);
    }
    
    fprintf (mgrverifyFile, 
	     "rc |= umpi_get_request_handles_nonblocking (op);\n");
  }
    
  /* REALLY WANT TO CHECK IF PARTIAL OR ANY SOURCE AT TASK */
  /* AND POSTOP SEND TO MANAGER ONLY IF IT IS; HOW TO IMPLEMENT? */
  if (AL_Test("completion") || 
      (AL_Test("recv") && !AL_Test("nonblocking"))) {
    /* handle the postop part of partial and any source completions... */
    if (!needed) {
      needed = 1;
      RecordGlobalPostStart (name);
    }

    fprintf (mgrverifyFile, 
	    "rc |= umpi_finish_partial_or_any_source_completions (op);\n");

    /* although not ideal, umpi_finish_partial_or_any_source_completions */
    /* performs deadlock checking on any source receive completions so */
    /* it can return an error that indicates a deadlock... */
    fprintf (mgrverifyFile, "if (rc & UMPIERR_DEADLOCK)\n");
    fprintf (mgrverifyFile, "print_deadlock();\n\n");
  }

  if (needed) {
    fprintf (mgrverifyFile, "return rc;\n");
    fprintf (mgrverifyFile, "} /* umpi_vcode_%s_g_post */\n\n", name);
    fprintf (mgrverifyHFile, 
	     "\nextern int umpi_vcode_%s_g_post (umpi_op_t * op);", name);
  }

  return needed;
}


void
RecordVerificationCode (char *name)
{
  int need_asynch_local_pre, need_asynch_local_post;
  int need_global_pre, need_global_post;
  int omit_pre = AL_Test ("noasynchpre");
  int omit_post = AL_Test ("noasynchpost");

  need_asynch_local_pre = RecordAsynchLocalPre (name);

  if (need_asynch_local_pre && omit_pre) {
    /* print warning */
    fprintf (stderr, 
	     "\n\t\tWARNING: Asynchronous local preprocessing not called\n");
   
    /* record warning comment(s)... */
    fprintf (outverifyFile, 
	     "\n/*------------ Wrapper not calling preop processing */\n");
  }
  else if (!need_asynch_local_pre) {
    AL_Set ("noasynchlocalpre");
  }

  need_global_pre = RecordGlobalPre (name);

  if (need_global_pre && omit_pre) {
    /* print warning */
    fprintf (stderr, 
	     "\n\t\tWARNING: Global preprocessing not called\n");
   
    /* record warning comment(s)... */
    fprintf (mgrverifyFile, 
	     "\n/*------------ Wrapper not calling preop processing */\n");
  }
  else if (!need_global_pre) {
    AL_Set ("noasynchglobalpre");
  }

  /* should omit if none exists... */
  if (!(need_asynch_local_pre || need_global_pre)) {
    AL_Set ("noasynchpre");
  }

  need_asynch_local_post = RecordAsynchLocalPost (name);

  if (need_asynch_local_post && omit_post) {
    /* print warning */
    fprintf (stderr, 
	     "\n\t\tWARNING: Asynchronous local postprocessing not called\n");
   
    /* record warning comment(s)... */
    fprintf (outverifyFile, 
	     "\n/*------------ Wrapper not calling postop processing */\n");
  }
  else if (!need_asynch_local_post) {
    AL_Set ("noasynchlocalpost");
  }

  need_global_post = RecordGlobalPost (name);

  if (need_global_post && omit_post) {
    /* print warning */
    fprintf (stderr, 
	     "\n\t\tWARNING: Global postprocessing not called\n");
   
    /* record warning comment(s)... */
    fprintf (mgrverifyFile, 
	     "\n/*------------ Wrapper not calling postop processing */\n");
  }

  /* should omit if none exists... */
  if (!(need_asynch_local_post || need_global_post)) {
    AL_Set ("noasynchpost");
  }
  else if (!need_global_post) {
    AL_Set ("noasynchglobalpost");
  }

  return;
}


void
RecordTypeMatchOp (char *name, char *fname_buf)
{
  if (AL_Test ("persistent_start") ||
      ((AL_Test ("send") ||
	AL_Test ("recv")) &&
       !AL_Test ("persistent_init"))) {
    sprintf (fname_buf, "umpi_check_pt2pt_type_match");
  }
  else if (!AL_Test ("collective") || 
      AL_Test ("comm_constructor") ||
      (strcmp (name, "MPI_Barrier") == 0) ||
      (strcmp (name, "MPI_Comm_free") == 0) ||
      (strcmp (name, "MPI_Finalize") == 0) ||
      (strcmp (name, "MPI_Cart_map") == 0) ||
      (strcmp (name, "MPI_Graph_map") == 0)) {
    /* point to point ops are handled differently; collectives */
    /* are easier since the op is the same on both sides */
    /* comm_constructors, barriers, comm_frees, finalize, and */
    /* cart_maps and graph_maps have no types to match... */
    sprintf (fname_buf, "NULL");
  }
  else if ((strcmp (name, "MPI_Allgather") == 0) ||
	   (strcmp (name, "MPI_Alltoall") == 0)) {
    sprintf (fname_buf, "umpi_check_alltoall_type_match");
  }
  else if (strcmp (name, "MPI_Allgatherv") == 0) {
    sprintf (fname_buf, "umpi_check_allgatherv_type_match");
  }
  else if (strcmp (name, "MPI_Alltoallv") == 0) {
    sprintf (fname_buf, "umpi_check_alltoallv_type_match");
  }
  else if (strcmp (name, "MPI_Bcast") == 0) {
    sprintf (fname_buf, "umpi_check_bcast_type_match");
  }
  else if (strcmp (name, "MPI_Gather") == 0) {
    sprintf (fname_buf, "umpi_check_gather_type_match");
  }
  else if (strcmp (name, "MPI_Gatherv") == 0) {
    sprintf (fname_buf, "umpi_check_gatherv_type_match");
  }
  else if (strcmp (name, "MPI_Scatter") == 0) {
    sprintf (fname_buf, "umpi_check_scatter_type_match");
  }
  else if (strcmp (name, "MPI_Scatterv") == 0) {
    sprintf (fname_buf, "umpi_check_scatterv_type_match");
  }
  else if ((strcmp (name, "MPI_Reduce") == 0) ||
	   (strcmp (name, "MPI_Allreduce") == 0) ||
	   (strcmp (name, "MPI_Scan") == 0)) {
    sprintf (fname_buf, "umpi_check_reduce_type_match");
  }
  else if (strcmp (name, "MPI_Reduce_scatter") == 0) {
    sprintf (fname_buf, "umpi_check_reducescatter_type_match");
  }
  else {
    assert (0);
  }

  return;
}


void
RecordLocalTypeMatchOp (char *name, char *fname_buf)
{
  if (!AL_Test ("collective") || 
      AL_Test ("comm_constructor") ||
      (strcmp (name, "MPI_Barrier") == 0) ||
      (strcmp (name, "MPI_Comm_free") == 0) ||
      (strcmp (name, "MPI_Finalize") == 0) ||
      (strcmp (name, "MPI_Cart_map") == 0) ||
      (strcmp (name, "MPI_Graph_map") == 0)) {
    /* point to point ops are handled differently; collectives */
    /* are easier since the op is the same on both sides */
    /* comm_constructors, barriers, comm_frees, finalize, and */
    /* cart_maps and graph_maps have no types to match... */
    sprintf (fname_buf, "NULL");
  }
  else if ((strcmp (name, "MPI_Allgather") == 0) ||
	   (strcmp (name, "MPI_Alltoall") == 0)) {
    sprintf (fname_buf, "umpi_check_local_alltoall_type_match");
  }
  else if (strcmp (name, "MPI_Allgatherv") == 0) {
    sprintf (fname_buf, "umpi_check_local_allgatherv_type_match");
  }
  else if (strcmp (name, "MPI_Alltoallv") == 0) {
    sprintf (fname_buf, "umpi_check_local_alltoallv_type_match");
  }
  else if (strcmp (name, "MPI_Bcast") == 0) {
    sprintf (fname_buf, "umpi_check_local_bcast_type_match");
  }
  else if (strcmp (name, "MPI_Gather") == 0) {
    sprintf (fname_buf, "umpi_check_local_gather_type_match");
  }
  else if (strcmp (name, "MPI_Gatherv") == 0) {
    sprintf (fname_buf, "umpi_check_local_gatherv_type_match");
  }
  else if (strcmp (name, "MPI_Scatter") == 0) {
    sprintf (fname_buf, "umpi_check_local_scatter_type_match");
  }
  else if (strcmp (name, "MPI_Scatterv") == 0) {
    sprintf (fname_buf, "umpi_check_local_scatterv_type_match");
  }
  else if ((strcmp (name, "MPI_Reduce") == 0) ||
	   (strcmp (name, "MPI_Allreduce") == 0) ||
	   (strcmp (name, "MPI_Scan") == 0) ||
	   (strcmp (name, "MPI_Reduce_scatter") == 0)) {
    sprintf (fname_buf, "umpi_check_local_reduce_type_match");
  }
  else {
    assert (0);
  }

  return;
}


void
RecordTypeResOp (char *name, char *fname_buf)
{
  if (!AL_Test ("collective") || 
      AL_Test ("comm_constructor") ||
      (strcmp (name, "MPI_Barrier") == 0) ||
      (strcmp (name, "MPI_Comm_free") == 0) ||
      (strcmp (name, "MPI_Finalize") == 0) ||
      (strcmp (name, "MPI_Cart_map") == 0) ||
      (strcmp (name, "MPI_Graph_map") == 0)) {
    /* point to point ops are handled differently; collectives */
    /* are easier since the op is the same on both sides */
    /* comm_constructors, barriers, comm_frees, finalize, and */
    /* cart_maps and graph_maps have no types to match... */
    sprintf (fname_buf, "NULL");
  }
  else if ((strcmp (name, "MPI_Allgather") == 0) ||
	   (strcmp (name, "MPI_Alltoall") == 0) ||
	   (strcmp (name, "MPI_Allgatherv") == 0) ||
	   (strcmp (name, "MPI_Alltoallv") == 0)) {
    sprintf (fname_buf, "umpi_alltoall_type_res");
  }
  else if (strcmp (name, "MPI_Bcast") == 0) {
    sprintf (fname_buf, "umpi_bcast_type_res");
  }
  else if ((strcmp (name, "MPI_Gather") == 0) ||
	   (strcmp (name, "MPI_Gatherv") == 0)) {
    sprintf (fname_buf, "umpi_gather_type_res");
  }
  else if ((strcmp (name, "MPI_Scatter") == 0) ||
	   (strcmp (name, "MPI_Scatterv") == 0)) {
    sprintf (fname_buf, "umpi_scatter_type_res");
  }
  else if ((strcmp (name, "MPI_Reduce") == 0) ||
	   (strcmp (name, "MPI_Allreduce") == 0) ||
	   (strcmp (name, "MPI_Scan") == 0) || 
	   (strcmp (name, "MPI_Reduce_scatter") == 0)) {
    sprintf (fname_buf, "umpi_reduce_type_res");
  }
  else {
    assert (0);
  }

  return;
}

  
/* eof */
