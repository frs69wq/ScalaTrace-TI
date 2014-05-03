/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef __VALUESET_H__
#define __VALUESET_H__

#include <string>
#include <mpi.h>

using namespace std;

class Histogram;
class ParamValue;

class ValueSet {
	public:
		ValueSet(){};
		virtual ~ValueSet(){}

		virtual string toString(){return NULL; };
		virtual string valuesToString(){return NULL; };
		virtual bool ranklistEquals(ValueSet* vs){
			return true;
		}

		virtual bool merge(ParamValue * pv){ return false; }
		virtual bool merge(Histogram * h){ return false; }

		virtual int getPackedSize(){return 0; }
		virtual void pack(void *buf, int bufsize, int *position, MPI_Comm comm){}
		virtual void unpack(void *buf, int bufsize, int *position, MPI_Comm comm){}
};

#endif
