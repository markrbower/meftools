#include <iostream>
#include <vector>

#include "CircularBuffer.h"
#include "CircularBufferMEF.h"
#include "CircularBufferMEF_threshold.h"

using namespace std;

bool CircularBufferMEF_threshold::isPeak() {
	// A peak occurs when the center value:
	// 1. is greater than threshold
	// 2. is the max of the buffer
	int result = 1;

	int idx = 0;
	if ( buffer[mid] >= threshold ) { // Requirement 1
		while ( idx < capacity & result == 1 ) {
			if ( buffer[idx] > buffer[mid] ) {
				result = 0; // Req 2
			}
		idx++;
		}
	} else {
		result = 0;
	}

	return result;
}

