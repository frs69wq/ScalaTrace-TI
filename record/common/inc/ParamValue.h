/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef __PARAMVALUE_H__
#define __PARAMVALUE_H__

#include <typeinfo>
#include <vector>
#include <string>
#include <map>
#include "Ranklist.h"
#include <sstream>
#include <cassert>
#include "ValueSet.h"
#include "Histogram.h"
#include <mpi.h>

using namespace std;

extern int my_rank;

class ParamValue : public ValueSet{
	private:
		vector<int> values;
		Ranklist ranklist;
		int iters;
		int cnt; /* for each single value in the vector of values, how many times
						it has been used. It equals to the sum of the usage counts of
						all the ranks in the associated ranklist. This member will be
						used for 'histogramization' */
		map<int, size_t> traverse_iterators; /* only used for traversal (replay) */

		int compressValues(int *arr, int size);


	public:
		ParamValue();
		ParamValue(int value, int rank = 0);
		ParamValue(int count, int* vector, int rank);
		~ParamValue();

		ParamValue* duplicate();

		vector<int>* getValues(){
			return &values;
		}
		void setValues(vector<int>& _values){
			values = _values;
		}

		Ranklist* getRanklist(){
			return &ranklist;
		}
		int getIters(){
			return iters;
		}
		void setIters(int _iters){
			iters = _iters;
		}
		int getCnt(){
			return cnt;
		}
		void setCnt(int _cnt){
			cnt = _cnt;
		}
		int getNumValues(bool expand = false){
			if(expand)
				return values.size() * iters;
			else
				return values.size();
		}
		bool hasMember(int rank){
			return ranklist.hasMember(rank);
		}
		void expand(int factor){
			iters *= factor;
			cnt *= factor;
		}
		void setOne(){
			for(vector<int>::iterator it = values.begin(); it != values.end(); it++)
				(*it) = 1;
		}

		void resetTraversalIterators();
		int getScalarValue();
		int getCurrentValue(bool inc = false, int rank = -1);
		int* getCurrentValues(int count, bool inc = false, int rank = -1);
		bool ranklistEquals(ValueSet* vs);

		bool merge(ParamValue* pv);
		bool merge(Histogram* h);

		string toString();
		string valuesToString();
		void input(string& buf, Ranklist* rl = NULL);

		void increase(int inc);

		int getPackedSize();
		void pack(void *buf, int bufsize, int *position, MPI_Comm comm);
		void unpack(void *buf, int bufsize, int *position, MPI_Comm comm);
};

#endif /*__PARAMVALUE_H__*/
