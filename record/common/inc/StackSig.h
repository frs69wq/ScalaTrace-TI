/**
 * Author: Xing Wu
 */

#ifndef __STACKSIG_H__
#define __STACKSIG_H__

#include <vector>
#include <string>
#include <sstream>
#include <mpi.h>

#define MPI_INTPTR_TYPE \
	((sizeof (int) == sizeof(intptr_t))            ? MPI_INT : \
	(sizeof (long int) == sizeof (intptr_t))      ? MPI_LONG : /* not MPI_LONG_INT !*/ \
	(sizeof (long long int) == sizeof (intptr_t)) ? MPI_LONG_LONG_INT : \
	MPI_INT)

#define ABS(a,b) (((a) >= (b)) ? ((a) - (b)) : ((b) - (a)))

using namespace std;

class StackSig{
	
private:
	vector<intptr_t> sig;

public:
	StackSig();
	StackSig(const StackSig& copy);
	~StackSig();

	bool match(StackSig *other);
	bool equals(StackSig *other);
	const vector<intptr_t> *getSig() const;
	void unwind();

	bool operator<(const StackSig& other) const;

	string toString() const;
	void input(string& buf);

	int getPackedSize() const;
	void pack(void *buf, int bufsize, int *position, MPI_Comm comm);
	void unpack(void *buf, int bufsize, int *position, MPI_Comm comm);

};

#endif /*__STACKSIG_H__*/
