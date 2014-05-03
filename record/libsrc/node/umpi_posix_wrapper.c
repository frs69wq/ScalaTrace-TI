/* Umpire wrappers for all traceable POSIX functions */

/* !!!!! This file is automatically generated by a wrapper-engine !!!!! */

#include <setjmp.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include "umpi_internal.h"

#include "umpi_pc.h"

extern int is_mpi_inited;

/*--------------------------------------------- write */

extern ssize_t  __real_write(  int fd ,
		const void *buf ,
		size_t count );

static int
posix_write_immediate_local_pre(umpi_op_t *op)
{

{
{
/*
  init_op (&operation);
  init_event(event);
  recordEventTime(event, 0);
  recordTime (&operation, 0);
*/
}
}
return 0;
}


static int
posix_write_pre( 
void * pc , int fd ,
const void *buf ,
size_t count )

{
int rc = 0;
umpi_op_t *uop = (umpi_op_t *)calloc(sizeof(umpi_op_t), 1);

assert(uop);
uop->op = umpi_write;
uop->rank = umpi_rank;
uop->seq_num = umpi_task_seq_num++;
uop->pc = pc;
		uop->data.mpi.fd = fd;
uop->data.mpi.buf = (void*)buf;
		uop->data.mpi.count = count;
umpi_inc_ref_count (uop);
#ifdef UMPI_DEBUG_LEVEL_2
q_append (umpi_task_op_q, uop);
#endif

uop->order = umpi_preop;

rc = posix_write_immediate_local_pre(uop);
umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);
return 0;
}

static int
posix_write_immediate_local_post(umpi_op_t *op)
{

{
{
/*
  operation.fields = COUNT;
  event_set_param_list(event, op->op, op->seq_num, COUNT | FH);
  operation.fields_extn = FH;
  operation.op = (MPI_Op_t)op->op;
  operation.seq_num = op->seq_num;
  operation.fh = file_handle_to_index (record_ptr,  op->data.mpi.fd);
  add_scalar_param_int(event, TYPE_FH, file_handle_to_index (record_ptr,  op->data.mpi.fd));
  operation.count = op->data.mpi.count;
  add_scalar_param_int(event, TYPE_COUNT, op->data.mpi.count);
  recordTime (&operation, 1);
  recordEventTime(event, 1);
  if(operation.fh>0)
  compress_rsd_event( &op_queue, &operation, event);
  recordEventTime(0,0);
recordTime (0, 0);
*/
}
}
return 0;
}


static int
posix_write_post( 
int MPI_rc, 
void * pc , int fd ,
const void *buf ,
size_t count )

{
int rc = 0;
umpi_op_t *uop = (umpi_op_t *)calloc(sizeof(umpi_op_t), 1);

assert(uop);
uop->op = umpi_write;
uop->rank = umpi_rank;
uop->seq_num = umpi_task_seq_num++;
uop->pc = pc;
		uop->data.mpi.fd = fd;
uop->data.mpi.buf = (void*)buf;
		uop->data.mpi.count = count;
umpi_inc_ref_count (uop);
#ifdef UMPI_DEBUG_LEVEL_2
q_append (umpi_task_op_q, uop);
#endif

uop->order = umpi_postop;
uop->res = MPI_rc;

rc = posix_write_immediate_local_post(uop);
umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);
return rc;
}

static int
gwrap_write( 
void * pc , int fd ,
const void *buf ,
size_t count )
{
int rc;

if (is_mpi_inited)
posix_write_pre(pc,
			fd,
			buf,
			count);
	rc = __real_write(  (fd),
(buf),
(count));

if (is_mpi_inited)
posix_write_post(rc, pc,
			fd,
			buf,
			count);
	return rc;
}

extern ssize_t  __wrap_write(  int fd ,
		const void *buf ,
		size_t count )
{
	int rc = 0;
	void *pc = 0x0;
	GET_FP;
	//DEFINE_PC;
	//GATHER_PC;
	rc = gwrap_write(pc,
			fd,
			buf,
			count );
	return rc;
}

/*--------------------------------------------- read */

extern ssize_t  __real_read(  int fd ,
		void *buf ,
		size_t count );

static int
posix_read_immediate_local_pre(umpi_op_t *op)
{

{
{
/*
  init_op (&operation);
  init_event(event);
  recordEventTime(event, 0);
  recordTime (&operation, 0);
*/
}
}
return 0;
}


