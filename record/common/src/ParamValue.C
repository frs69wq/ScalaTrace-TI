/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include <iostream>      // Added by Henri so that smpicxx can compile this


#include "ParamValue.h"

ParamValue::ParamValue()
{}

ParamValue::ParamValue(int value, int rank):
    ranklist(rank),
    iters(1)
{
  values.push_back(value);
  cnt = iters * ranklist.getNumRanks();
  if(cnt < 0) {
    cerr << "warning: integer overflow" << endl;
    cnt = INT_MAX;
  }
}

ParamValue::ParamValue(int count, int* vector, int rank):
    ranklist(rank),
    iters(1)
{
  for(int i=0; i<count; i++)
    values.push_back(vector[i]);
  cnt = iters * ranklist.getNumRanks();
  if(cnt < 0) {
    cerr << "warning: integer overflow" << endl;
    cnt = INT_MAX;
  }
}

ParamValue::~ParamValue(){};

ParamValue* ParamValue::duplicate(){
  ParamValue *pv = new ParamValue();
  pv->getRanklist()->setRanks(*(ranklist.getRanks()) );
  pv->getRanklist()->setNumRanks(ranklist.getNumRanks());
  pv->setValues(values);
  pv->setIters(iters);
  pv->setCnt(cnt);
  return pv;
}


int ParamValue::compressValues(int *arr, int size){
  int *subarray, len, index;
  bool flag;
  // iterate through all the possible lengths
  for(len=1; len<size/2+1; len++){
    if( size % len == 0 ){
      flag = true;
      // compare the subarrays
      for(subarray=arr+len, index=len; index<size; index+=len, subarray+=len){
        if(memcmp(arr, subarray, len*sizeof(int)) != 0){
          flag = false;
          break;
        }
      }
      if(flag)
        return len;
    }
  }
  return size;
}

void ParamValue::resetTraversalIterators(){
  traverse_iterators.clear();
}

int ParamValue::getScalarValue(){
  if(values.size() != 1){
    cerr << "error: cannot get scalar value from vector" << endl;
    cerr << toString() << endl;
    exit(1);
  }
  return values[0];
}

int ParamValue::getCurrentValue(bool inc, int rank){
  if(rank == -1)
    rank = my_rank;

  map<int, size_t>::iterator it = traverse_iterators.find(rank);
  if(it == traverse_iterators.end()){
    traverse_iterators[rank] = 0;
    it = traverse_iterators.find(rank);
  }
  size_t traverse_iterator = it->second;

  if(values.size() <= traverse_iterator){
    cerr << "cannot find the next value in vector, values.size() = " << values.size() << ", traverse_iterator = " << traverse_iterator << endl;
    exit(1);
  }
  int rtn = values[traverse_iterator];
  if(inc){
    traverse_iterator = (traverse_iterator + 1) % values.size();
    traverse_iterators[rank] = traverse_iterator;
  }
  return rtn;
}

int* ParamValue::getCurrentValues(int count, bool inc, int rank){
  if(rank == -1)
    rank = my_rank;

  map<int, size_t>::iterator it = traverse_iterators.find(rank);
  if(it == traverse_iterators.end()){
    traverse_iterators[rank] = 0;
    it = traverse_iterators.find(rank);
  }
  size_t traverse_iterator = it->second;

  if(values.size() <= traverse_iterator){
    cerr << "cannot find the next value in vector, values.size() = " << values.size() << ", traverse_iterator = " << traverse_iterator << endl;
    exit(1);
  }
  int *rtns = (int *)malloc(count * sizeof(int));
  if(rtns == NULL)
    return NULL;

  int size = values.size();
  for(int i=0; i<count; i++)
    rtns[i] = values[(traverse_iterator + i) % size];
  if(inc){
    traverse_iterator = (traverse_iterator + count) % size;
    traverse_iterators[rank] = traverse_iterator;
  }
  return rtns;

}

bool ParamValue::ranklistEquals(ValueSet* vs){
  if(typeid(*vs) == typeid(ParamValue))
    return ranklist.equals(dynamic_cast<ParamValue *>(vs)->getRanklist() );
  else if (typeid(*vs) == typeid(Histogram))
    return true;
  else
    return false;
}


bool ParamValue::merge(Histogram* h){
  return false;
}

