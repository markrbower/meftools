#include <iostream>
#include <vector>

#include "CircularBuffer.h"
#include "CircularBufferMEF.h"
#include "CircularBufferMEF_allPeaks.h"

using namespace std;

bool CircularBufferMEF_allPeaks::isPeak() {
	// A peak occurs when the center value:
	// 1. idx is the max of the buffer
	int result = 1;

	int idx = 0;
	while ( idx < capacity & result == 1 ) {
		if ( buffer[idx] > buffer[mid] ) {
			result = 0; // Req 1
		}
		idx++;
	}

	return result;
}

