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
