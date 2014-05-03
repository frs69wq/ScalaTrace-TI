/**
 * Author: Xing Wu
 */

#include "StatTime.h"

struct timeval StatTime::timestamp;

StatTime::StatTime(int s):
Stat(s)
{}

void StatTime::start(){
	gettimeofday(&(StatTime::timestamp), NULL);
}

long long StatTime::end(){
	struct timeval now;
	gettimeofday(&now, NULL);
	long long whole = now.tv_sec - (StatTime::timestamp).tv_sec;
	long long frac = now.tv_usec - (StatTime::timestamp).tv_usec;
	return whole * 1000000 + frac;
}
