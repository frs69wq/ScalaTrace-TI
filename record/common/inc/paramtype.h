/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef __PARAMTYPE_H__
#define __PARAMTYPE_H__



#define RECVCOUNTS                 1
#define DISPLS                     2
#define COUNTS                     3
#define RDISPLS                    4
#define ARRAY_OF_DIMS              5
#define ARRAY_OF_PERIODS           6
#define DIMSP                      7
#define ARRAY_OF_DEGREES           8
#define ARRAY_OF_EDGES             9
#define ARRAY_OF_RANKS             10
#define RANGES                     11
#define ARRAY_OF_REQUESTS          12
#define ARRAY_OF_BLOCKLENS         13
#define ARRAY_OF_DISPLACEMENTS     14
#define ARRAY_OF_TYPES             15

/* list of op types for ascii parsing */
#define COMM                       16
#define COUNT                      18
#define DATATYPE                   20
#define RECVCOUNT                  21
#define RECVTYPE                   22
#define MPI_OP                     24
#define RT                       25
#define DEST                       26
#define TAG                        27
#define REQUEST                    28
#define REORDER                    29
//#define COMM_OUT                   1ll << 30
#define NEWRANK                    32
#define GROUP                      33
//#define GROUP_OUT                  1ll << 34
#define COLOR                      35
#define KEY                        36
#define GROUP1                     37
#define GROUP2                     38
#define LOCAL_LEADER               39
#define PEER_COMM                  40
#define REMOTE_LEADER              41
#define HIGH                       42
#define SOURCE                     43
#define SENDTAG                    44
#define RECVTAG                    45
#define NEWTYPE                    46
#define OLDTYPE                    48
#define BLOCKLEN                   49
#define STRIDE                     50
#define GLOBAL_WORLD               51
#define NULL_REQ                   52
#define OUTCOUNT                   53

/* list of op types for i/o tracing */

#define ARRAY_OF_GSIZES           54 
#define ARRAY_OF_DISTRIBS         55 
#define ARRAY_OF_DARGS            56 
#define ARRAY_OF_PSIZES           57 

#define FILENAME                  58 
#define AMODE                     59 
#define FH                        60
#define OFFSET                    61 
#define DISP                      62
#define DATAREP                   63 
#define ETYPE                     64 
#define FILETYPE                  65 
#define WHENCE                    66 
#define COMM_SIZE                 67 
#define RANK                      68 
#define NDIMS                     69 
#define ORDER                     70
#define POSIX_MODE                71 
#define POSIX_FLAGS               72 


/* loop iteration */
#define LOOPITER                  73
#define LOOPMEM                   74


/* Statistics*/
#define STAT_TIME                 75
#define STAT_INST                 76

#endif /*__PARAMTYPE_H__*/
