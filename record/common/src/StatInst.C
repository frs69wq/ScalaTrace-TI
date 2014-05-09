/*
 * Copyright (c) 2014
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "StatInst.h"
#include <iostream>

long long StatInst::counter;
int StatInst::Events[30];

StatInst::StatInst(int s):
    Stat(s)
{
  int EventSet = PAPI_NULL;
  int event_code;
  PAPI_library_init(PAPI_VER_CURRENT);
  if(PAPI_create_eventset(&EventSet)!=PAPI_OK) {
     cerr << "Could not create the EventSet" << endl;
  }
  PAPI_event_name_to_code("PAPI_TOT_INS",&event_code);

  PAPI_add_event(EventSet, event_code);
  StatInst::Events[0]=event_code;

  if(PAPI_start_counters(Events,1)!=PAPI_OK)
    cerr << "Unable to start counters for event code %d" << event_code << endl;
  else
    cout << "Start counters for event code %d" << event_code << endl;

  if(PAPI_accum_counters(values, 1)!=PAPI_OK)
    cerr << "PAPI event is not supported" << endl;

  //TODO See later what these Georges's variable do
  //  ins1=values[0];
  //  t1=values[0];


}
StatInst::~StatInst(){
  PAPI_stop_counters(values, 1);
}
void StatInst::start(){
  PAPI_accum_counters(values, 1);
  StatInst::counter = values[0];
}

long long StatInst::end(){
  long long current_counter_value;
  PAPI_accum_counters(values, 1);
  current_counter_value = values[0];
  return current_counter_value - StatInst::counter;
}
