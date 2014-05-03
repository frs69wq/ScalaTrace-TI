/* !!!!! This file is automatically generated by a wrapper-engine !!!!! */


#include "umpi_internal.h"

void
umpi_copy_op_MPI_Allgatherv(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.recvcounts != NULL) {
newop->data.mpi.recvcounts =
(int *) malloc (sizeof(int) * op->data.mpi.size);
assert (newop->data.mpi.recvcounts);
bcopy (op->data.mpi.recvcounts,
newop->data.mpi.recvcounts,
sizeof(int) * op->data.mpi.size);
}

if (op->data.mpi.displs != NULL) {
newop->data.mpi.displs =
(int *) malloc (sizeof(int) * op->data.mpi.size);
assert (newop->data.mpi.displs);
bcopy (op->data.mpi.displs,
newop->data.mpi.displs,
sizeof(int) * op->data.mpi.size);
}

return;
} /* umpi_copy_op_MPI_Allgatherv */


void
umpi_copy_op_MPI_Alltoallv(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.counts != NULL) {
newop->data.mpi.counts =
(int *) malloc (sizeof(int) * op->data.mpi.size);
assert (newop->data.mpi.counts);
bcopy (op->data.mpi.counts,
newop->data.mpi.counts,
sizeof(int) * op->data.mpi.size);
}

if (op->data.mpi.sdispls != NULL) {
newop->data.mpi.sdispls =
(int *) malloc (sizeof(int) * op->data.mpi.size);
assert (newop->data.mpi.sdispls);
bcopy (op->data.mpi.sdispls,
newop->data.mpi.sdispls,
sizeof(int) * op->data.mpi.size);
}

if (op->data.mpi.recvcounts != NULL) {
newop->data.mpi.recvcounts =
(int *) malloc (sizeof(int) * op->data.mpi.size);
assert (newop->data.mpi.recvcounts);
bcopy (op->data.mpi.recvcounts,
newop->data.mpi.recvcounts,
sizeof(int) * op->data.mpi.size);
}

if (op->data.mpi.rdispls != NULL) {
newop->data.mpi.rdispls =
(int *) malloc (sizeof(int) * op->data.mpi.size);
assert (newop->data.mpi.rdispls);
bcopy (op->data.mpi.rdispls,
newop->data.mpi.rdispls,
sizeof(int) * op->data.mpi.size);
}

return;
} /* umpi_copy_op_MPI_Alltoallv */


void
umpi_copy_op_MPI_Cart_coords(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_coords != NULL) {
newop->data.mpi.array_of_coords =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_coords);
bcopy (op->data.mpi.array_of_coords,
newop->data.mpi.array_of_coords,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Cart_coords */


void
umpi_copy_op_MPI_Cart_create(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_dims != NULL) {
newop->data.mpi.array_of_dims =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_dims);
bcopy (op->data.mpi.array_of_dims,
newop->data.mpi.array_of_dims,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.array_of_periods != NULL) {
newop->data.mpi.array_of_periods =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_periods);
bcopy (op->data.mpi.array_of_periods,
newop->data.mpi.array_of_periods,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Cart_create */


void
umpi_copy_op_MPI_Cart_get(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_dims != NULL) {
newop->data.mpi.array_of_dims =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_dims);
bcopy (op->data.mpi.array_of_dims,
newop->data.mpi.array_of_dims,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.array_of_periods != NULL) {
newop->data.mpi.array_of_periods =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_periods);
bcopy (op->data.mpi.array_of_periods,
newop->data.mpi.array_of_periods,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.array_of_coords != NULL) {
newop->data.mpi.array_of_coords =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_coords);
bcopy (op->data.mpi.array_of_coords,
newop->data.mpi.array_of_coords,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Cart_get */


void
umpi_copy_op_MPI_Cart_map(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_dims != NULL) {
newop->data.mpi.array_of_dims =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_dims);
bcopy (op->data.mpi.array_of_dims,
newop->data.mpi.array_of_dims,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.array_of_periods != NULL) {
newop->data.mpi.array_of_periods =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_periods);
bcopy (op->data.mpi.array_of_periods,
newop->data.mpi.array_of_periods,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Cart_map */


void
umpi_copy_op_MPI_Cart_rank(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.retcoords != NULL) {
newop->data.mpi.retcoords =
(int *) malloc (sizeof(int) * op->data.mpi.size);
assert (newop->data.mpi.retcoords);
bcopy (op->data.mpi.retcoords,
newop->data.mpi.retcoords,
sizeof(int) * op->data.mpi.size);
}

return;
} /* umpi_copy_op_MPI_Cart_rank */


void
umpi_copy_op_MPI_Cart_sub(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.dimsp != NULL) {
newop->data.mpi.dimsp =
(int *) malloc (sizeof(int) * op->data.mpi.size);
assert (newop->data.mpi.dimsp);
bcopy (op->data.mpi.dimsp,
newop->data.mpi.dimsp,
sizeof(int) * op->data.mpi.size);
}

return;
} /* umpi_copy_op_MPI_Cart_sub */


void
umpi_copy_op_MPI_Dims_create(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_dims != NULL) {
newop->data.mpi.array_of_dims =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_dims);
bcopy (op->data.mpi.array_of_dims,
newop->data.mpi.array_of_dims,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Dims_create */


void
umpi_copy_op_MPI_Gatherv(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.recvcounts != NULL) {
newop->data.mpi.recvcounts =
(int *) malloc (sizeof(int) * op->data.mpi.size);
assert (newop->data.mpi.recvcounts);
bcopy (op->data.mpi.recvcounts,
newop->data.mpi.recvcounts,
sizeof(int) * op->data.mpi.size);
}

if (op->data.mpi.displs != NULL) {
newop->data.mpi.displs =
(int *) malloc (sizeof(int) * op->data.mpi.size);
assert (newop->data.mpi.displs);
bcopy (op->data.mpi.displs,
newop->data.mpi.displs,
sizeof(int) * op->data.mpi.size);
}

return;
} /* umpi_copy_op_MPI_Gatherv */


void
umpi_copy_op_MPI_Graph_create(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_degrees != NULL) {
newop->data.mpi.array_of_degrees =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_degrees);
bcopy (op->data.mpi.array_of_degrees,
newop->data.mpi.array_of_degrees,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.edges != NULL) {
newop->data.mpi.edges =
(int *) malloc (sizeof(int) * op->data.mpi.array_of_degrees[op->data.mpi.count-1]);
assert (newop->data.mpi.edges);
bcopy (op->data.mpi.edges,
newop->data.mpi.edges,
sizeof(int) * op->data.mpi.array_of_degrees[op->data.mpi.count-1]);
}

return;
} /* umpi_copy_op_MPI_Graph_create */


void
umpi_copy_op_MPI_Graph_get(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_degrees != NULL) {
newop->data.mpi.array_of_degrees =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_degrees);
bcopy (op->data.mpi.array_of_degrees,
newop->data.mpi.array_of_degrees,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.retedges != NULL) {
newop->data.mpi.retedges =
(int *) malloc (sizeof(int) * op->data.mpi.edgecount);
assert (newop->data.mpi.retedges);
bcopy (op->data.mpi.retedges,
newop->data.mpi.retedges,
sizeof(int) * op->data.mpi.edgecount);
}

return;
} /* umpi_copy_op_MPI_Graph_get */


