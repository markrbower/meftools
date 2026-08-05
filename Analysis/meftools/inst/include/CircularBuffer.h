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
        int front;
        int mid;
        int center;
        int back;
        int capacity;

        CircularBuffer(int capacity);
        virtual void push_back(double val);
        void pop_front();
        double getFront();
        double getMid();
        double getBack();
	int getCapacity();
        bool empty();
        bool full(); 
        int size();
        void printBuffer();
        vector<double> getBuffer();
        bool isPeak();
        bool isPeak( float parm1, float parm2 );
//	virtual void reset( long long timeStart, long long timeStep );
};
#endif

