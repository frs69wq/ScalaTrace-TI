/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "HistoBin.h"

HistoBin::HistoBin():
start(0),
min(0),
max(0),
end(0),
average(0),
frequency(0)
{}

HistoBin::HistoBin(double _start, double _end):
    start(_start),
    min(DBL_MAX),
    max(-DBL_MAX),
    end(_end),
    average(0),
    frequency(0)
{}

HistoBin::~HistoBin(){}

void HistoBin::add(double value){

  min = (value < min) ? value : min;
  max = (value > max) ? value : max;
  frequency++;

  average += (value - average)/frequency;

  assert(average >= 0.0f);
}

void HistoBin::merge(HistoBin *other){
  assert(end <= other->getStart());
  end = other->getEnd();

  int other_freq = other->getFrequency();
  double other_avg = other->getAverage();

  if(other_freq > 0)
    max = other->getMax();

  if(frequency == 0 && other_freq > 0)
    min = other->getMin();

  if( (frequency + other_freq) > 0)
    average = ( average * frequency + other_avg * other_freq ) / (double)(frequency + other_freq);

  frequency += other_freq;
}

string HistoBin::toString(){
  ostringstream rtn;
  double minimum = min, maximum = max;
  if(minimum == DBL_MAX)
    minimum = 0;
  if(maximum == -DBL_MAX)
    maximum = 0;
  rtn << "{" << setprecision(2) << fixed << start;
  rtn << " " << setprecision(2) << fixed << minimum;
  rtn << " " << setprecision(2) << fixed << average;
  rtn << " " << setprecision(2) << fixed << maximum;
  rtn << " " << setprecision(2) << fixed << end;
  rtn << " " << frequency << "}";
  return rtn.str();
}

void HistoBin::input(string& buf){
  stringstream ss (stringstream::in | stringstream::out);
  ss << buf;
  ss >> start;
  ss >> min;
  ss >> average;
  ss >> max;
  ss >> end;
  ss >> frequency;
  if(frequency == 0 ){
    min = DBL_MAX;
    max = -DBL_MAX;
  }
}

int HistoBin::getPackedSize(){
  return sizeof(double) * 5 + sizeof(int);
}

void HistoBin::pack(void *buf, int bufsize, int *position, MPI_Comm comm){
  PMPI_Pack(&start, 1, MPI_DOUBLE, buf, bufsize, position, comm);
  PMPI_Pack(&min, 1, MPI_DOUBLE, buf, bufsize, position, comm);
  PMPI_Pack(&max, 1, MPI_DOUBLE, buf, bufsize, position, comm);
  PMPI_Pack(&end, 1, MPI_DOUBLE, buf, bufsize, position, comm);
  PMPI_Pack(&average, 1, MPI_DOUBLE, buf, bufsize, position, comm);
  PMPI_Pack(&frequency, 1, MPI_INT, buf, bufsize, position, comm);
}

void HistoBin::unpack(void *buf, int bufsize, int *position, MPI_Comm comm){
  PMPI_Unpack(buf, bufsize, position, &start, 1, MPI_DOUBLE, comm);
  PMPI_Unpack(buf, bufsize, position, &min, 1, MPI_DOUBLE, comm);
  PMPI_Unpack(buf, bufsize, position, &max, 1, MPI_DOUBLE, comm);
  PMPI_Unpack(buf, bufsize, position, &end, 1, MPI_DOUBLE, comm);
  PMPI_Unpack(buf, bufsize, position, &average, 1, MPI_DOUBLE, comm);
  PMPI_Unpack(buf, bufsize, position, &frequency, 1, MPI_INT, comm);
}
