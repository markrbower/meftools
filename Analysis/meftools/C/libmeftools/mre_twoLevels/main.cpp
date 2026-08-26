#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "A.h"
#include "B.h"

using namespace std;

int main (int argc, const char * argv[]) {

	B b;
	A a = A( b );   // B should assign internal value to 1 ...
	cout << a.status() << endl;     // ... and so print 1, here.
	
	B b2 = B(2);	// The internal value is assigned to 2 ...
	A a2 = A( b2 );
	cout << a2.status() << endl;    // ... and so print 2, here.

}

