#include "RandMnger.h"

RandMnger::RandMnger(){}

RandMnger::RandMnger(unsigned int _seed, int _limit):
seed(_seed),
limit(_limit)
{}

RandMnger::~RandMnger(){}

double RandMnger::getRandomValue(Histogram *h){
	if(!h){
		cerr << "error: null histogram" << endl;
		exit(0);
	}

	int r = rand();
	set<double>::iterator it;

	if(seenValues.size() >= (unsigned int)limit){
		return valuelist[ r%limit ];
	}

	double rtn = h->randomValue(r);
	it = seenValues.find(rtn);
	if(it == seenValues.end()){
		seenValues.insert(rtn);
		valuelist.push_back(rtn);
	}

	return rtn;
}

