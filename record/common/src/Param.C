/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include <iostream>      // Added by Henri so that smpicxx can compile this

#include "Param.h"
#include "Event.h"

Param::Param(int param_type):
param_type(param_type)
{}

Param::Param(){}

Param::~Param(){
  vector<ValueSet *>::iterator it;
  for(it = param_values.begin(); it != param_values.end(); it++){
    if(*it != NULL)
      delete *it;
  }
}

Param* Param::duplicate(){
  if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
    cerr << "error: Param::duplicateParam() should not be called against a histogram" << endl;
    cerr << toString() << endl;
    exit(0);
  } else {
    Param *p = new Param(param_type);
    for(size_t i=0; i<param_values.size(); i++)
      p->getParamValues()->push_back(dynamic_cast<ParamValue *>(param_values[i])->duplicate());
    return p;
  }
  return NULL;
}


bool Param::initWithScalar(int value, int rank){
  if(param_values.size() > 0)
    return false; /* has already been initialized */

  ParamValue *scalar_value = new ParamValue(value, rank);
  param_values.push_back(scalar_value);

  return true;
}

bool Param::initWithVector(int count, int *values, int rank){
  if(param_values.size() > 0)
    return false; /* has already been initialized */

  ParamValue *vector_value = new ParamValue(count, values, rank);
  param_values.push_back(vector_value);

  return true;
}

int* Param::getParamValueAt(int i, int* count){
  if( i<0 || i>= (int)param_values.size() ) {
    if(count)
      *count = 0;
    return NULL;
  } else if (typeid(*(param_values[i])) == typeid(Histogram) ) {
    if(count)
      *count = 0;
    return NULL;
  } else {
    ParamValue * pv = dynamic_cast<ParamValue *>(param_values[i]);
    assert(pv);
    vector<int> *values = pv->getValues();
    if(!values || values->size() == 0){
      if(count)
        *count = 0;
      return NULL;
    } else {
      if(count)
        *count = values->size();
      int* rtn = (int*)malloc(values->size() * sizeof(int));
      for(unsigned k=0; k<values->size(); k++)
        rtn[k] = values->at(k);
      return rtn;
    }
  }
}

int Param::getScalarValueForRank(int rank){
  if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
    cerr << "error: Param::getScalarValueForRank() should not be called against a histogram" << endl;
    cerr << toString() << endl;
    exit(0);
  } else {
    for(size_t i=0; i<param_values.size(); i++){
      if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank))
        return dynamic_cast<ParamValue *>(param_values[i])->getScalarValue();
    }
  }
  return -1;
}


int Param::getValueForRank(int rank, RandMnger *randmnger){
  if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
    if(!randmnger) {
      return (int)dynamic_cast<Histogram *>(param_values[0])->avg();
    } else {
      double rtn = randmnger->getRandomValue(dynamic_cast<Histogram *>(param_values[0]));
      return (int)round(rtn);
    }
  } else {
    for(size_t i=0; i<param_values.size(); i++){
      if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank))
        return dynamic_cast<ParamValue *>(param_values[i])->getCurrentValue(true, rank);
    }
  }
  return 0;
}

int* Param::getValuesForRank(int count, int rank, RandMnger* randmnger){
  if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
    cerr << "error: Param::getValuesForRank() should not be called against a histogram" << endl;
    cerr << toString() << endl;
    exit(0);
  } else {
    for(size_t i=0; i<param_values.size(); i++){
      if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank))
        return dynamic_cast<ParamValue *>(param_values[i])->getCurrentValues(count, true, rank);
    }
  }
  return NULL;
}

/**
 * For efficiency reasons, 
 * this function assumes there is only 1 element
 * int param_values and it is of the type ParamValue.
 * Besides, it returns the "current value" without
 * check if the rank of the calling MPI process is
 * in the ranklist of the ParamValue
 */
int Param::getCurrentValue(int rank){
  assert(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(ParamValue));
  return dynamic_cast<ParamValue *>(param_values[0])->getCurrentValue(true, rank);
}
/**
 * For efficiency reasons, 
 * this function assumes there is only 1 element
 * int param_values and it is of the type ParamValue.
 * Besides, it returns the "current values" without
 * check if the rank of the calling MPI process is
 * in the ranklist of the ParamValue
 */
int* Param::getCurrentValues(int count, int rank){
  assert(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(ParamValue));
  return dynamic_cast<ParamValue *>(param_values[0])->getCurrentValues(count, true,  rank);
}

