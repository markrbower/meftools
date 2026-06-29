#ifndef CIRCULAR_BUFFER_MEF
#define CIRCULAR_BUFFER_MEF

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

#include "CircularBuffer.h"

using namespace std;

class CircularBufferMEF : public CircularBuffer {
    private:
    public:
	long long centerTime;
	long long timeStep;

	CircularBufferMEF() : CircularBuffer{0} {

	};

	CircularBufferMEF(int capacity, long long startTime, long long stepTime ) :
		CircularBuffer{capacity} {
	    centerTime = startTime - stepTime;
	    timeStep = stepTime;	
	};

	long long getTime();

	void reset( long long startTime, long long timeStep );

	void push_back( double value );

	virtual bool isPeak() {};
};
#endif

