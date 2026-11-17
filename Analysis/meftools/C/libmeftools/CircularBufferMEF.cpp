#include <iostream>
#include <vector>

#include "CircularBuffer.h"
#include "CircularBufferMEF.h"

using namespace std;

long long CircularBufferMEF::getTime() { return centerTime; }

void CircularBufferMEF::reset( long long timeStart, long long stepSize ) {
	centerTime = timeStart - stepSize;
	timeStep = stepSize;
	cout << "CircularBufferMEF.reset(): " << centerTime << "\t" << timeStep << endl;
};

void CircularBufferMEF::push( double value ) {
	CircularBuffer::push( value );
	centerTime += timeStep;
}

CircularBuffer* CircularBufferMEF::duplicate() {
        static CircularBuffer newbuf = CircularBufferMEF( capacity, CircularBufferMEF::centerTime, CircularBufferMEF::timeStep );
        return &newbuf;  
}       
        

