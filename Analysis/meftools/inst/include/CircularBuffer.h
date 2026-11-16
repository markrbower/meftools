#ifndef CIRCULAR_BUFFER
#define CIRCULAR_BUFFER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

using namespace std;

class CircularBuffer {
    private:
    public:
    	vector<double> buffer;
        int head;
        int mid;
        int tail;
        int capacity;
	int count;
	int midSample;

        CircularBuffer();
        CircularBuffer(int capacity);
        virtual void push_tail(double val);
        double getHead();
        double getMid();
        double getTail();
        void printBuffer();
        vector<double> getBuffer();
        bool isPeak();
        bool isPeak( float parm1, float parm2 );
	virtual void reset( long long timeStart, long long timeStep );
	virtual long long getTime();
	virtual CircularBuffer* duplicate();
	int circularDistance( int from, int to );
	int computeKB();
};
#endif

