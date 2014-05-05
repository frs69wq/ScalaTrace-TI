/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include <iostream>      // Added by Henri so that smpicxx can compile this


#include "Histogram.h"

Histogram::Histogram() :
num_elems(0),
num_bins(NUM_BINS),
max_rank(-1), 
min_rank(-1),
max_value(-DBL_MAX),  
min_value(DBL_MAX),
avg_value(0)
{}


Histogram::~Histogram () {
  for(unsigned i=0; i<bins.size(); i++)
    delete bins[i];
}

int Histogram::find_bin(double value){
  int i = 0;

  for(i=0; i<num_bins; i++)
    if(bins[i]->getStart() <= value && value <= bins[i]->getEnd())
      break;

  return i == num_bins ? -1 : i;
}

void Histogram::smooth(){
  double minDiff = DBL_MAX, temp;
  int merge = -1;
  for(unsigned i = 0; i<bins.size()-1; i++){
    temp = bins[i+1]->getAverage() - bins[i]->getAverage();
    if( temp < minDiff){
      minDiff = temp;
      merge = i;
    }
  }
  assert(0 <= merge && merge < num_bins );
  bins[merge]->merge(bins[merge+1]);
  bins.erase(bins.begin() + merge + 1);
}

void Histogram::add(double value, int rank) {  
  assert(rank>=0);
  if(value < 0){
    if(value >= -DOUBLE_EPSILON){
      cout << "warning: negative double value " << value << " changed to 0.0f" << endl;
      value = 0.0f;
    } else {
      cerr << "error: rank " << my_rank << ", add a negative value " << value << " into histogram" << endl;
      cerr << toString() << endl;
      exit(0);
    }
  }

  if (num_elems == 0) {
    double bin_size = (value != 0.0) ? (2.0 * value)/num_bins : 1.0;

    for (int i=0; i < num_bins; i++) {
      HistoBin *bin = new HistoBin(i*bin_size, (i+1) * bin_size);
      bins.push_back(bin);
    }
  }

  num_elems++;

  // maintain min/max values.
  update_min(value, rank);
  update_max(value, rank);
  update_avg(value, 1);

  // put the value in the histogram
  int binIdx = find_bin(value);
  if(binIdx != -1){
    bins[binIdx]->add(value);
  } else {
    double tail_start = bins[num_bins-1]->getEnd();
    double tail_end = 2 * value - tail_start;
    /*if(tail_end < tail_start){
			cout << "tail_end: " << tail_end << endl;
			cout << "tail_start: " << tail_start << endl;
			cout << "value: " << value << endl;
			cout << toString() << endl;
			while(1);
		}*/
    assert(tail_end >= tail_start);
    HistoBin *tail = new HistoBin(tail_start, tail_end);
    tail->add(value);
    bins.push_back(tail);
    smooth();
  }

}

bool Histogram::merge(ParamValue *pv){
  if(pv == NULL)
    return false;

  vector<int> *vs = pv->getValues();
  /*int iter = pv->getIters();
	int numRanks = pv->getRanklist()->getNumRanks();
	iter *= numRanks;*/
  int iter = pv->getCnt();
  int rank = pv->getRanklist()->getFirstRank();
  for(unsigned i=0; i<vs->size(); i++)
    for(int j=0; j<iter; j++)
      add((double) vs->at(i), rank);
  return true;
}

bool Histogram::merge(Histogram *other){
  int other_max_rank = other->getMaxRank();
  int other_min_rank = other->getMinRank();
  vector<HistoBin *> *other_bins = other->getBins();
  assert(other_bins->size() == (size_t)num_bins);
  int minBin = -1, maxBin = -2;
  for(int i=0; i<num_bins; i++)
    if(other_bins->at(i)->getFrequency() > 0){
      minBin = i;
      break;
    }
  for(int i=num_bins-1; i>=0; i--)
    if(other_bins->at(i)->getFrequency() > 0){
      maxBin = i;
      break;
    }
  assert(maxBin >= minBin);

  int rank = -1;
  int freq;
  double mean;
  HistoBin *bin;
  for(int i=minBin; i<=maxBin; i++){
    bin = other_bins->at(i);
    freq = bin->getFrequency();
    if( freq > 0 && i == minBin){
      rank = other_min_rank;
      add(bin->getMin(), rank);
      freq--;
      if(freq > 0){ // at least 2 elements
        // if freq == 2, then adding min and max preserves the mean
        add(bin->getMax(), rank);
        freq--;
        if(freq > 0){
          // if freq > 2, then add values to preserve the mean
          mean = bin->getAverage() * (freq + 2);
          mean -= bin->getMin();
          mean -= bin->getMax();
          mean = mean / freq;
        }
      }
    }
    else if( freq > 0 && i == maxBin ){
      rank = other_max_rank;
      add(bin->getMax(), rank);
      freq--;
      if(freq > 0){
        add(bin->getMin(), rank);
        freq--;
        if(freq > 0){
          mean = bin->getAverage() * (freq + 2);
          mean -= bin->getMin();
          mean -= bin->getMax();
          mean = mean / freq;
        }
      }
    }
    else if( freq > 0 && i != maxBin && i != minBin){
      rank = other_min_rank;
      add(bin->getMin(), rank);
      freq--;
      if(freq > 0){
        add(bin->getMax(), rank);
        freq--;
        if(freq > 0){
          mean = bin->getAverage() * (freq + 2);
          mean -= bin->getMin();
          mean -= bin->getMax();
          mean = mean / freq;
        }

      }
    }
    for(int j=0; j<freq; j++){
      add(mean, rank);
    }
  }

  return true;
}

