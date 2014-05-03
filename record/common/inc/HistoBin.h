/**
 * Author: Xing Wu
 */

#ifndef __HISTOBIN_H__
#define __HISTOBIN_H__


#include <string>
#include <sstream>
#include <cassert>
#include <float.h>
#include <iomanip>
#include <mpi.h>

using namespace std;

class HistoBin {

private:

	double start;        /* bin start */
	double min;          /* bin start */
	double max;          /* bin end */
	double end;          /* bin end */
	double average;      /* Average */
	int    frequency;    /* #items */

public:
	HistoBin();
	HistoBin(double _start, double _end);
	~HistoBin();

	/* Adds a value to the bin and updates values. */
	void add(double value);

	void merge(HistoBin *other);

	/*getters and setters*/
	double getStart(){
		return start;
	}
	void setStart(double _start){
		start = _start;
	}

	double getMin(){
		return min;
	}
	void setMin(double _min){
		min = _min;
	}

	double getMax(){
		return max;
	}
	void setMax(double _max){
		max = _max;
	}

	double getEnd(){
		return end;
	}
	void setEnd(double _end){
		end = _end;
	}

	double getAverage(){
		return average;
	}
	void setAverage(double _average){
		average = _average;
	}

	int getFrequency(){
		return frequency;
	}
	void setFrequency(int _frequency){
		frequency = _frequency;
	}
	void incFrequency(){
		frequency++;
	}


	string toString();
	void input(string& buf);

	int getPackedSize();
	void pack(void *buf, int bufsize, int *position, MPI_Comm comm);
	void unpack(void *buf, int bufsize, int *position, MPI_Comm comm);

};


#endif /*__HISTOBIN_H__*/