static int
posix_read_pre( 
void * pc , int fd ,
void *buf ,
size_t count )

{
int rc = 0;
umpi_op_t *uop = (umpi_op_t *)calloc(sizeof(umpi_op_t), 1);

assert(uop);
uop->op = umpi_read;
uop->rank = umpi_rank;
uop->seq_num = umpi_task_seq_num++;
uop->pc = pc;
		uop->data.mpi.fd = fd;
uop->data.mpi.buf = (void*)buf;
		uop->data.mpi.count = count;
umpi_inc_ref_count (uop);
#ifdef UMPI_DEBUG_LEVEL_2
q_append (umpi_task_op_q, uop);
#endif

uop->order = umpi_preop;

rc = posix_read_immediate_local_pre(uop);
umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);
return 0;
}

static int
posix_read_immediate_local_post(umpi_op_t *op)
{

{
{
/*
  operation.fields = COUNT;
  event_set_param_list(event, op->op, op->seq_num, COUNT | FH);
  operation.fields_extn = FH;
  operation.op = (MPI_Op_t)op->op;
  operation.seq_num = op->seq_num;
  operation.fh = file_handle_to_index (record_ptr,  op->data.mpi.fd);
  add_scalar_param_int(event, TYPE_FH, file_handle_to_index (record_ptr,  op->data.mpi.fd));
  operation.count = op->data.mpi.count;
  add_scalar_param_int(event, TYPE_COUNT, op->data.mpi.count);
  recordTime (&operation, 1);
  recordEventTime(event, 1);
  if(operation.fh>0)
  compress_rsd_event( &op_queue, &operation, event);
  recordEventTime(0,0);
recordTime (0, 0);
*/
}
}
return 0;
}


static int
posix_read_post( 
int MPI_rc, 
void * pc , int fd ,
void *buf ,
size_t count )

{
int rc = 0;
umpi_op_t *uop = (umpi_op_t *)calloc(sizeof(umpi_op_t), 1);

assert(uop);
uop->op = umpi_read;
uop->rank = umpi_rank;
uop->seq_num = umpi_task_seq_num++;
uop->pc = pc;
		uop->data.mpi.fd = fd;
uop->data.mpi.buf = (void*)buf;
		uop->data.mpi.count = count;
umpi_inc_ref_count (uop);
#ifdef UMPI_DEBUG_LEVEL_2
q_append (umpi_task_op_q, uop);
#endif

uop->order = umpi_postop;
uop->res = MPI_rc;

rc = posix_read_immediate_local_post(uop);
umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);
return rc;
}

static int
gwrap_read( 
void * pc , int fd ,
void *buf ,
size_t count )
{
int rc;

if (is_mpi_inited)
posix_read_pre(pc,
			fd,
			buf,
			count);
	rc = __real_read(  (fd),
(buf),
(count));

if (is_mpi_inited)
posix_read_post(rc, pc,
			fd,
			buf,
			count);
	return rc;
}

extern ssize_t  __wrap_read(  int fd ,
		void *buf ,
		size_t count )
{
	int rc = 0;
	void *pc = 0x0;
	GET_FP;
	//DEFINE_PC;
	//GATHER_PC;
	rc = gwrap_read(pc,
			fd,
			buf,
			count );
	return rc;
}

/*--------------------------------------------- open */

extern int  __real_open(  const char *filename ,
		int flags ,
		mode_t mode );

static int
posix_open_immediate_local_pre(umpi_op_t *op)
{

{
{
/*
  init_op (&operation);
  init_event(event);
  recordEventTime(event, 0);
  recordTime (&operation, 0);
*/
}
}
return 0;
}


static int
posix_open_pre( 
void * pc , const char *filename ,
int flags ,
mode_t mode )

{
int rc = 0;
umpi_op_t *uop = (umpi_op_t *)calloc(sizeof(umpi_op_t), 1);

assert(uop);
uop->op = umpi_open;
uop->rank = umpi_rank;
uop->seq_num = umpi_task_seq_num++;
uop->pc = pc;
uop->data.mpi.filename = (char *) malloc((strlen(filename)+1) * sizeof(char));
assert (uop->data.mpi.filename);
bcopy (filename, uop->data.mpi.filename, (strlen(filename)+1) * sizeof(char));
		uop->data.mpi.flags = flags;
		uop->data.mpi.mode = mode;
umpi_inc_ref_count (uop);
#ifdef UMPI_DEBUG_LEVEL_2
q_append (umpi_task_op_q, uop);
#endif

uop->order = umpi_preop;

rc = posix_open_immediate_local_pre(uop);
umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);
return 0;
}

