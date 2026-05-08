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
    	vector<double> buffer;
        int front;
        int mid;
        int center;
        int back;
        int capacity;
    public:
        CircularBuffer(int capacity);
        void push_back(double val);
        void pop_front();
        double getFront();
        double getMid();
        double getBack();
        bool empty();
        bool full(); 
        int size();
        void printBuffer();
        vector<double> getBuffer();
        bool isPeak();
};
#endif

