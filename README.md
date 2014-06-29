ScalaTrace-TI
=============

Merging Time Independent Trace Replay and ScalaTrace

Project started by F. Suter and H. Casanova on May 2, 2014 based on
the official distribution of ScalaTrace V2.
All source files have been added a proper comment fixing the 
copyright before any modification of the code.


Here follows the original README from the ScalaTraceV2 distribution
##################################################################
# Introduction
##################################################################

This is a distribution of ScalaTrace 2, the next generation of the 
ScalaTrace tool that collects communication traces for parallel 
applications using the Message Passing Interface. 



##################################################################
# To compile
##################################################################

This ScalaTrace 2 package contains two components: ScalaTrace (the
tracing tool) and ScalaReplay (a replay engine for the ScalaTrace
traces). 

To compile ScalaTrace,

* cd ./record/config
* Edit Makefile.config
* cd ..
* make
* cd ..
* cd ./stackwalk/ver0
* make

Configurable options:

Edit ./record/libsrc/Makefile.libsrc

* RETAIN_TAGS:
n: ignore TAG for MPI_Send, MPI_Recv, etc, for better compression
y: preserve TAG

* PARAM_HISTO:
n: fully lossless tracing
y: record SRC/DEST/COUNT parameters with histograms when lossless 
   compression fails
   More configuration:
   HISTO_THRESHOLD: in ./record/common/inc/Param.h, the maximum
                    vector length for an elastic data element to 
                    remain lossless, e.g., HISTO_THRESHOLD = 30

* SIG_DIFF: 
integer: stack signature difference. This is the maximum number
of different frames allowed in two signatures

* LOOP_LCS:
n: disable approximate loop matching
y: enable approximate loop matching
   More configuration:
   MAX_LENGTH_DIFF: in ./record/common/inc/Trace.h, the maximum
                    percentage difference on length for two loop
                    iterations to be considered approximately 
                    matching, e.g., MAX_LENGTH_DIFF = 0.2 (20%)

To compile ScalaReplay

* cd ./replay
* Edit Makefile
* make 

Configurable options:

Edit ./replay/Makefile

* HISTO_REPLAY
n: deterministic replay, only applicable to fully lossless traces
y: probabilistic replay, applicable to both lossless and lossy 
   traces

* RAND_DEST
integer: maximum number of random destinations for each MPI task,
         e.g., RAND_DEST = 10



##################################################################
# To run
##################################################################

To use ScalaTrace, link ScalaTrace with your application:  

* ST=/path/to/ScalaTrace
* mpicxx -g -L$(ST)/record/lib -L$(ST)/stackwalk/ver0 app.c -o app 
  -lglob -lstackwalk
* mpirun -np <n> ./app
  the trace file will be generated at application completion, in
  the folder trace_<n>
* Compression levels:
  libdump.a: trace with no compression
  libnode.a: trace with intra-node loop compression
  libglob.a: trace with both intra-node and inter-node compression

To use ScalaReplay, use an application trace (T) as the input

* SR=/path/to/ScalaTrace/replay
* mpirun -np <n> $(SR)/replay /path/to/trace/T



##################################################################
# Example
##################################################################

See ./test/1dstencil


