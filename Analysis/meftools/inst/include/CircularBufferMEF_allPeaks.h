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

class CircularBufferMEF_allPeaks : public CircularBufferMEF {
    private:
    public:
	CircularBufferMEF_allPeaks(int capacity, long long startTime, long long stepTime ) :
			CircularBufferMEF{capacity, startTime, stepTime} {
	};

	bool isPeak();
};
#endif

