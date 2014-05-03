#include <stdlib.h>
#include <stdio.h>
#include "ReqHandler.h"
#include <assert.h>

req_handler_t record_req;

void init_req(req_handler_t * r, int size)
{
	req_handler_t req;
	int i;

	req = (req_handler_t) malloc (sizeof(struct req_handler));

	req->req_bufcur = 0;
	req->req_bufsize = size;
	req->req_buf = (MPI_Request *) malloc(sizeof(MPI_Request) * req->req_bufsize);
	for(i = 0; i < req->req_bufsize; i++)
		req->req_buf[i] = MPI_REQUEST_NULL;

	*r = req;
}

void add_request_entry(req_handler_t req, MPI_Request request)
{
	if(request != MPI_REQUEST_NULL) {
		req->req_buf[req->req_bufcur] = request;
		req->req_bufcur++;
		if(req->req_bufcur >= req->req_bufsize)
			req->req_bufcur = 0;
	} else {
		fprintf (stderr, "error add_request_entry: request NULL\n");
	}
}

void finalize_req(req_handler_t req)
{
	free(req->req_buf);  
	req->req_buf = NULL;
	req->req_bufsize = 0;
	req->req_bufcur = 0;
	free(req);
}

void reset_offset (req_handler_t req, int offset)
{
	// don't reset offset if request is MPI_REQUEST_NULL
	if (offset < 0) {
		return;
	}

	assert (req);
	assert (req->req_bufsize > 0);

	int i = (req->req_bufcur + offset)%req->req_bufsize ;

	req->req_buf[i] = MPI_REQUEST_NULL;

	return;
} 


//NOTE: Null request isn't really a request; it's an offset... just make it -1?
void lookup_request(req_handler_t req, int offset, int null_request, MPI_Request *request) {
	if(offset != null_request) {
		*request = req->req_buf[(req->req_bufcur + offset) % req->req_bufsize];

		if(*request == MPI_REQUEST_NULL) {
			fprintf(stderr, "[%d,%d]: error: unable to locate request handle offset=%d,"
						"null_request=%d \n", __LINE__,my_rank, offset, null_request);
			exit(1);
		}
	} else {
		*request = MPI_REQUEST_NULL;
	}
}

void lookup_requestlist(req_handler_t req, int *offsets, int count, int null_request, MPI_Request *requests){
	int i;

	for(i = 0; i < count; i++) {
		assert(null_request == -1);
		if(offsets[i] != null_request) {
			requests[i] = req->req_buf[(req->req_bufcur + offsets[i]) % req->req_bufsize];
			req->req_buf[(req->req_bufcur + offsets[i]) % req->req_bufsize] = MPI_REQUEST_NULL;

			if(requests[i] == MPI_REQUEST_NULL) {
				fprintf(stderr, "[%d,%d]: error: unable to locate request handle offset=%d,"
							"null_request=%d \n", __LINE__, my_rank, offsets[i], null_request);
				exit(1);
			}
		} else {
			requests[i] = MPI_REQUEST_NULL;
		}
	}
}


void lookup_requestlist_waitsome(req_handler_t req,int *offsets, int count, int null_request, MPI_Request *requests) {
	int i;

	for(i = 0; i < count; i++) {
		if(offsets[i] != null_request) {
			requests[i] = req->req_buf[(req->req_bufcur + offsets[i]) % req->req_bufsize];
		} else {
			requests[i] = MPI_REQUEST_NULL;
		}
	}
}

void lookup_offset (req_handler_t req, MPI_Request request, int *offset)
{
	int i;
	*offset = -1;

	// MPI_REQUEST_NULL is just -1
	if(request == MPI_REQUEST_NULL) return;

	// search for request in the buffer.
	for(i = 0; i < req->req_bufsize && request != req->req_buf[i]; i++);
    
	if (i == req->req_bufsize) { // check if found
		fprintf(stderr, "%d: %d error: unable to locate request handle "
				"(allocate additional space for the request buffer)\n", my_rank, __LINE__);
		exit(1);
	}

	*offset = (i - req->req_bufcur + req->req_bufsize) % req->req_bufsize;
}



void lookup_offsetlist (req_handler_t req, MPI_Request *request, int count, int* output, int reset){
	int i, j, k;
	int offset;

	for(i = 0; i < count; i++) {
		output[i] = -1;
		if(request[i] != MPI_REQUEST_NULL) {
			for(j = 0; j < req->req_bufsize; j++) {
				k = (req->req_bufcur - j + req->req_bufsize) % req->req_bufsize;
				if(request[i] == req->req_buf[k]) {
					offset = (k - req->req_bufcur + req->req_bufsize) % req->req_bufsize;
					output[i] = offset;
					if(reset)
		    			req->req_buf[k] = MPI_REQUEST_NULL;
					break;
				}
			}

			if(output[i] == -1) {
				fprintf(stderr, "%d: error: unable to locate request handle \
								(allocate additional space for the request buffer)\n", my_rank);
				exit(0);
			}
		} 	
	}
}

