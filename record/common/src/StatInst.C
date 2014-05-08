/*
 * Copyright (c) 2014
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "StatInst.h"
#include <iostream>

StatInst::StatInst(int s):
    Stat(s)
{
  int event_code;
  PAPI_library_init(PAPI_VER_CURRENT);
  if(PAPI_create_eventset(&EventSet)!=PAPI_OK) {
     cout << "Could not create the EventSet" << endl;
  }
  PAPI_event_name_to_code("PAPI_TOT_INS",&event_code);

  PAPI_add_event(EventSet, event_code);

}

void StatInst::start(){

}

long long StatInst::end(){
  return 0;
}
