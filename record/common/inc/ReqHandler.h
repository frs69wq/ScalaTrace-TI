/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef REQ_HANDLER_H
#define REQ_HANDLER_H

#include <stdio.h>
#include <mpi.h>

extern int my_rank;

#ifdef __cplusplus
extern "C" {
#endif

#define REQ_BUF_SIZE 10000

typedef struct req_handler{
  MPI_Request *req_buf;
  int req_bufsize;
  int req_bufcur;
} * req_handler_t;

extern req_handler_t record_req;

void init_req(req_handler_t * req,int size);
void add_request_entry(req_handler_t req, MPI_Request request);
void finalize_req(req_handler_t req);
void reset_offset (req_handler_t req, int offset);

void lookup_request(req_handler_t req, int offset, int null_request, MPI_Request *request);
void lookup_requestlist(req_handler_t req, int *offsets, int count, int null_request, MPI_Request *requests);
void lookup_requestlist_waitsome(req_handler_t req, int *offsets, int count, int null_request, MPI_Request *requests);

void lookup_offset(req_handler_t req, MPI_Request request, int *offset);
void lookup_offsetlist(req_handler_t req, MPI_Request *requests, int count, int* output, int reset);

#ifdef __cplusplus
}
#endif
#endif
