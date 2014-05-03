/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

/*
 * This class holds one MPI event with parameters.
 */

#ifndef __EVENT_H__
#define __EVENT_H__

#include "paramtype.h"
#include "limits.h"

#define PHASE_COMP 0
#define PHASE_COMM 1

#ifdef __cplusplus

#include <map>
#include <string>
#include <sstream>

#include "Ranklist.h"
#include "Param.h"
#include "StackSig.h"
#include "Loop.h"
#include <mpi.h>
#include "RandMnger.h"

#define INPUT_GLOBAL 0
#define INPUT_NODE   1

/*
#define MATCH      (0)
#define MFAIL_GEN  (1)
#define MFAIL_ARR  (1<<1)
#define MFAIL_SRC  (1<<2)
#define MFAIL_DST  (1<<3)
#define MFAIL_REQ  (1<<4)
#define MFAIL_CNT  (1<<5)
#define MFAIL_CLR  (1<<6)
#define MFAIL_KEY  (1<<7)
*/

typedef enum {
	SINGLE   = 1,
	MEMBER   = 1<<1,
	LEADER   = 1<<2,
	PENDING_MEMBER  = 1<<3,
	PENDING_LEADER  = 1<<4,
	PENDING_TAIL    = 1<<5
} location_info;

extern int my_rank;

using namespace std;

class Stat;
class StatTime;

class Event{

private:
	int op;
	location_info loc;
	Ranklist ranklist;
	map<int, Param*> params;
	StackSig signature;
	Loop loopStack;

	map<int, Stat*> compStats;
	map<int, Stat*> commStats;

	/* sequence number: used during replay to identify an event */
	int id;

public:
	Event* prev;
	Event* next;
	
	Event(int op, int rank);
	Event();
	~Event();

	Event* getDummyEvent();

	void reset(int phase);
	void record(Event *last, int phase);
	map<int, Stat*> *getCompStats(){
		return &compStats;
	}
	map<int, Stat*> *getCommStats(){
		return &commStats;
	}
	void eventStatsMerge(Event *e);
	void updateStatsSig(StackSig* oldSig, StackSig* newSig);
	double getStatValue(int phase, int stattype, StackSig* prev_sig);

	int getOp(){
		return op;
	}
	void setOp(int operation){
		op = operation;
	}
	bool opMatch(Event *other){
		if(other)
			return op == other->getOp();
		else
			return false;
	}

	int getId(){
		return id;
	}
	void setId(int _id){
		id = _id;
	}

	void eventParamMerge(Event *e);
	void eventRanklistMerge(Event *e);
	map<int, Param*> * getParams(){
		return &params;
	}
	
	bool checkLoc(location_info check);
	void setLoc(location_info set){
		loc = (location_info)( loc | set );
	}
	void unsetLoc(location_info unset){
		loc = (location_info)( loc & (~unset));
	}

	void addScalarValue(int type, int value, int rank);
	void addVectorValue(int type, int count, int *values, int rank);
	void getParamValues(int type, int index, int* count, int** values);
	int getScalarParamValue(int type);
	int getCurrentValue(int type, int rank = -1);
	int* getCurrentValues(int type, int count, int rank = -1);
	int getParamValueForRank(int type, int rank, RandMnger* randmnger = NULL);
	int* getParamValuesForRank(int type, int count, int rank, RandMnger* randmnger = NULL);
	void resetTraversalIterators();

	bool sigMatch(Event *e);
	bool sigEquals(Event *e);
	StackSig* getSignature(){
		return &signature;
	}
	void setSignature(StackSig& s){
		signature = s;
	}

	Loop *getLoopStack(){
		return &loopStack;
	}
	int getLoopDepth(){
		return loopStack.getDepth();
	}
	void expandLoop(int expandFactor, int expandDepth = -1);
	void alignLoop(int targetDepth);
	bool loopMatch(Event *e, int cur_depth, int target_length);
	void insertLoop(int mem, int iter){
		loopStack.insertLoop(mem, iter);
	}

	Ranklist* getRanklist(){
		return &ranklist;
	}
	void setRanklist(Ranklist& rl){
		ranklist = rl;
	}
	bool hasMember(int rank){
		return ranklist.hasMember(rank);
	}

	int getPackedSize();
	void pack(void *buf, int bufsize, int *position, MPI_Comm comm);
	void unpack(void *buf, int bufsize, int *position, MPI_Comm comm);

	string toString(int seq_num = -1);

	static Event* inputEvent(string& buf, int mode, int rank = -1);

};

extern "C" {
#endif /* __cplusplus */

#define OP_ANY_SOURCE (INT_MAX)
#define ENCODE_SOURCE(s) (((s) == MPI_ANY_SOURCE) ?  OP_ANY_SOURCE : ((s) - my_rank + my_size) % my_size)
#define DECODE_SOURCE(s) (((s) ==  OP_ANY_SOURCE) ? MPI_ANY_SOURCE : ((s) + my_rank)%my_size)

#define ENCODE_DEST(d) ((d - my_rank + my_size) % my_size)
#define DECODE_DEST(d) ((d + my_rank) % my_size)

#define ENCODE_KEY(key) ((key - my_rank + my_size) % my_size)
#define DECODE_KEY(key) ((key + my_rank) % my_size)

#define set_rank_offset(op, field)  ((op).rank_offsets |= (field))
#define has_rank_offset(op, field)  ((op).rank_offsets &  (field) ?  1  :  0 )
#define show_rank_offset(op, field)  ((op).rank_offsets &  (field) ? 'o' : 'c')

// send/recv tags may not be relevant for replay purposes
// So we don't record this information for better compression
#ifdef FEATURE_RETAIN_TAGS
#define ENCODE_TAG(a) (a)
#else
#define ENCODE_TAG(a) (0)
#endif


typedef struct Event Event;

extern Event* event;

void createEvent(Event **event, int op, int rank);

void resetStats(Event *event, int phase);
void recordStats(Event *event, Event *last, int phase);

void deleteEvent(Event **event);

void addScalarValue(Event *event, int type, int value, int rank);
void addVectorValue(Event *event, int type, int count, int* values, int rank);

void getParamValues(Event *event, int type, int index, int *count, int** values);
int getScalarParamValue(Event *event, int type);

char* outputEvent(Event *event);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EVENT_H__ */
