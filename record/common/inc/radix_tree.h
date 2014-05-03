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
