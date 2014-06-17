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
long long StatInst::values [1];

StatInst::StatInst(int s):
    Stat(s)
{
  if(PAPI_accum_counters(values, 1)!=PAPI_OK)
    cerr << "PAPI event is not supported" << endl;
}

void StatInst::start(){
  PAPI_accum_counters(values, 1);
  StatInst::counter = values[0];
}

long long StatInst::end(){
  long long current_counter_value;
  PAPI_accum_counters(values, 1);
  current_counter_value = values[0];
//  cout << "end() called: " << values[0] -StatInst::counter
//      << " counted instructions" <<endl;
  return (values[0]-StatInst::counter)/1000;  // Dividing by 1000 to avoid overflows
					      // without loosing too much accuracy
}
