#include <iostream>
#include <vector>

#include "CircularBuffer.h"

using namespace std;

//CircularBuffer::CircularBuffer() {
//	capacity = 0;
//	midSample = -1;
//}

CircularBuffer::CircularBuffer(int _capacity) {
        
        // If the capacity is invalid
        if (_capacity < 0) {
            throw invalid_argument("Invalid capacity");
        }
        this->capacity = _capacity;
        this->head = 0;
        this->mid = 0;
        this->back = 0;
	this->midSample = -1; // The first sample will be #0.
        buffer.resize(capacity);
}

// Function to add an element to the buffer
void CircularBuffer::push_back(double val) {
        buffer[head] = val;
        if (full()) {
	        tail  = (tail + 1)  % capacity;
	        head = (head + 1) % capacity;
		mid   = (mid + 1)   % capacity;
		midSample++;
        } else {
	        head  = (head + 1);
		if ( size() % 2 && size() > 2 ) { // size() is odd
			mid = (mid + 1) % capacity;
			midSample++;
		}	
	}
}

double CircularBuffer::getHead() {
        if (empty()) {
            throw out_of_range("CircularBuffer is empty");
        }
        return buffer[head];
}
    
double CircularBuffer::getMid() {
        if (empty()) {
            throw out_of_range("CircularBuffer is empty");
        }
        return buffer[mid];
}
    
double CircularBuffer::getTail() {
        if (empty()) {
            throw out_of_range("CircularBuffer is empty");
        }
        return buffer[tail];
}

// Function to check if the buffer is empty
bool CircularBuffer::empty() { return head == tail; }

// Function to check if the buffer is full
bool CircularBuffer::full() {
        return (tail + 1) % capacity == head;
}

// Function to get the size of the buffer
int CircularBuffer::size() {
        if (tail >= head) {
            return (back+capacity) - front;
        }
        return capacity - (front - back);
}

// Function to print the elements of the buffer
void CircularBuffer::printBuffer() {
        int idx = front;
        while (idx != back) {
            cout << buffer[idx] << " ";
            idx = (idx + 1) % capacity;
        }
        cout << endl;
}

vector<double> CircularBuffer::getBuffer() {
	cout << "CircularBuffer: get()" << endl;
        vector<double> buf;
	buf.reserve( capacity );
	cout << front << endl;
	cout << back << endl;
	cout << capacity << endl;
	// Prime the loop so that a simple, fast check allows 'back'.
        int idx = front;
        buf.push_back( buffer[idx] );
        idx = (idx + 1) % capacity;
        while (idx != front) {
//	    cout << idx << ": " << buffer[idx] << endl;
            buf.push_back( buffer[idx] );
//	    cout << idx << " : " << capacity << endl;
            idx = (idx + 1) % capacity;
        }
        cout << "buf size: " << buf.size() << endl;
	return( buf );
}

bool CircularBuffer::isPeak() {
        if (full()) {
            int idx = front;
            while (idx != back) {
                if ( idx != mid ) {
                    if ( buffer[idx] >= buffer[mid] )
                        return 0;
                }
                idx = (idx + 1) % capacity;
            }
            return 1;
        }
        return 0;
}            

void CircularBuffer::reset( long long timeStart, long long timeStep ) {}

long long CircularBuffer::getTime() {
	cout << "CircularBuffer::getTime() should not be called." << endl;
	return 0L;
};

CircularBuffer* CircularBuffer::duplicate() {
	static CircularBuffer newbuf( capacity );
	return &newbuf;	
}

int CircularBuffer::computeKB() {
	if ( buffer.size() == 0 ) {
		return 0;
	}
	// Compute the square (i.e., energy) at each point, keeping track of the max and its index.
	int maxIdx = 0;
	double maxValue = buffer[0];
	for ( size_t i=1; i<buffer.size(); i++ ) {
		double sqv = buffer[i] * buffer[i];
		if ( sqv > maxValue ) {
			maxIdx = i;
			maxValue = sqv;
		}
	}

	int idxDiff = circularDistance( maxIdx, mid );
/*
	// Compute the relative distance from "mid" to "max".
	// What if the maxIdx is beyond the wrap-around; i.e., capacity=32, mid=30, max=2?
	// Compute forward and backward, take the smaller absolute diff, then add.
	int d1 = (maxIdx - mid) % capacity;	
	int d2 = (mid - maxIdx) % capacity;	
	int idxDiff;
	if ( d1 < d2 ) {
		idxDiff = d1;
	} else {
		idxDiff = -d2;
	}
*/
	// Compute the absolute index.
	int absIdx = midSample + idxDiff;

	// Return the absolute index.
	return absIdx;
}

int CircularBuffer::circularDistance( int fromIdx, int toIdx ) {
	// Compute forward and backward, take the smaller absolute diff, then add.
	int idxDiff;
	int d1 = (fromIdx - toIdx) % capacity;	
	int d2 = (toIdx - fromIdx) % capacity;	
	if ( d1 < d2 ) {
		idxDiff = d1;
	} else {
		idxDiff = -d2;
	}
	return idxDiff;
}

/*
int main() {

    // Create a buffer of size 5
    CircularBuffer buffer(5);

    // Add elements to the buffer
    for (int i = 1; i <= 5; ++i) {
        buffer.push_back(i);
        cout << "Added: " << i << endl;
    }

    // Print elements of the buffer after adding elements
    cout << "Elements of the buffer: ";
    buffer.printBuffer();

    // Check the size of the circular buffer before deletion
    // of elements
    cout << "Buffer size: " << buffer.size() << endl;

    // Remove elements from the buffer
    for (int i = 0; i < 2; ++i) {
        cout << "Removed: " << buffer.getFront() << endl;
        buffer.pop_front();
    }
    // Print elements of the buffer after removing elements
    cout << "Elements of the buffer: ";
    buffer.printBuffer();

    // Check the size of the circular buffer after deletion
    // of elements
    cout << "Buffer size: " << buffer.size() << endl;
    cout << "Current Back: " << buffer.getBack() << endl;
    cout << "Current Front: " << buffer.getFront();
    return 0;
}
*/
