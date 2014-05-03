/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

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
