#include <mpi.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

#define NSTEPS   10
#define BUFSIZE  128
#define BOUND    2
#define METHOD   1

//#define DEBUG

int main(int argc, char *argv[])
{
	int rank, size, step, i, j;
	char rbuf[BOUND * 2][BUFSIZE];
	char sbuf[BOUND * 2][BUFSIZE];
	MPI_Request request_list[BOUND * 4];
	MPI_Status status[BOUND * 4];

	struct timeval begin, end;
    int whole, frac;

	for (i = 0; i < BOUND*4; i++) request_list[i] = MPI_REQUEST_NULL;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


#ifdef DEBUG
	int wait = 1;
	if(argc > 1){
		if(rank == 0) while(wait);
		MPI_Barrier(MPI_COMM_WORLD);
	}
#endif

	MPI_Barrier(MPI_COMM_WORLD);
	gettimeofday(&begin, NULL);
	/* do n steps */
	for(step = 0; step < NSTEPS; step++) {
		//MPI_Barrier(MPI_COMM_WORLD);
		for(j = 0, i = -BOUND; i <= BOUND; i++, j++) {
			if((i != 0) && ((i + rank) > -1) && ((i + rank < size))) {
				MPI_Irecv(rbuf[j], BUFSIZE, MPI_CHAR, (rank + i + size) % size, 0, MPI_COMM_WORLD, &request_list[(BOUND * 2) + j]);
				MPI_Isend(sbuf[j], BUFSIZE, MPI_CHAR, (rank + i + size) % size, 0, MPI_COMM_WORLD, &request_list[j]);
				//MPI_Irecv(rbuf[j], step, MPI_CHAR, (rank + i + size) % size, 0, MPI_COMM_WORLD, &request_list[(BOUND * 2) + j]);
				//MPI_Isend(sbuf[j], step, MPI_CHAR, (rank + i + size) % size, 0, MPI_COMM_WORLD, &request_list[j]);
			}else if(i != 0) {
				request_list[j] = MPI_REQUEST_NULL;
				request_list[(BOUND * 2) + j] = MPI_REQUEST_NULL;
			}else{
				j--;
			}
		}
#if METHOD == 1
		MPI_Waitall(BOUND * 4, request_list, status);
#else
		for(i=0; i<BOUND*4; i++)
			if(request_list[i] != MPI_REQUEST_NULL )
				MPI_Wait(request_list + i, status + i);
#endif
/* Introduce huge computation */
//    for (i = 0; i < 1e9; i++)
//      j = 2 * i - (j / 2);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	gettimeofday(&end, NULL);

	if(rank == 0){
		if(end.tv_usec < begin.tv_usec){
    	    whole = end.tv_sec - begin.tv_sec - 1;
        	frac = end.tv_usec + 1000000 - begin.tv_usec;
        } else {
            whole = end.tv_sec - begin.tv_sec;
            frac = end.tv_usec - begin.tv_usec;
        }
        printf("time: %d.%d (second)\n", whole, frac);
	}

	MPI_Finalize();


	return 0;
}
