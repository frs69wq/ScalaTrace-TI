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
