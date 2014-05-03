/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "mpi_util.h"

// definition for apps that use MPI
#include <mpi.h>
#include <stdio.h>
#include <assert.h>

signed char log2pow2(unsigned long long powerOf2) {
  // make sure it's a power of 2.
  assert(isPowerOf2(powerOf2));
  signed char n = -1;
  while (powerOf2 > 0) {
    powerOf2 >>= 1;
    n++;
  }
  return n;
}


int packed_size(int count, MPI_Datatype type, MPI_Comm comm) {
  int size;
  int err = PMPI_Pack_size(count, type, comm, &size);
  if (err) fprintf(stderr, "Error getting size!");
  return size;
}

int my_rank, my_size;