static int
posix_open_immediate_local_post(umpi_op_t *op)
{

{
{
/*
  operation.fields = NO_FIELDS;
  event_set_param_list(event, op->op, op->seq_num, FILENAME | POSIX_MODE | POSIX_FLAGS);
  operation.fields_extn = FILENAME | POSIX_MODE | POSIX_FLAGS;
  operation.op = op->op;
  operation.seq_num = op->seq_num;
  operation.filepath = (char *)malloc((strlen(op->data.mpi.filename)+1)*sizeof(char));//&(op->data.mpi.filename);
  add_scalar_param_string(event, TYPE_FILENAME, op->data.mpi.filename);
  strcpy(operation.filepath,op->data.mpi.filename);
  //strcpy(operation.filepath,"btio.full.out");
  operation.posixargs.mode = op->data.mpi.mode;
  add_scalar_param_int(event, TYPE_POSIX_MODE, op->data.mpi.mode);
  operation.posixargs.flags = op->data.mpi.flags;
  add_scalar_param_int(event, TYPE_POSIX_FLAGS, op->data.mpi.flags);
  add_file_handle_entry(record_ptr, op->res);
  operation.fh = file_handle_to_index(record_ptr, op->res);
  add_scalar_param_int(event, TYPE_FH, file_handle_to_index (record_ptr,  op->res));
  operation.is_mpi_io = 1;
  char* sep = strrchr(operation.filepath,'/');
  if(sep!=NULL) {
    operation.filename = (char *)malloc((strlen(sep+1)+1)*sizeof(char));//Reason for spe+1 to remove / from filename
    strcpy(operation.filename,sep+1);
    memset(sep+1,'\0',strlen(sep+1));
  }
  else {
    operation.filename = (char *)malloc((strlen(operation.filepath)+1)*sizeof(char));
    strcpy(operation.filename,operation.filepath);
  }
  recordTime (&operation, 1);
  recordEventTime(event, 1);
  compress_rsd_event( &op_queue, &operation, event);
  recordEventTime(0,0);
recordTime (0, 0);
*/
}
}
return 0;
}


static int
posix_open_post( 
int MPI_rc, 
void * pc , const char *filename ,
int flags ,
mode_t mode )

{
int rc = 0;
umpi_op_t *uop = (umpi_op_t *)calloc(sizeof(umpi_op_t), 1);

assert(uop);
uop->op = umpi_open;
uop->rank = umpi_rank;
uop->seq_num = umpi_task_seq_num++;
uop->pc = pc;
uop->data.mpi.filename = (char *) malloc((strlen(filename)+1) * sizeof(char));
assert (uop->data.mpi.filename);
bcopy (filename, uop->data.mpi.filename, (strlen(filename)+1) * sizeof(char));
		uop->data.mpi.flags = flags;
		uop->data.mpi.mode = mode;
umpi_inc_ref_count (uop);
#ifdef UMPI_DEBUG_LEVEL_2
q_append (umpi_task_op_q, uop);
#endif

uop->order = umpi_postop;
uop->res = MPI_rc;

rc = posix_open_immediate_local_post(uop);
umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);
return rc;
}

static int
gwrap_open( 
void * pc , const char *filename ,
int flags ,
mode_t mode )
{
int rc;

if (is_mpi_inited)
posix_open_pre(pc,
			filename,
			flags,
			mode);
	rc = __real_open(  (filename),
(flags),
(mode));

if (is_mpi_inited)
posix_open_post(rc, pc,
			filename,
			flags,
			mode);
	return rc;
}

extern int  __wrap_open(  const char *filename ,
		int flags ,
		mode_t mode )
{
	int rc = 0;
	void *pc = 0x0;
	GET_FP;
	//DEFINE_PC;
	//GATHER_PC;
	rc = gwrap_open(pc,
			filename,
			flags,
			mode );
	return rc;
}

/*--------------------------------------------- open64 */

