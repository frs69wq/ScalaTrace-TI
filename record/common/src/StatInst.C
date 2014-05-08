/*
 * Copyright (c) 2014
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "StatInst.h"


StatInst::StatInst(int s):
    Stat(s)
{}

void StatInst::start(){

}

long long StatInst::end(){
  return 0;
}