vector<int>* Param::getValues(int rank){
  if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
    cerr << "error: Param::getValues() should not be called against a histogram" << endl;
    cerr << toString() << endl;
    exit(0);
  } else {
    for(size_t i=0; i<param_values.size(); i++){
      if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank))
        return dynamic_cast<ParamValue *>(param_values[i])->getValues();
    }
  }
  return NULL;
}

int Param::getNumValues(bool expand, int rank){
  if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
    cerr << "error: Param::getNumValues() should not be called against a histogram" << endl;
    cerr << toString() << endl;
    exit(0);
  } else {
    for(size_t i=0; i<param_values.size(); i++){
      if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank))
        return dynamic_cast<ParamValue *>(param_values[i])->getNumValues(expand);
    }
  }
  return -1;
}

void Param::expandLoop(int factor, int rank){
  if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
    cerr << "error: Param::expandLoop() should not be called against a histogram" << endl;
    cerr << toString() << endl;
    exit(0);
  } else {
    for(size_t i=0; i<param_values.size(); i++){
      if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank))
        dynamic_cast<ParamValue *>(param_values[i])->expand(factor);
    }
  }
}

void Param::loopSetOne(int rank){
  if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
    cerr << "error: Param::loopSetOne() should not be called against a histogram" << endl;
    cerr << toString() << endl;
    exit(0);
  } else {
    for(size_t i=0; i<param_values.size(); i++){
      if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank))
        dynamic_cast<ParamValue *>(param_values[i])->setOne();
    }
  }
}

bool Param::triggerHistogramize(){
  if(!histogramizable())
    return false;

  if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) )
    return false;

  int total = 0;
  for(unsigned i=0; i<param_values.size(); i++){
    assert(typeid(*(param_values[i])) == typeid(ParamValue));
    total += dynamic_cast<ParamValue *>(param_values[i])->getNumValues();
  }
  if(total >= HISTO_THRESHOLD)
    return true;
  else
    return false;
}

void Param::histogramize(){
  Histogram *h = new Histogram();
  ParamValue *pv;

  for(unsigned i=0; i<param_values.size(); i++){
    assert(typeid(*(param_values[i])) == typeid(ParamValue));
    pv = dynamic_cast<ParamValue *>(param_values[i]);
    vector<int>* values = pv->getValues();
    /*int numRanks = pv->getRanklist()->getNumRanks();
		int iter = pv->getIters();
		iter *= numRanks;*/
    int iter = pv->getCnt();
    int rank = pv->getRanklist()->getFirstRank();

    for(unsigned j=0; j<values->size(); j++)
      for(int k=0; k<iter; k++)
        h->add((double)(values->at(j)), rank);
  }

  for(unsigned i=0; i<param_values.size(); i++)
    delete param_values[i];
  param_values.clear();
  param_values.push_back(h);

}

void Param::resetTraversalIterators(){
  for(unsigned i=0; i<param_values.size(); i++)
    if(typeid(*(param_values[i])) == typeid(ParamValue))
      dynamic_cast<ParamValue *>(param_values[i])->resetTraversalIterators();
}

