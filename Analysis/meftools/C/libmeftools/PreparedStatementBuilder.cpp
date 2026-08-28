#include <iostream>
#include <vector>

#include "PreparedStatementBuilder.h"

using namespace std;

PreparedStatementBuilder::PreparedStatementBuilder() {
	initialized = 0;
}

PreparedStatementBuilder::PreparedStatementBuilder(string tableName, map<string,string> insertThis ) {
	initialized = 1;
        
	}

PreparedStatementBuilder::PreparedStatementBuilder(string tableName, list< map<string,string> > insertThese ) {
	initialized = 1;
        
	}

// TESTING
/*
int main() {

    // Create a buffer of size 5
    PreparedStatementBuilder buffer(5);

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
