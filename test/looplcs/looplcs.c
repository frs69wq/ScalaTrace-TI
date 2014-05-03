#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define COUNT 10

#define TESTCASE 2

int main(int argc, char** argv){
	int rank;
	MPI_Comm comm[COUNT];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if(rank != 0){
		MPI_Finalize();
		return 0;
	}

	for(int i=0; i<COUNT; i++)
		MPI_Comm_dup(MPI_COMM_WORLD, &comm[i]);

#if TESTCASE == 1
	// test the compression of the following event stream:
	// A B C D C D E C D E B C D C D E B C D C D B C D B C D E
	for(int i=0; i<1; i++){
		MPI_Barrier(comm[0]); // A
		for(int k=0; k<5; k++){
			MPI_Barrier(comm[1]); // B

			for(int j=0; j<3; j++){
				MPI_Barrier(comm[2]); // C
				MPI_Barrier(comm[3]); // D
				if( (k==0 && j >= 1) || (k==1 && j==1) || k==4)
					MPI_Barrier(comm[4]); // E
				if(k==1 && j==1)
					break;
				if(k==2 && j==1)
					break;
				if(k==3 && j==0)
					break;
				if(k==4 && j==0)
					break;
			}
		}
	}
#endif

#if TESTCASE == 2
	// test the compression of the following event stream:
	// A   B C D C D C D   B C D C D C D C D E   B C   B C D E   A C F D F D F D E
	for(int i=0; i<2; i++){
		MPI_Barrier(comm[0]); // A
		for(int j=0; j<4; j++){
			if(i==0)
				MPI_Barrier(comm[1]); // B
			for(int k=0; k<4; k++){
				MPI_Barrier(comm[2]); // C
				if(j==2) 
					break;
				for(int l=0; l<3; l++){
					if(i==1)
						MPI_Barrier(comm[5]); // F
					MPI_Barrier(comm[3]); // D
					if(i==0)
						break;
				}
				if(i==1)
					break;
				if(j==0 && k==2)
					break;
				if(j==1 && k==3)
					break;
				if(j==3)
					break;
			}
			if((i==0&&j==0) || (i==0&&j==2))
				continue;
			MPI_Barrier(comm[4]); // E
			if(i==1)
				break;
		}
	}
#endif

#if TESTCASE == 3
	// A B C D C D  A B C D C D C D  A B C D C D C D C D
	int k;
	for(int i=0; i<3; i++){
		MPI_Barrier(comm[0]); // A
		MPI_Barrier(comm[1]); // B
		k = i+2	;
		for(int j=0; j<k; j++){
			MPI_Barrier(comm[2]); // C
			MPI_Barrier(comm[3]); // D
		}
	}
#endif

#if TESTCASE == 4
	// A B C D  A B C D E C D E C D E
	for(int i=0; i<2; i++){
		MPI_Barrier(comm[0]); // A
		MPI_Barrier(comm[1]); // B
		for(int j=0; j<3; j++){
			MPI_Barrier(comm[2]); // C
			MPI_Barrier(comm[3]); // D
			if(i == 0) break;
			MPI_Barrier(comm[4]); // E
		}
	}
#endif

#if TESTCASE == 5
	// A B C D E F  A B C D E F  C D E F  D E F D E F
	for(int i=0; i<2; i++){
		MPI_Barrier(comm[0]); // A
		MPI_Barrier(comm[1]); // B
		for(int j=0; j<2; j++){
			MPI_Barrier(comm[2]); // C
			for(int k=0; k<3; k++){
				MPI_Barrier(comm[3]); // D
				MPI_Barrier(comm[4]); // E
				MPI_Barrier(comm[5]); // F
				if(j==0)
					break;
			}
			if(i==0 && j==0)
				break;
		}
	}
#endif

	MPI_Finalize();
	return 0;
}
