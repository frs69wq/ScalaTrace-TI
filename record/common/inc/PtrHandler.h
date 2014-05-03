/*
 * Cop	yright (c) 2007 - May 2, 2014:
 * Prasun Ratn <prasun@ncsu.edu>, Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef __PTR_HANDLER_H__
#define __PTR_HANDLER_H__

#include <mpi.h>
//#include "opstruct.h"

#ifdef __cplusplus
extern "C" {
#endif

  /* This file maps MPI datatypes to indices in an internal lookup table.
   * While implementations like MPICH do this internally, the MPI standard
   * allows any sort of opaque handle to be used for datatypes (e.g. in
   * OpenMPI), so we do our own mapping here.
   */
#define PTR_HANDLER_SZ 1000

  /* This struct is used to hold lookup tables for user-defined datatypes 
   * and communicators.  It needs to be inited by init_ptr and then passed
   * through to the various handle-management routines below.
   */
  struct ptr_handler;
  typedef struct ptr_handler *ptr_handler_t;


  extern ptr_handler_t record_ptr;


  /* This needs to be called first, before any of the other routines, as
   * it sets up the internal mappings used by lookups below.
   * 
   * ptr_handler object returned by this should be used to perform
   * lookups using the other routines.
   */
  ptr_handler_t init_ptr(int size);

  /* Frees a ptr_handler_t returned by init_ptr. */
  void finalize_ptr(ptr_handler_t);

  /* TODO: Docs for below routines. */ 
  int type_to_index(ptr_handler_t ptr, MPI_Datatype type);
  int comm_to_index(ptr_handler_t ptr, MPI_Comm comm);
  int comm_current(ptr_handler_t ptr);
  int comm_bufsize(ptr_handler_t ptr);
  int comm_abs_index(ptr_handler_t ptr, int index);
  int group_to_index(ptr_handler_t ptr, MPI_Group group);
  int op_to_index(ptr_handler_t ptr, MPI_Op op);

  MPI_Datatype index_to_type(ptr_handler_t ptr, int index);
  MPI_Comm index_to_comm(ptr_handler_t ptr, int index);
  MPI_Group index_to_group(ptr_handler_t ptr, int index);
  MPI_Op index_to_op (ptr_handler_t ptr, int index);

  void add_comm_entry(ptr_handler_t ptr, MPI_Comm comm);
  void add_type_entry(ptr_handler_t ptr, MPI_Datatype type);
  void add_group_entry(ptr_handler_t ptr, MPI_Group group);
  void add_op_entry(ptr_handler_t ptr, MPI_Op op);
  
  void remove_comm_entry(ptr_handler_t ptr, MPI_Comm comm);
  void remove_type_entry(ptr_handler_t ptr, MPI_Datatype type);
  void remove_group_entry(ptr_handler_t ptr, MPI_Group group);
  void remove_op_entry(ptr_handler_t ptr, MPI_Op op);

  /* functions for i/o tracing */
  int file_handle_to_index(ptr_handler_t ptr, MPI_File file);
  MPI_File index_to_file_handle(ptr_handler_t ptr, int index);
  void add_file_handle_entry(ptr_handler_t ptr, MPI_File file);
  void remove_file_handle_entry(ptr_handler_t ptr, MPI_File file);

#ifdef __cplusplus
                       }
#endif 

#endif /* __PTR_HANDLER_H__  */
  /* vim: set tw=80 sw=2 expandtab: */
