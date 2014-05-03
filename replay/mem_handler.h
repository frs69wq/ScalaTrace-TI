/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */
#ifndef MEM_HANDLER_H
#define MEM_HANDLER_H

void init_mem(int size);
void add_mem_entry(void *mem_addr);
void free_mem_entry(int index);
void free_mem_entrylist(int *indexes, int count, int null_request);
void finalize_mem();

#endif
