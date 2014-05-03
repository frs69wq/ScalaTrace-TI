/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef _UMPIRE_ERRNO_H
#define _UMPIRE_ERRNO_H

typedef unsigned umpi_errno;

/* Power of two values for error codes allows bit-wise combining */
/* and recovery so return value can encode multiple error codes */

#define UMPIERR_OK 0

#define UMPIERR_FATAL (1)

#define UMPIERR_LEAK_COMM (2)
#define UMPIERR_LEAK_TYPE (4)
#define UMPIERR_LEAK_GROUP (8)
#define UMPIERR_LEAK_REDUCE_OP (16)
#define UMPIERR_LEAK_ERRHANDLER (32)
#define UMPIERR_LOST_REQUEST (64)
#define UMPIERR_REDUNDANT_FREE (128)
#define UMPIERR_MOD_SEND_BUF (256)
#define UMPIERR_TYPE_SIG_MISMATCH (512)
#define UMPIERR_DEADLOCK ( 1024 | UMPIERR_FATAL )
#define UMPIERR_BAD_HANDLE ( 2048 | UMPIERR_FATAL )
#define UMPIERR_MPIERR ( 4096 | UMPIERR_FATAL )
#define UMPIERR_INTERNAL ( 8192 | UMPIERR_FATAL )
#define UMPIERR_INSUFFICIENT_BUF ( 16384 )


#endif

/* eof */
