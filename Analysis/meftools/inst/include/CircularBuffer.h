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
    	vector<int> buffer;
        int front;
        int mid;
        int center;
        int back;
        int capacity;
    public:
        CircularBuffer(int capacity);
        void push_back(int val);
        void pop_front();
        int getFront();
        int getMid();
        int getBack();
        bool empty();
        bool full(); 
        int size();
        void printBuffer();
        vector<int> getBuffer();
        bool isPeak();
};
#endif

