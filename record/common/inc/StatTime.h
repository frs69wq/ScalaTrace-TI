/*
 * Copyright (c) up to May 2, 2014: Xing Wu <xwu3@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#ifndef __STATTIME_H__
#define __STATTIME_H__

#include "Stat.h"
#include <ctime>
#include <sys/time.h>

class StatTime : public Stat{

public:
	static struct timeval timestamp;

	StatTime(int s);

	void start();
	long long end();

};

#endif /*__STATTIME_H__*/
