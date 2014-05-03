/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef MPI_UTIL_H
#define MPI_UTIL_H

#include <mpi.h>
#include <stdint.h>

#if defined(__cplusplus) && !defined(__STDC_FORMAT_MACROS)
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* ==================================================================================================== */
/* Following declartions/definitions control types of objects in the opstruct and how we should
   deal with them.  Code that uses this header should stick to these abstractions, as actual
   types are subject to change. 
*/
typedef int64_t field_mask_t;     /* Type for the bitmask that tells which fields are valid. */
#define PRIfieldINT PRIx64        /* Proper format string for field mask values. */

typedef int      array_elt_t;     /* Type for the contents of the various saved arrays in the replay op. */
#define PRImpiARR "d"             /* Proper format string for MPI handles. */

typedef intptr_t mpi_hval_t;      /* Type we should convert MPI internal types to when printing out. */
#define PRImpiPTR PRIdPTR         /* Proper format string for MPI handles. */


#define isPowerOf2(num) (!(num & (num-1)))

// takes log of a number known to be a power of 2.
signed char log2pow2(unsigned long long powerOf2);

  /**
   * Wrapper around MPI routine; just fails on error and returns the result of
   * MPI_Pack.
   */
  int packed_size(int count, MPI_Datatype type, MPI_Comm comm);

  extern int my_rank, my_size;

#define get_my_size() my_size
#define get_my_rank() my_rank

#define MPI_INTPTR_TYPE \
  ((sizeof (int) == sizeof(intptr_t))            ? MPI_INT : \
   (sizeof (long int) == sizeof (intptr_t))      ? MPI_LONG : /* not MPI_LONG_INT !*/ \
   (sizeof (long long int) == sizeof (intptr_t)) ? MPI_LONG_LONG_INT : \
   MPI_INT)
/*
#define PMPI_Unpack(a,b,c,d,e,f,g)  do { \
  printf ("[%d "__FILE__":%d] outcount=%d\n", my_rank, __LINE__, e);  \
  PMPI_Unpack(a,b,c,d,e,f,g); } while (0)
*/
#ifdef __cplusplus
}
#endif //__cplusplus
#endif // MPI_UTIL_H