bool ParamValue::merge(ParamValue* pv){
  if(ranklistEquals( pv)){
    /* matching ranklist, different value list, this is a
     * loop compression, compress the value list */
    vector<int>	 *v = pv->getValues();
    if(values == *v){
      iters += pv->getIters();
    } else {
      int size = iters * values.size() + pv->getIters() * v->size();
      int *expanded = (int *)malloc( size * sizeof(int));
      int iter = 0;
      for(int i=0; i<iters; i++){
        for(unsigned int j=0; j<values.size(); j++){
          expanded[iter++] = values[j];
        }
      }
      for(int i=0; i<pv->getIters(); i++){
        for(unsigned int j=0; j<v->size(); j++){
          expanded[iter++] = v->at(j);
        }
      }
      int len = compressValues(expanded, size);
      assert(size % len == 0);
      iters = size / len;
      values.clear();
      values.insert(values.begin(), expanded, expanded+len);
      free(expanded);
    }
    cnt = iters * ranklist.getNumRanks();
    if(cnt < 0) {
      cerr << "warning: integer overflow" << endl;
      cnt = INT_MAX;
    }
    return true;
  } else if (values == *(pv->getValues())) {
    /* matching value list, different ranklist, this is a
     * inter-node compression, compress the ranklists */
    cnt += pv->getCnt();
    if(cnt < 0) {
      cerr << "warning: integer overflow" << endl;
      cnt = INT_MAX;
    }
    ranklist.merge(pv->getRanklist());
    return true;
  } else {
    /* neither the ranklist, nor the value list, is matching, cannot compress*/
    return false;
  }
}

string ParamValue::toString(){
  ostringstream rtn;
  rtn << "( "  ;
  for(unsigned i = 0; i < values.size(); i++)
    rtn << values[i] << " ";
  rtn << ")[ " << ranklist.toString() << " ]";
  return rtn.str();
}

string ParamValue::valuesToString(){
  ostringstream rtn;
  for(unsigned i = 0; i < values.size(); i++)
    rtn << values[i] << " ";
  return rtn.str();
}

void ParamValue::input(string& buf,  Ranklist *rl){
  stringstream ss(stringstream::in | stringstream::out);
  int v;
  if(buf.find("[") == string::npos){
    /* a ranklist must be provided */
    assert(rl);
    ss << buf;
    while( ss >> v )
      values.push_back(v);
    ranklist = *rl;
  } else {
    string vals = buf.substr(0, buf.find(")"));
    vals = vals.substr(vals.find("(")+1);
    ss << vals;
    while( ss >> v )
      values.push_back(v);
    string rl_str = buf.substr(buf.find("[")+1);
    rl_str = rl_str.substr(0, rl_str.find("]") );
    ranklist.input(rl_str);
  }
}


void ParamValue::increase(int inc){
  assert(values.size() == 1); // see comments in Loop::incLoopIter()
  values[0] += 1;
}

int ParamValue::getPackedSize(){
  int size = 0;
  size += sizeof(int); /* type of the current ValueSet */
  size += sizeof(int); /* values.size() */
  size += sizeof(int) * values.size();
  size += ranklist.getPackedSize();
  size += sizeof(int); /* iters */
  size += sizeof(int); /* cnt */
  return size;
}


void ParamValue::pack(void *buf, int bufsize, int *position, MPI_Comm comm){
  int type = 0; /* 0 -- ParamValue, 1 -- Histogram */
  PMPI_Pack(&type, 1, MPI_INT, buf, bufsize, position, comm);
  int size = values.size();
  PMPI_Pack(&size, 1, MPI_INT, buf, bufsize, position, comm);
  for(int i=0; i<size; i++)
    PMPI_Pack(&(values[i]), 1, MPI_INT, buf, bufsize, position, comm);
  ranklist.pack(buf, bufsize, position, comm);
  PMPI_Pack(&iters, 1, MPI_INT, buf, bufsize, position, comm);
  PMPI_Pack(&cnt, 1, MPI_INT, buf, bufsize, position, comm);


}


void ParamValue::unpack(void *buf, int bufsize, int *position, MPI_Comm comm){
  int size, value;
  PMPI_Unpack(buf, bufsize, position, &size, 1, MPI_INT, comm);
  for(int i=0; i<size; i++){
    PMPI_Unpack(buf, bufsize, position, &value, 1, MPI_INT, comm);
    values.push_back(value);
  }
  ranklist.unpack(buf, bufsize, position, comm);
  PMPI_Unpack(buf, bufsize, position, &iters, 1, MPI_INT, comm);
  PMPI_Unpack(buf, bufsize, position, &cnt, 1, MPI_INT, comm);

}