void
umpi_copy_op_MPI_Graph_map(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_degrees != NULL) {
newop->data.mpi.array_of_degrees =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_degrees);
bcopy (op->data.mpi.array_of_degrees,
newop->data.mpi.array_of_degrees,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.edges != NULL) {
newop->data.mpi.edges =
(int *) malloc (sizeof(int) * op->data.mpi.array_of_degrees[op->data.mpi.count-1]);
assert (newop->data.mpi.edges);
bcopy (op->data.mpi.edges,
newop->data.mpi.edges,
sizeof(int) * op->data.mpi.array_of_degrees[op->data.mpi.count-1]);
}

return;
} /* umpi_copy_op_MPI_Graph_map */


void
umpi_copy_op_MPI_Graph_neighbors(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_neighbors != NULL) {
newop->data.mpi.array_of_neighbors =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_neighbors);
bcopy (op->data.mpi.array_of_neighbors,
newop->data.mpi.array_of_neighbors,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Graph_neighbors */


void
umpi_copy_op_MPI_Group_excl(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_ranks != NULL) {
newop->data.mpi.array_of_ranks =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_ranks);
bcopy (op->data.mpi.array_of_ranks,
newop->data.mpi.array_of_ranks,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Group_excl */


void
umpi_copy_op_MPI_Group_incl(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_ranks != NULL) {
newop->data.mpi.array_of_ranks =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_ranks);
bcopy (op->data.mpi.array_of_ranks,
newop->data.mpi.array_of_ranks,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Group_incl */


void
umpi_copy_op_MPI_Group_range_excl(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.ranges != NULL) {
newop->data.mpi.ranges =
(int *) malloc (sizeof(int) * 3 * op->data.mpi.count);
assert (newop->data.mpi.ranges);
bcopy (op->data.mpi.ranges,
newop->data.mpi.ranges,
sizeof(int) * 3 * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Group_range_excl */


void
umpi_copy_op_MPI_Group_range_incl(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.ranges != NULL) {
newop->data.mpi.ranges =
(int *) malloc (sizeof(int) * 3 * op->data.mpi.count);
assert (newop->data.mpi.ranges);
bcopy (op->data.mpi.ranges,
newop->data.mpi.ranges,
sizeof(int) * 3 * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Group_range_incl */


void
umpi_copy_op_MPI_Group_translate_ranks(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_ranks != NULL) {
newop->data.mpi.array_of_ranks =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_ranks);
bcopy (op->data.mpi.array_of_ranks,
newop->data.mpi.array_of_ranks,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.array_of_ranks2 != NULL) {
newop->data.mpi.array_of_ranks2 =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_ranks2);
bcopy (op->data.mpi.array_of_ranks2,
newop->data.mpi.array_of_ranks2,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Group_translate_ranks */


void
umpi_copy_op_MPI_Reduce_scatter(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.recvcounts != NULL) {
newop->data.mpi.recvcounts =
(int *) malloc (sizeof(int) * op->data.mpi.size);
assert (newop->data.mpi.recvcounts);
bcopy (op->data.mpi.recvcounts,
newop->data.mpi.recvcounts,
sizeof(int) * op->data.mpi.size);
}

return;
} /* umpi_copy_op_MPI_Reduce_scatter */


void
umpi_copy_op_MPI_Scatterv(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.counts != NULL) {
newop->data.mpi.counts =
(int *) malloc (sizeof(int) * op->data.mpi.size);
assert (newop->data.mpi.counts);
bcopy (op->data.mpi.counts,
newop->data.mpi.counts,
sizeof(int) * op->data.mpi.size);
}

if (op->data.mpi.displs != NULL) {
newop->data.mpi.displs =
(int *) malloc (sizeof(int) * op->data.mpi.size);
assert (newop->data.mpi.displs);
bcopy (op->data.mpi.displs,
newop->data.mpi.displs,
sizeof(int) * op->data.mpi.size);
}

return;
} /* umpi_copy_op_MPI_Scatterv */


void
umpi_copy_op_MPI_Startall(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_requests != NULL) {
newop->data.mpi.array_of_requests =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_requests);
bcopy (op->data.mpi.array_of_requests,
newop->data.mpi.array_of_requests,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Startall */


void
umpi_copy_op_MPI_Testall(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_requests != NULL) {
newop->data.mpi.array_of_requests =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_requests);
bcopy (op->data.mpi.array_of_requests,
newop->data.mpi.array_of_requests,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.statuses_srcs != NULL) {
newop->data.mpi.statuses_srcs =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.statuses_srcs);
bcopy (op->data.mpi.statuses_srcs,
newop->data.mpi.statuses_srcs,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Testall */


void
umpi_copy_op_MPI_Testany(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_requests != NULL) {
newop->data.mpi.array_of_requests =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_requests);
bcopy (op->data.mpi.array_of_requests,
newop->data.mpi.array_of_requests,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Testany */


void
umpi_copy_op_MPI_Testsome(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_requests != NULL) {
newop->data.mpi.array_of_requests =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_requests);
bcopy (op->data.mpi.array_of_requests,
newop->data.mpi.array_of_requests,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.array_of_indices != NULL) {
newop->data.mpi.array_of_indices =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_indices);
bcopy (op->data.mpi.array_of_indices,
newop->data.mpi.array_of_indices,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.statuses_srcs != NULL) {
newop->data.mpi.statuses_srcs =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.statuses_srcs);
bcopy (op->data.mpi.statuses_srcs,
newop->data.mpi.statuses_srcs,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Testsome */


void
umpi_copy_op_MPI_Type_create_darray(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_gsizes != NULL) {
newop->data.mpi.array_of_gsizes =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_gsizes);
bcopy (op->data.mpi.array_of_gsizes,
newop->data.mpi.array_of_gsizes,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.array_of_distribs != NULL) {
newop->data.mpi.array_of_distribs =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_distribs);
bcopy (op->data.mpi.array_of_distribs,
newop->data.mpi.array_of_distribs,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.array_of_dargs != NULL) {
newop->data.mpi.array_of_dargs =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_dargs);
bcopy (op->data.mpi.array_of_dargs,
newop->data.mpi.array_of_dargs,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.array_of_psizes != NULL) {
newop->data.mpi.array_of_psizes =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_psizes);
bcopy (op->data.mpi.array_of_psizes,
newop->data.mpi.array_of_psizes,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Type_create_darray */


void
umpi_copy_op_MPI_Type_create_indexed_block(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_displacements != NULL) {
newop->data.mpi.array_of_displacements =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_displacements);
bcopy (op->data.mpi.array_of_displacements,
newop->data.mpi.array_of_displacements,
sizeof(int) * op->data.mpi.count);
}

return;
} /* umpi_copy_op_MPI_Type_create_indexed_block */


void
umpi_copy_op_MPI_Type_create_subarray(umpi_op_t*op, umpi_op_t*newop)
{
if (op->data.mpi.array_of_sizes != NULL) {
newop->data.mpi.array_of_sizes =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_sizes);
bcopy (op->data.mpi.array_of_sizes,
newop->data.mpi.array_of_sizes,
sizeof(int) * op->data.mpi.count);
}

if (op->data.mpi.array_of_subsizes != NULL) {
newop->data.mpi.array_of_subsizes =
(int *) malloc (sizeof(int) * op->data.mpi.count);
assert (newop->data.mpi.array_of_subsizes);
bcopy (op->data.mpi.array_of_subsizes,
newop->data.mpi.array_of_subsiz