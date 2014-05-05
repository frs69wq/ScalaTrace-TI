/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "radix_tree.h"

int get_msb(int rank)
{
  int msb = (sizeof(int) * 8) - 1;
  while(!(rank >> msb) && msb > -1)
    msb--;

  return msb;
}

int get_child(int rank, int child)
{
  int msb, mask, i;

  rank++;
  msb = get_msb(rank);
  mask = 0;

  for(i = 0; i < msb; i++)
    mask |= (1 << i);

  return ((1 << (msb + 1)) | (child << msb) | (rank & mask)) - 1;
}

int get_parent(int rank)
{
  int msb, mask, i;

  rank++;
  msb = get_msb(rank);
  mask = 0;

  for(i = 0; i < msb - 1; i++)
    mask |= (1 << i);

  return ((1 << (msb - 1)) | (rank & mask)) - 1;
}
