#ifndef __SEED_H__
#define __SEED_H__

#include <vector>
#include <set>
#include <stdlib.h>
#include <climits>
#include "Histogram.h"


using namespace std;

class RandMnger{

private:
	unsigned int seed;
	int limit;
	set<double> seenValues;
	vector<double> valuelist;

public:
	RandMnger();
	RandMnger(unsigned int _seed, int limit = INT_MAX);
	~RandMnger();

	void setSeed(unsigned int _seed){
		seed = _seed;
	}
	unsigned int getSeed(){
		return seed;
	}

	int rand(){
		return rand_r(&seed);
	}

	double getRandomValue(Histogram *h);

};

#endif /* __SEED_H__ */
