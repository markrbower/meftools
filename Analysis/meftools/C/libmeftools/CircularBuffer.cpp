#include <iostream>
#include <vector>

#include "CircularBuffer.h"

using namespace std;

CircularBuffer::CircularBuffer(int _capacity) {
        
        // If the capacity is invalid
        if (_capacity < 0) {
            throw invalid_argument("Invalid capacity");
        }
        this->capacity = _capacity + 1;
        this->front = 0;
        this->mid = 0;
        this->back = 0;
        buffer.resize(capacity);
    }

    // Function to add an element to the buffer
void CircularBuffer::push_back(int val) {
        buffer[back] = val;
        if (full()) {
	        back  = (back + 1)  % capacity;
	        front = (front + 1) % capacity;
		mid   = (mid + 1)   % capacity;
//            throw overflow_error("CircularBuffer is full");
        } else {
	        back  = (back + 1);
		if ( size() % 2 && size() > 2 ) { // size() is odd
			mid = (mid + 1) % capacity;
		}	
	}
    }

    // Function to remove an element from the buffer
void CircularBuffer::pop_front() {
        if (empty()) {
            throw underflow_error("CircularBuffer is empty");
        }
        if ( (size() % 2)==0 ) { // Currently has even length
		mid = (mid+1) % capacity;
	}
        front = (front + 1) % capacity;
    }
    
int CircularBuffer::getFront() {
        if (empty()) {
            throw out_of_range("CircularBuffer is empty");
        }
        return buffer[front];
    }
    
int CircularBuffer::getMid() {
        if (empty()) {
            throw out_of_range("CircularBuffer is empty");
        }
        return buffer[mid];
    }
    
int CircularBuffer::getBack() {
        if (empty()) {
            throw out_of_range("CircularBuffer is empty");
        }
        return (back == 0) ? buffer[capacity - 1] : buffer[back - 1];
    }

    // Function to check if the buffer is empty
bool CircularBuffer::empty() { return front == back; }

    // Function to check if the buffer is full
bool CircularBuffer::full() {
        return (back + 1) % capacity == front;
    }

    // Function to get the size of the buffer
int CircularBuffer::size() {
        if (back >= front) {
            return back - front;
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

vector<int> CircularBuffer::getBuffer() {
        vector<int> buf;
        int idx = front;
        while (idx != back) {
            buf.push_back( buffer[idx] );
            idx = (idx + 1) % capacity;
        }
        cout << endl;
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
