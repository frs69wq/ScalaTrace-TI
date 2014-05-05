/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "StackSig.h"

extern "C" intptr_t * do_stack_unwind ();
extern "C" void       do_stack_free   (intptr_t*);

StackSig::StackSig(){}
StackSig::~StackSig(){}

StackSig::StackSig(const StackSig& copy):
    sig( *(copy.getSig()) )
{}

bool StackSig::operator<(const StackSig& other) const{
  return sig < *other.getSig();
}

void StackSig::unwind(){
  intptr_t * uw_sig = do_stack_unwind ();
  int i = 0, flag;
  sig.push_back(uw_sig[i++]);
  while(uw_sig[i]){
    flag = 0;
    for(unsigned j=0; j<sig.size(); j++)
      if(sig[j] == uw_sig[i]){
        flag = 1;
        break;
      }
    if(!flag)
      sig.push_back(uw_sig[i]);
    i++;
  }
  do_stack_free (uw_sig);
}

bool StackSig::match(StackSig *other){
#if defined FEATURE_SIG_DIFF && FEATURE_SIG_DIFF > 0
  if(sig == *(other->getSig()))
    return true;

  const vector<intptr_t> *other_sig = other->getSig();
  int diff_degree = FEATURE_SIG_DIFF;
  int size = sig.size();
  int other_size = other_sig->size();

  /*
   * returns TRUE only if two signatures have the same length
   * and they are different by at most diff_degree frame
   */

  /* check length */
  if(size != other_size)
    return false;

  /* compare each frame */
  for(int i = 0; i < size; i++){
    if(sig[i] != other_sig->at(i)){
      diff_degree--;
      if(diff_degree < 0)
        return false;
    }
  }
  return true;
#else
  return sig == *(other->getSig());
#endif
}

bool StackSig::equals(StackSig *other){
  return sig == *(other->getSig());
}

const vector<intptr_t> * StackSig::getSig() const{
  return &sig;
}

string StackSig::toString() const{
  ostringstream rtn;
  if(sig.size() <= 0)
    return "NULL";

  rtn << hex << sig[0];
  for(unsigned i=1; i<sig.size(); i++)
    rtn << "_" << hex << sig[i];
  return rtn.str();
}

void StackSig::input(string& buf){
  size_t pos = buf.find("_");
  while(pos != string::npos){
    buf.replace(pos, 1, " ");
    pos = buf.find("_", pos+1);
  }
  intptr_t addr;
  istringstream ss(buf);
  while(ss >> hex >> addr)
    sig.push_back(addr);
}

int StackSig::getPackedSize() const{
  return sizeof(int) + sizeof(intptr_t) * sig.size();
}

void StackSig::pack(void *buf, int bufsize, int *position, MPI_Comm comm){
  int size = sig.size();
  PMPI_Pack(&size, 1, MPI_INT, buf, bufsize, position, comm);
  for(int i=0; i<size; i++)
    PMPI_Pack(&(sig[i]), 1, MPI_INTPTR_TYPE, buf, bufsize, position, comm);
}

void StackSig::unpack(void *buf, int bufsize, int *position, MPI_Comm comm){
  int size;
  intptr_t value;
  PMPI_Unpack(buf, bufsize, position, &size, 1, MPI_INT, comm);
  for(int i=0; i<size; i++){
    PMPI_Unpack(buf, bufsize, position, &value, 1, MPI_INTPTR_TYPE, comm);
    sig.push_back(value);
  }
}
