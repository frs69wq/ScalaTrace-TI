/**
 * Author: Xing Wu
 */

#ifndef __STAT_H__
#define __STAT_H__

#include <map>
#include <string>
#include <sstream>
#include "Histogram.h"
#include "StackSig.h"
#include "Event.h"

using namespace std;

class Stat{

private:
	int stattype;
	map<StackSig, Histogram *> pathstats;

public:
	Stat(int s);
	Stat();
	virtual ~Stat();

	void resetStat();
	void recordStat(StackSig *predSig);
	void merge(Stat *other);
	void updateSig(StackSig* oldSig, StackSig* newSig);

	map<StackSig, Histogram *>* getPathstats(){
		return &pathstats;
	}
	int getStattype(){
		return stattype;
	}

	double getValue(StackSig* sig);

	virtual void start(){}
	virtual long long end(){return 0;}

	string toString(int phase = -1);
	void input(string& buf);

	int getPackedSize();
	void pack(void *buf, int bufsize, int *position, MPI_Comm comm);
	void unpack(void *buf, int bufsize, int *position, MPI_Comm comm);


};

#endif /*__STAT_H__*/
