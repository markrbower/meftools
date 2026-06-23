#ifndef CIRCULAR_BUFFER_MEF_THRESHOLD
#define CIRCULAR_BUFFER_MEF_THRESHOLD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

#include "CircularBuffer.h"
#include "CircularBufferMEF.h"

using namespace std;

class CircularBufferMEF_threshold : public virtual CircularBuffer, public virtual CircularBufferMEF {
    private:
	double threshold;
    public:
	CircularBufferMEF_threshold(int capacity, long long startTime, long long stepTime, double t_ ) :
		CircularBuffer{capacity}, CircularBufferMEF{capacity, startTime, stepTime} {

		threshold = t_;
	};

	bool isPeak();
};
#endif

