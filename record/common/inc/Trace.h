/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

/*
 *	A Trace is a doubly linked list of Events
 */

#ifndef __TRACE_H__
#define __TRACE_H__

#ifdef __cplusplus

#include "Event.h"
#include <fstream>
#include <string>
#include <iostream>
#include <mpi.h>
#include <vector>
#include <map>
#include <climits>

#define WINDOW_SIZE 5000
#define MAX_LENGTH_DIFF 0.2

typedef int (event_handler)(Event *e, int depth, int iter);


using namespace std;

class Event;

enum{
	DIRECTION_UP,
	DIRECTION_LEFT,
	DIRECTION_UL
};

typedef struct matching_pair {
	int index1; /* merge */
	int index2; /* target or slave */
} matching_pair_t;

typedef struct lphd{
	Event *event;
	int pos;
} loophead_t;

typedef struct iteration{
	Event *merge_head;
	Event *merge_tail;
	int merge_length;
	Event *target_head;
	Event *target_tail;
	int target_length;
} iteration_t;

typedef multimap<Event*, iteration_t*> iterationmap_t;
typedef multimap<Event*, iteration_t*>::iterator iterationmap_iterator_t;

class PendingIterations{
	private:
		iterationmap_t mh_key;
		iterationmap_t mt_key;
		iterationmap_t th_key;
		iterationmap_t tt_key;

	public:
		PendingIterations();
		~PendingIterations();

		void insert(iteration_t* iteration);
		void create(Event* merge_head, Event* merge_tail, int merge_length,
						Event* target_head, Event* target_tail, int target_length);

		vector<iteration_t *> getIterationsByMergeHead(Event* merge_head);
		vector<iteration_t *> getIterationsByMergeTail(Event* merge_tail);
		vector<iteration_t *> getIterationsByTargetHead(Event* target_head);
		vector<iteration_t *> getIterationsByTargetTail(Event* target_tail);

		void updateIterationMergeHead(iteration_t* iteration, Event* new_merge_head);
		void updateIterationMergeTail(iteration_t* iteration, Event* new_merge_tail);
		void updateIterationTargetHead(iteration_t* iteration, Event* new_target_head);
		void updateIterationTargetTail(iteration_t* iteration, Event* new_target_tail);

		void deleteIteration(iteration_t* iteration);

		void print();

};

class Trace{
	private:
		Event* head;
		Event* tail;

		PendingIterations pendingIterations;

		/* ***********************************************************************
		 * the following functions are used for intra-node compression 
		 * ***********************************************************************/

		/* the following three functions are used for regular loop compression which
		 * requires loop iterations to be exactly matching */
		void fastCompressLoop(int window);
		bool matchLoop(Event* merge_head, Event* merge_tail, int merge_length,
						Event *target_head, Event* target_tail, int target_length);
		void mergeLoopEvents(Event* merge_head, Event* merge_tail, Event *target_head, Event* target_tail);
		void updateLoopInfo(Event* merge_head, Event* merge_tail, Event *target_head, Event *target_tail, int merge_length);
		void deleteTail(Event* merge_tail);

		/* the following functions are used for advanced loop compression which 
		 * performs longest common subsequence analysis for consecutive loop
		 * iterations */
		void slowCompressLoop(int window);
		int realLength(Event* head, Event* tail);
		int realLengthHelper(iteration_t* iteration, int bound);
		void updateIds(Event* from);
		iteration_t* compressLoopLCS(iteration_t* iteration);
		vector<matching_pair_t *>* matchLoopLCS(Event* merge_head, Event* merge_tail, int merge_length,
						Event *target_head, Event* target_tail, int target_length);
		void insertEvent(Event* e, Event *position);
		int updateLoopLCS(Event* merge_head, Event* merge_tail, int merge_length, Event* target_head, Event* target_tail, vector<matching_pair_t *>* pairs);
		void deleteIteration(Event *target_head, Event* target_tail);


		/* ***********************************************************************
		 * the following functions are used for inter-node compression 
		 * ***********************************************************************/
		void get_lcs_helper(int *b, int i, int j, int width, vector<matching_pair_t *> *pairs);
		vector<matching_pair_t *> * getLCS(int *b, int mlen, int slen);
		void LCSDoMerge(vector<matching_pair_t *> *pairs, Trace* slave);
		bool traverse_recursive(Event* loophead, int depth, event_handler handler, int *len);

	public:
		Trace();
		~Trace();

		static void deleteTrace(Trace *trace);

		Event* getHead(){
			return head;
		}
		void setHead(Event *e){
			head = e;
		}
		Event* getTail(){
			return tail;
		}

		int traceLen(){
			int i = 0;
			Event *iter = head;
			while(iter){
				i++;
				iter = iter->next;
			}
			return i;
		}
		void resetTraversalIterators();

		void appendEvent(Event* event, bool compress = true);
		void finalizePendingIterations();
		void cleanupLoops();

		static Trace* inputTrace(const char* filename, int mode, int rank = -1);
		void outputTrace(const char* filename);
		void printTrace();

		int getPackedSize();
		void pack(void *buf, int bufsize, int *postion, MPI_Comm comm);
		void unpack(void *buf, int bufsize, int *postion, MPI_Comm comm);

		void sendTrace(int dest);
		void recvTrace(int src);
		void LCSMergeTrace(Trace *slave);

		bool traverse(event_handler handler);

};


extern "C" {
#endif /* __cplusplus */

typedef struct Trace Trace;

extern Trace trace, left_trace, right_trace;

void appendEvent(Trace *trace, Event *event);

void finalizePendingIterations(Trace *trace);
void cleanupLoops(Trace *trace);

Trace* inputTrace(char *filename, int mode );
void outputTrace(Trace *trace, char *filename);

void sendTrace(Trace *trace, int dest);
void recvTrace(Trace *trace, int src);
void mergeTrace(Trace *master, Trace *slave);

void deleteTrace(Trace *trace);

Event* tailEvent(Trace *trace);
Event* headEvent(Trace *trace);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__TRACE_H__*/