extern int  __real_open64(  const char *filename ,
		int flags ,
		mode_t mode );

static int
posix_open64_immediate_local_pre(umpi_op_t *op)
{

{
{
/*
  init_op (&operation);
  init_event(event);
  recordEventTime(event, 0);
  recordTime (&operation, 0);
*/
}
}
return 0;
}


static int
posix_open64_pre( 
void * pc , const char *filename ,
int flags ,
mode_t mode )

{
int rc = 0;
umpi_op_t *uop = (umpi_op_t *)calloc(sizeof(umpi_op_t), 1);

assert(uop);
uop->op = umpi_open64;
uop->rank = umpi_rank;
uop->seq_num = umpi_task_seq_num++;
uop->pc = pc;
uop->data.mpi.filename = (char *) malloc((strlen(filename)+1) * sizeof(char));
assert (uop->data.mpi.filename);
bcopy (filename, uop->data.mpi.filename, (strlen(filename)+1) * sizeof(char));
		uop->data.mpi.flags = flags;
		uop->data.mpi.mode = mode;
umpi_inc_ref_count (uop);
#ifdef UMPI_DEBUG_LEVEL_2
q_append (umpi_task_op_q, uop);
#endif

uop->order = umpi_preop;

rc = posix_open64_immediate_local_pre(uop);
umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);
return 0;
}

static int
posix_open64_immediate_local_post(umpi_op_t *op)
{

{
{
/*
  operation.fields = NO_FIELDS;
  event_set_param_list(event, op->op, op->seq_num, FILENAME | POSIX_MODE | POSIX_FLAGS);
  operation.fields_extn = FILENAME | POSIX_MODE | POSIX_FLAGS;
  operation.op = op->op;
  operation.seq_num = op->seq_num;
  operation.filepath = (char *)malloc((strlen(op->data.mpi.filename)+1)*sizeof(char));//&(op->data.mpi.filename);
  add_scalar_param_string(event, TYPE_FILENAME, op->data.mpi.filename);
  strcpy(operation.filepath,op->data.mpi.filename);
  //strcpy(operation.filepath,"btio.full.out");
  operation.posixargs.mode = op->data.mpi.mode;
  add_scalar_param_int(event, TYPE_POSIX_MODE, op->data.mpi.mode);
  operation.posixargs.flags = op->data.mpi.flags;
  add_scalar_param_int(event, TYPE_POSIX_FLAGS, op->data.mpi.flags);
  add_file_handle_entry(record_ptr, op->res);
  operation.fh = file_handle_to_index(record_ptr, op->res);
  add_scalar_param_int(event, TYPE_FH, file_handle_to_index (record_ptr,  op->res));
  operation.is_mpi_io = 1;
  char* sep = strrchr(operation.filepath,'/');
  if(sep!=NULL) {
    operation.filename = (char *)malloc((strlen(sep+1)+1)*sizeof(char));//Reason for spe+1 to remove / from filename
    strcpy(operation.filename,sep+1);
    memset(sep+1,'\0',strlen(sep+1));
  }
  else {
    operation.filename = (char *)malloc((strlen(operation.filepath)+1)*sizeof(char));
    strcpy(operation.filename,operation.filepath);
  }
  recordTime (&operation, 1);
  recordEventTime(event, 1);
  compress_rsd_event( &op_queue, &operation, event);
  recordEventTime(0,0);
recordTime (0, 0);
*/
}
}
return 0;
}


static int
posix_open64_post( 
int MPI_rc, 
void * pc , const char *filename ,
int flags ,
mode_t mode )

{
int rc = 0;
umpi_op_t *uop = (umpi_op_t *)calloc(sizeof(umpi_op_t), 1);

assert(uop);
uop->op = umpi_open64;
uop->rank = umpi_rank;
uop->seq_num = umpi_task_seq_num++;
uop->pc = pc;
uop->data.mpi.filename = (char *) malloc((strlen(filename)+1) * sizeof(char));
assert (uop->data.mpi.filename);
bcopy (filename, uop->data.mpi.filename, (strlen(filename)+1) * sizeof(char));
		uop->data.mpi.flags = flags;
		uop->data.mpi.mode = mode;
umpi_inc_ref_count (uop);
#ifdef UMPI_DEBUG_LEVEL_2
q_append (umpi_task_op_q, uop);
#endif

uop->order = umpi_postop;
uop->res = MPI_rc;

rc = posix_open64_immediate_local_post(uop);
umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);
return rc;
}

