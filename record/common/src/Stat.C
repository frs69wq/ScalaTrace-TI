/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "Stat.h"

Stat::Stat(int s):
stattype(s)
{}

Stat::Stat(){}

Stat::~Stat(){
  map<StackSig, Histogram *>::iterator it;
  for(it = pathstats.begin(); it != pathstats.end(); it++){
    if(it->second)
      delete it->second;
  }
}

string Stat::toString(int phase){
  ostringstream rtn;
  map<StackSig, Histogram *>::iterator it;
  int i=0;
  for(it = pathstats.begin(); it != pathstats.end(); it++, i++){
    if(phase == PHASE_COMP){
      rtn << "COMP ";
    } else if (phase == PHASE_COMM) {
      rtn << "COMM ";
    }
    rtn << hex << stattype << "-" << i << ": ";
    rtn << it->first.toString() << " ";
    if(it->second)
      rtn << it->second->toString();
    rtn << "\n";
  }
  return rtn.str();
}

void Stat::input(string& buf){
  string strsig = buf.substr(0, buf.find("("));
  StackSig sig;
  sig.input(strsig);
  Histogram *h = new Histogram();
  if(!h)
    exit(1);
  string histo = buf.substr(buf.find("("));
  h->input(histo);
  pathstats[sig] = h;
}

void Stat::resetStat(){
  start();
}

void Stat::recordStat(StackSig *predSig){
  long long value;
  extern int my_rank;

  value = end();
  map<StackSig, Histogram *>::iterator it = pathstats.find(*predSig);
  if(it == pathstats.end()){
    Histogram *h = new Histogram();
    h->add((double)value, my_rank);
    pathstats[*predSig] = h;
  } else {
    it->second->add((double)value, my_rank);
  }
}

double Stat::getValue(StackSig* sig){
  if(!sig)
    return 0;
  map<StackSig, Histogram *>::iterator it = pathstats.find(*sig);
  if(it != pathstats.end() ){
    if(it->second)
      return it->second->avg();
    else
      return 0;
  } else {
    return 0;
  }
}

void Stat::merge(Stat *other){
  if(other == NULL)
    return;

  map<StackSig, Histogram *> *otherstats = other->getPathstats();
  map<StackSig, Histogram *>::iterator iter;
  map<StackSig, Histogram *>::iterator found;

  for(iter = otherstats->begin(); iter != otherstats->end(); iter++){
    found = pathstats.find(iter->first);
    if(found == pathstats.end() ){
      pathstats[iter->first] = iter->second;
      iter->second = NULL;
    } else {
      found->second->merge(iter->second);
    }
  }
}

void Stat::updateSig(StackSig* oldSig, StackSig* newSig){
  if(oldSig->equals(newSig))
    return;

  map<StackSig, Histogram *>::iterator found_old = pathstats.find(*oldSig);
  map<StackSig, Histogram *>::iterator found_new;
  if(found_old != pathstats.end() ){
    found_new = pathstats.find(*newSig);
    if(found_new == pathstats.end()) {
      pathstats[*newSig] = found_old->second;
      pathstats[*oldSig] = NULL;
    } else {
      found_new->second->merge(found_old->second);
    }
    pathstats.erase(found_old);
  }
}

int Stat::getPackedSize(){
  int size = 0;
  size += sizeof(int); /* stattype */
  size += sizeof(int); /* num of paths */
  map<StackSig, Histogram *>::iterator it;
  for(it=pathstats.begin(); it!=pathstats.end(); it++){
    size += it->first.getPackedSize();
    size += it->second->getPackedSize();
  }
  return size;
}

void Stat::pack(void *buf, int bufsize, int *position, MPI_Comm comm){
  PMPI_Pack(&stattype, 1, MPI_INT, buf, bufsize, position, comm);
  int size = pathstats.size();
  PMPI_Pack(&size, 1, MPI_INT, buf, bufsize, position, comm);
  map<StackSig, Histogram *>::iterator it;
  StackSig ss;
  for(it=pathstats.begin(); it!=pathstats.end(); it++){
    ss = it->first;
    ss.pack(buf, bufsize, position, comm);
    it->second->pack(buf, bufsize, position, comm);
  }
}

void Stat::unpack(void *buf, int bufsize, int *position, MPI_Comm comm){
  PMPI_Unpack(buf, bufsize, position, &stattype, 1, MPI_INT, comm);
  int size = 0, type;
  PMPI_Unpack(buf, bufsize, position, &size, 1, MPI_INT, comm);
  StackSig *s;
  Histogram *h;
  for(int i=0; i<size; i++){
    s = new StackSig();
    s->unpack(buf, bufsize, position, comm);
    PMPI_Unpack(buf, bufsize, position, &type, 1, MPI_INT, comm);
    h = new Histogram();
    h->unpack(buf, bufsize, position, comm);
    pathstats[*s] = h;
    delete s;
  }
}
