#include <iostream>
#include <vector>

#include "CircularBuffer.h"
#include "CircularBufferMEF.h"

using namespace std;

long long CircularBufferMEF::getTime() { return centerTime; }

void CircularBufferMEF::reset( long long timeStart, long long stepSize ) {
	centerTime = timeStart - stepSize;
	timeStep = stepSize;
};

void CircularBufferMEF::push_back( double value ) {
	CircularBuffer::push_back( value );
	centerTime += timeStep;
}

