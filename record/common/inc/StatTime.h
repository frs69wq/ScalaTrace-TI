/**
 * Author: Xing Wu
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