double Histogram::randomValue(int rand){
  if(num_elems == 0){
    cerr << "Error: histogram of no elements" << endl;
    cerr << toString() << endl;
    return -1.0f;
  }

  int r = rand % num_elems;
  r += 1;

  int freq;
  for(int i=0; i<num_bins; i++){
    freq = bins[i]->getFrequency();
    if(r <= freq){
      // return a value in this bin
      assert(freq > 0 && r > 0);
      if(freq == 1)
        return bins[i]->getAverage();
      return bins[i]->getMin() + (bins[i]->getMax() - bins[i]->getMin()) / (double)(freq-1) * (r - 1);
    }
    r -= freq;
  }
  /* This is unreachable! Should always return a value in the for loop */
  assert(0);
  return -1.0f;
}

string Histogram::toString(){
  ostringstream rtn;
  rtn << "(" << num_elems << " ";
  rtn << setprecision(2) << fixed << avg_value << " ";
  rtn << min_rank << " ";
  rtn << setprecision(2) << fixed << min_value << " ";
  rtn << max_rank << " ";
  rtn << setprecision(2) << fixed << max_value << ")";
  for(unsigned i=0; i<bins.size(); i++)
    rtn << bins[i]->toString();
  return rtn.str();
}

string Histogram::valuesToString(){
  return toString();
}

void Histogram::input(string& buf){
  stringstream ss (stringstream::in | stringstream::out);
  string histo = buf.substr(0, buf.find(")"));
  histo = histo.substr(buf.find("(")+1);
  ss << histo;
  ss >> num_elems;
  ss >> avg_value;
  ss >> min_rank;
  ss >> min_value;
  ss >> max_rank;
  ss >> max_value;
  size_t pos = buf.find("{");
  int num_bins = 0;
  string bin_str;
  HistoBin *bin;
  while(pos != string::npos){
    num_bins++;
    bin_str = buf.substr(pos+1, buf.find("}", pos+1) - (pos+1));
    bin = new HistoBin();
    bin->input(bin_str);
    bins.push_back(bin);
    pos = buf.find("{", pos+1);
  }
}

int Histogram::getPackedSize(){
  int size = 0;
  size += sizeof(int); /* type of the current ValueSet object */
  size += sizeof(int) * 4 + sizeof(double) * 3; /* scalar members */
  for(unsigned i=0; i<bins.size(); i++)
    size += bins[i]->getPackedSize();
  return size;
}

void Histogram::pack(void *buf, int bufsize, int *position, MPI_Comm comm){
  int type = 1; /* 0 -- ParamValue, 1 -- Histogram */
  PMPI_Pack(&type, 1, MPI_INT, buf, bufsize, position, comm);
  PMPI_Pack(&num_elems, 1, MPI_INT, buf, bufsize, position, comm);
  PMPI_Pack(&num_bins, 1, MPI_INT, buf, bufsize, position, comm);
  PMPI_Pack(&max_rank, 1, MPI_INT, buf, bufsize, position, comm);
  PMPI_Pack(&min_rank, 1, MPI_INT, buf, bufsize, position, comm);
  PMPI_Pack(&max_value, 1, MPI_DOUBLE, buf, bufsize, position, comm);
  PMPI_Pack(&min_value, 1, MPI_DOUBLE, buf, bufsize, position, comm);
  PMPI_Pack(&avg_value, 1, MPI_DOUBLE, buf, bufsize, position, comm);
  for(unsigned i=0; i<bins.size(); i++)
    bins[i]->pack(buf, bufsize, position, comm);
}

void Histogram::unpack(void *buf, int bufsize, int *position, MPI_Comm comm){
  PMPI_Unpack(buf, bufsize, position, &num_elems, 1, MPI_INT, comm);
  PMPI_Unpack(buf, bufsize, position, &num_bins, 1, MPI_INT, comm);
  PMPI_Unpack(buf, bufsize, position, &max_rank, 1, MPI_INT, comm);
  PMPI_Unpack(buf, bufsize, position, &min_rank, 1, MPI_INT, comm);
  PMPI_Unpack(buf, bufsize, position, &max_value, 1, MPI_DOUBLE, comm);
  PMPI_Unpack(buf, bufsize, position, &min_value, 1, MPI_DOUBLE, comm);
  PMPI_Unpack(buf, bufsize, position, &avg_value, 1, MPI_DOUBLE, comm);
  for(int i=0; i<num_bins; i++){
    HistoBin *bin = new HistoBin();
    bin->unpack(buf, bufsize, position, comm);
    bins.push_back(bin);
  }
}