static int
gwrap_open64( 
void * pc , const char *filename ,
int flags ,
mode_t mode )
{
int rc;

if (is_mpi_inited)
posix_open64_pre(pc,
			filename,
			flags,
			mode);
	rc = __real_open64(  (filename),
(flags),
(mode));

if (is_mpi_inited)
posix_open64_post(rc, pc,
			filename,
			flags,
			mode);
	return rc;
}

extern int  __wrap_open64(  const char *filename ,
		int flags ,
		mode_t mode )
{
	int rc = 0;
	void *pc = 0x0;
	GET_FP;
	//DEFINE_PC;
	//GATHER_PC;
	rc = gwrap_open64(pc,
			filename,
			flags,
			mode );
	return rc;
}

/*--------------------------------------------- creat */

extern int  __real_creat(  const char *pathname ,
		mode_t mode );

static int
posix_creat_immediate_local_pre(umpi_op_t *op)
{

{
{
/*
  init_op (&operation);
  init_event(event);
  recordEventTime(event, 0);
  recordTime (&operation, 0);
*/
}
}
return 0;
}


static int
posix_creat_pre( 
void * pc , const char *pathname ,
mode_t mode )

{
int rc = 0;
umpi_op_t *uop = (umpi_op_t *)calloc(sizeof(umpi_op_t), 1);

assert(uop);
uop->op = umpi_creat;
uop->rank = umpi_rank;
uop->seq_num = umpi_task_seq_num++;
uop->pc = pc;
uop->data.mpi.pathname = *(pathname);
		uop->data.mpi.mode = mode;
umpi_inc_ref_count (uop);
#ifdef UMPI_DEBUG_LEVEL_2
q_append (umpi_task_op_q, uop);
#endif

uop->order = umpi_preop;

rc = posix_creat_immediate_local_pre(uop);
umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);
return 0;
}

static int
posix_creat_immediate_local_post(umpi_op_t *op)
{

{
{
/*
  operation.fields = NO_FIELDS;
  event_set_param_list(event, op->op, op->seq_num, FILENAME | POSIX_MODE);
  operation.fields_extn = FILENAME | POSIX_MODE;
  operation.op = op->op;
  operation.seq_num = op->seq_num;
  operation.filepath = (char *)malloc((strlen(op->data.mpi.filename)+1)*sizeof(char));//&(op->data.mpi.filename);
  add_scalar_param_string(event, TYPE_FILENAME, op->data.mpi.filename);
  strcpy(operation.filepath,op->data.mpi.filename);
  //strcpy(operation.filepath,"btio.full.out");
  operation.posixargs.mode = op->data.mpi.mode;
  add_scalar_param_int(event, TYPE_POSIX_MODE, op->data.mpi.mode);
  add_file_handle_entry(record_ptr, op->res);
  operation.fh = file_handle_to_index(record_ptr, op->res);
  add_scalar_param_int(event, TYPE_FH, file_handle_to_index (record_ptr,  op->res));
  operation.is_mpi_io = 1;
  char* sep = strrchr(operation.filepath,'/');
  if(sep!=NULL) {
    operation.filename = (char *)malloc((strlen(sep+1)+1)*sizeof(char));//Reason for spe+1 to remove / from filename
    strcpy(operation.filename,sep+1);
    memset(sep+1,'\0',strlen(sep+1));
  }
  else {
    operation.filename = (char *)malloc((strlen(operation.filepath)+1)*sizeof(char));
    strcpy(operation.filename,operation.filepath);
  }
  recordTime (&operation, 1);
  recordEventTime(event, 1);
  compress_rsd_event( &op_queue, &operation, event);
  recordEventTime(0,0);
recordTime (0, 0);
*/
}
}
return 0;
}


static int
posix_creat_post( 
int MPI_rc, 
void * pc , const char *pathname ,
mode_t mode )