void Param::merge(Param *other){
  if(param_type != other->getParamType())
    return;

  vector<ValueSet *> *values = other->getParamValues();

  if(param_values.size() == 1 && values->size() == 1
      && param_values[0]->ranklistEquals(values->at(0))){
    /* loop compression */
    if(typeid(*(param_values[0])) == typeid(ParamValue)
        && typeid(*(values->at(0))) == typeid(ParamValue) ){
      /* rlparam into rlparam*/
      param_values[0]->merge(dynamic_cast<ParamValue *>(values->at(0)) );
      if(triggerHistogramize()){
        histogramize();
      }
    } else if (typeid(*(param_values[0])) == typeid(ParamValue)
        && typeid(*(values->at(0))) == typeid(Histogram) ){
      /* histogram into rlparam*/
      /* 1. histogramize the rlparam */
      assert(histogramizable() );
      histogramize();
      /* 2. histogram into histogram */
      param_values[0]->merge(dynamic_cast<Histogram *>(values->at(0)) );
    } else if (typeid(*(param_values[0])) == typeid(Histogram)
        && typeid(*(values->at(0))) == typeid(ParamValue) ){
      /* rlparam into histogram */
      param_values[0]->merge(dynamic_cast<ParamValue *>(values->at(0)) );
    } else {
      /* histogram into histogram */
      param_values[0]->merge(dynamic_cast<Histogram *>(values->at(0)) );
    }

  } else {
    /* inter-node compression */
    if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
      /* rlparam or histogram into histogram*/
      for(unsigned i = 0; i<values->size(); i++){
        if(typeid(*(values->at(i))) == typeid(ParamValue) )
          param_values[0]->merge(dynamic_cast<ParamValue *>(values->at(i)) );
        else
          param_values[0]->merge(dynamic_cast<Histogram *>(values->at(i)) );
      }
    } else {
      if(typeid(*(values->at(0))) == typeid(Histogram)){
        /* histogram into rlparam */
        /* 1. histogramize the rlparam */
        assert(histogramizable() );
        histogramize();
        /* 2. histogram into histogram */
        param_values[0]->merge(dynamic_cast<Histogram *>(values->at(0)) );
      } else {
        int flag;
        /* multiple rlparams into multiple rlparams */
        for(unsigned i=0; i<values->size(); i++){
          assert(typeid(*(values->at(i))) == typeid(ParamValue) );
          flag = false;
          for(unsigned j=0; j<param_values.size(); j++){
            assert(typeid(*(param_values[j])) == typeid(ParamValue) );
            if(param_values[j]->merge(dynamic_cast<ParamValue *>(values->at(i))) ){
              flag = true;
              break;
            }
          }
          if(!flag){
            param_values.push_back(dynamic_cast<ParamValue *>(values->at(i)) );
            values->at(i) = NULL;
          }
        }
        if(triggerHistogramize()){
          histogramize();
        }
      }
    }
  }
}


void Param::incValues(int inc){
  assert(param_values.size() == 1); // this function is called only during intra-node compression
  assert(typeid(*(param_values[0])) == typeid(ParamValue)); // see comments in Loop::incLoopIter()
  dynamic_cast<ParamValue *>(param_values[0])->increase(inc);
}


void Param::input(string& buf, int mode, int rank, Ranklist *rl){
  if(rl && !rl->hasMember(rank) && mode == INPUT_NODE)
    return;

  if(buf.find("{") != string::npos){
    /* input a histogram */
    Histogram *h = new Histogram();
    if(!h)
      exit(1);
    h->input(buf);
    param_values.push_back(h);
  } else {
    /* input a rlparam */
    ParamValue *pv = new ParamValue();
    if(!pv)
      exit(1);
    pv->input(buf, rl);
    if( mode == INPUT_GLOBAL || pv->getRanklist()->hasMember(rank) ){
      param_values.push_back(pv);
    } else {
      delete pv;
    }
  }
}

string Param::toString(){
  ostringstream rtn;

  if(param_values.size() == 1){
    rtn << hex << param_type << " ";
    rtn << param_values[0]->valuesToString() << "\n";
  } else {
    for(unsigned i=0; i<param_values.size(); i++){
      rtn << hex << param_type << " ";
      rtn << param_values[i]->toString() << "\n";
    }
  }

  return rtn.str();

}

string Param::loopParamToString(){
  ostringstream rtn;
  for(unsigned i=0; i<param_values.size(); i++){
    rtn << param_values[i]->toString();
  }

  return rtn.str();

}

int Param::getPackedSize(){
  int size = 0;
  size += sizeof(int);
  size += sizeof(int); /* param_values.size() */
  for(unsigned i = 0; i<param_values.size(); i++){
    size += param_values[i]->getPackedSize();
  }
  return size;
}


void Param::pack(void *buf, int bufsize, int *position, MPI_Comm comm){
  PMPI_Pack(&param_type, 1, MPI_INT, buf, bufsize, position, comm);
  int size = param_values.size();
  PMPI_Pack(&size, 1, MPI_INT, buf, bufsize, position, comm);
  for(unsigned i = 0; i<param_values.size(); i++)
    param_values[i]->pack(buf, bufsize, position, comm);
}


void Param::unpack(void *buf, int bufsize, int *position, MPI_Comm comm){
  PMPI_Unpack(buf, bufsize, position, &param_type, 1, MPI_INT, comm);
  int size, type;
  PMPI_Unpack(buf, bufsize, position, &size, 1, MPI_INT, comm);
  for(int i=0; i<size; i++){
    PMPI_Unpack(buf, bufsize, position, &type, 1, MPI_INT, comm);
    /* 0 -- ParamValue, 1 -- Histogram */
    ValueSet *vs;
    if( type == 0 ){
      vs = new ParamValue();
    } else {
      vs = new Histogram();
    }
    vs->unpack(buf, bufsize, position, comm);
    param_values.push_back(vs);
  }
}

