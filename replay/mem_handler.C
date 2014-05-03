/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */
#include <stdlib.h>
#include "mem_handler.h"

void **mem_array;
int mem_size;
int mem_cur;

void init_mem(int size) {
  int i;
  mem_array = (void **) malloc(sizeof(void *) * size);
  for(i = 0; i < size; i++)
    mem_array[i] = NULL;
  mem_size = size;
  mem_cur = 0;
}

void add_mem_entry(void *mem_addr){
  if(mem_array[mem_cur]!=NULL){
    free(mem_array[mem_cur]);
  }
  mem_array[mem_cur] = mem_addr;
  mem_cur++;
  if(mem_cur >= mem_size)
    mem_cur = 0;
}

void free_mem_entry(int index){
  if(index != -1 && mem_array[(mem_cur + index) % mem_size]){
    free(mem_array[(mem_cur + index) % mem_size]);
    mem_array[(mem_cur + index) % mem_size] = NULL;
  }
}

void free_mem_entrylist(int *indexes, int count, int null_request){
  int i;
  for(i = 0; i < count; i++) {
    if(indexes[i] != null_request) {
      free(mem_array[(mem_cur + indexes[i]) % mem_size]);
      mem_array[(mem_cur + indexes[i]) % mem_size] = NULL;
    }
  }
}

void finalize_mem(){
  int i;
  for(i = 0; i < mem_size; i++){
    if(mem_array[i])
      free(mem_array[i]);
  }
  free(mem_array);
}
