#ifndef MEM_HANDLER_H
#define MEM_HANDLER_H

void init_mem(int size);
void add_mem_entry(void *mem_addr);
void free_mem_entry(int index);
void free_mem_entrylist(int *indexes, int count, int null_request);
void finalize_mem();

#endif