{
int rc = 0;
umpi_op_t *uop = (umpi_op_t *)calloc(sizeof(umpi_op_t), 1);

assert(uop);
uop->op = umpi_creat;
uop->rank = umpi_rank;
uop->seq_num = umpi_task_seq_num++;
uop->pc = pc;
uop->data.mpi.pathname = *(pathname);
		uop->data.mpi.mode = mode;
umpi_inc_ref_count (uop);
#ifdef UMPI_DEBUG_LEVEL_2
q_append (umpi_task_op_q, uop);
#endif

uop->order = umpi_postop;
uop->res = MPI_rc;

rc = posix_creat_immediate_local_post(uop);
umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);
return rc;
}

static int
gwrap_creat( 
void * pc , const char *pathname ,
mode_t mode )
{
int rc;

if (is_mpi_inited)
posix_creat_pre(pc,
			pathname,
			mode);
	rc = __real_creat(  (pathname),
(mode));

if (is_mpi_inited)
posix_creat_post(rc, pc,
			pathname,
			mode);
	return rc;
}

extern int  __wrap_creat(  const char *pathname ,
		mode_t mode )
{
	int rc = 0;
	void *pc = 0x0;
	GET_FP;
	//DEFINE_PC;
	//GATHER_PC;
	rc = gwrap_creat(pc,
			pathname,
			mode );
	return rc;
}

/*--------------------------------------------- close */

extern int  __real_close(  int fd );

static int
posix_close_immediate_local_pre(umpi_op_t *op)
{

{
{
/*
  init_op (&operation);
  init_event(event);
  recordEventTime(event, 0);
  operation.fields = NO_FIELDS;
  event_set_param_list(event, op->op, op->seq_num, FH);
  operation.fields_extn = FH;
  operation.op = op->op;
  operation.seq_num = op->seq_num;
  operation.fh = file_handle_to_index(record_ptr, op->data.mpi.fd);
  add_scalar_param_int(event, TYPE_FH, file_handle_to_index(record_ptr, op->data.mpi.fd) );
  remove_file_handle_entry(record_ptr, op->data.mpi.fd);
  operation.is_mpi_io = 1;
  recordTime (&operation, 0);
  recordEventTime(event, 0);
  if(operation.fh > 0)
  compress_rsd_event( &op_queue, &operation, event);
  recordEventTime(0,0);
  recordTime (0, 0);
*/
}
}
return 0;
}


static int
posix_close_pre( 
void * pc , int fd )

{
int rc = 0;
umpi_op_t *uop = (umpi_op_t *)calloc(sizeof(umpi_op_t), 1);

assert(uop);
uop->op = umpi_close;
uop->rank = umpi_rank;
uop->seq_num = umpi_task_seq_num++;
uop->pc = pc;
		uop->data.mpi.fd = fd;
umpi_inc_ref_count (uop);
#ifdef UMPI_DEBUG_LEVEL_2
q_append (umpi_task_op_q, uop);
#endif

uop->order = umpi_preop;

rc = posix_close_immediate_local_pre(uop);
umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);
return 0;
}

static int
posix_close_immediate_local_post(umpi_op_t *op)
{

{
{
/*
  init_op (&operation);
  init_event(event);
  recordEventTime(event, 0);
  recordTime (&operation, 0);
*/
}
}
return 0;
}


static int
posix_close_post( 
int MPI_rc, 
void * pc , int fd )

{
int rc = 0;
umpi_op_t *uop = (umpi_op_t *)calloc(sizeof(umpi_op_t), 1);

assert(uop);
uop->op = umpi_close;
uop->rank = umpi_rank;
uop->seq_num = umpi_task_seq_num++;
uop->pc = pc;
		uop->data.mpi.fd = fd;
umpi_inc_ref_count (uop);
#ifdef UMPI_DEBUG_LEVEL_2
q_append (umpi_task_op_q, uop);
#endif

uop->order = umpi_postop;
uop->res = MPI_rc;

rc = posix_close_immediate_local_post(uop);
umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);
return rc;
}

static int
gwrap_close( 
void * pc , int fd )
{
int rc;

if (is_mpi_inited)
posix_close_pre(pc,
			fd);
	rc = __real_close(  (fd));

if (is_mpi_inited)
posix_close_post(rc, pc,
			fd);
	return rc;
}

extern int  __wrap_close(  int fd )
{
	int rc = 0;
	void *pc = 0x0;
	GET_FP;
	//DEFINE_PC;
	//GATHER_PC;
	rc = gwrap_close(pc,
			fd );
	return rc;
}


/*eof*/