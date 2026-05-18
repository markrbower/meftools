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

using namespace std;

class CircularBufferMEF : public CircularBuffer {
    private:
	long long centerTime;
	long long timeStep;
    public:
	CircularBufferMEF(int capacity, long long startTime, long long step=1 ) :
		CircularBuffer{capacity},
		centerTime{startTime - step},
		timeStep{step}
	{ };

	long long getTime();
};
#endif

