/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef RADIX_TREE_H
#define RADIX_TREE_H

#define LEFT  0
#define RIGHT 1

#ifdef __cplusplus
extern "C" {
#endif

int get_msb(int rank);
int get_child(int rank, int child);
int get_parent(int rank);

#ifdef __cplusplus
}
#endif

#endif
