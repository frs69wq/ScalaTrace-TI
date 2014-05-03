/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */


#ifndef __PARAM_H__
#define __PARAM_H__

#include "ParamValue.h"
#include "paramtype.h"
#include <string>
#include <sstream>
#include <cassert>
#include "Histogram.h"
#include "ValueSet.h"
#include <mpi.h>
#include <cmath>
#include "RandMnger.h"

#define HISTO_THRESHOLD 1

extern int my_rank;

using namespace std;

class Param{
	private:
		int param_type;
		vector<ValueSet *> param_values;

	public:
		Param(int param_type);
		Param();
		~Param();

		Param* duplicate();

		int getParamType(){
			return param_type;
		}
		vector<ValueSet *> *getParamValues(){
			return &param_values;
		}

		bool initWithScalar(int value, int rank);
		bool initWithVector(int count, int* values, int rank);

		int *getParamValueAt(int i, int* count);
		int getScalarValueForRank(int rank = my_rank);
		int getValueForRank(int rank, RandMnger* randmnger = NULL);
		int* getValuesForRank(int count, int rank, RandMnger* randmnger = NULL);
		int getCurrentValue(int rank);
		int* getCurrentValues(int count, int rank);
		vector<int>* getValues(int rank = my_rank);
		int getNumValues(bool expand = false, int rank = my_rank);
		void expandLoop(int factor, int rank = my_rank);
		void loopSetOne(int rank = my_rank);
		void resetTraversalIterators();
		void merge(Param *other);

		string toString();
		string loopParamToString();
		void input(string& buf, int mode, int rank, Ranklist *rl = NULL);

		bool histogramizable(){
			switch(param_type){
				case COUNT:
				case SOURCE:
				case DEST:
#ifdef FEATURE_PARAM_HISTO
					return true;
#endif
				default:
					return false;
			}
		}

		bool triggerHistogramize();

		void histogramize();

		void incValues(int inc);

		int getPackedSize();
		void pack(void *buf, int bufsize, int *position, MPI_Comm comm);
		void unpack(void *buf, int bufsize, int *position, MPI_Comm comm);
};

#endif /*__PARAM_H__*/
