#include <iostream>
#include <vector>

#include "CircularBuffer.h"
#include "CircularBufferMEF.h"
#include "CircularBufferMEF_allPeaks.h"

using namespace std;

CircularBufferMEF_allPeaks::CircularBufferMEF_allPeaks(int capacity_, long long startTime_, long long stepTime_ ) {
        cout << "CircularBufferMEF_allPeaks constructor." << endl;
        this->CircularBufferMEF::capacity = capacity_;
        this->CircularBufferMEF::centerTime = startTime_ - stepTime_;
        this->CircularBufferMEF::timeStep = stepTime_;
        cout << "CircularBufferMEF_allPeaks constructed." << endl;
};

bool CircularBufferMEF_allPeaks::isPeak() {
	// A peak occurs when the center value:
	// 1. idx is the max of the buffer
	int result = 1;

	if ( full() ) {
		int idx = 0;
		while ( idx < capacity & result == 1 ) {
			if ( abs(buffer[idx]) > abs(buffer[mid]) ) {
				result = 0; // Req 1
			}
			idx++;
		}
	} else {
		result = 0;
	}
	return result;
}

